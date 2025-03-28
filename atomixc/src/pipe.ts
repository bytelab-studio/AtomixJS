import * as acorn from "acorn";
import {DataSectionBuilder} from "./format/data";
import {StringTableBuilder} from "./format/string-table";
import {ConstantDoubleOperand, ConstantIntegerOperand, ConstantUNumberOperand, Instruction, Opcodes} from "./opcodes";

const pipe: Record<string, (node: any, ctx: PipeContext) => void> = {};

export interface PipeContext {
    data: DataSectionBuilder;
    stringTable: StringTableBuilder;
}

export function beginPipe(program: acorn.Program, ctx: PipeContext) {
    pipeNode(program, ctx);
}

function pipeNode(node: any, ctx: PipeContext) {
    if (!(node.type in pipe)) {
        throw `Type "${node.type}" is not declared in the pipe.`;
    }
    pipe[node.type](node, ctx);
}

pipe["Program"] = (node: acorn.Program, ctx: PipeContext) => {
    for (const item of node.body) {
        pipeNode(item, ctx);
        if (item.type == "FunctionDeclaration") {
            ctx.data.addInstruction(new Instruction(Opcodes.POP));
        }
    }
}

pipe["Literal"] = (node: acorn.Literal, ctx: PipeContext) => {
    if (node.value == null) {
        ctx.data.addInstruction(new Instruction(Opcodes.LD_NULL));
        return;
    }

    switch (typeof node.value) {
        case "string":
            const idx = ctx.stringTable.registerString(node.value);
            ctx.data.addInstruction(new Instruction(Opcodes.LD_STRING).addOperand(new ConstantUNumberOperand(idx, "short")));
            break;
        case "number":
            if (node.value % 1 == 0) {
                ctx.data.addInstruction(new Instruction(Opcodes.LD_INT).addOperand(new ConstantIntegerOperand(node.value)));
            } else {
                ctx.data.addInstruction(new Instruction(Opcodes.LD_DOUBLE).addOperand(new ConstantDoubleOperand(node.value)));
            }
            break;
        case "boolean":
            if (node.value) {
                ctx.data.addInstruction(new Instruction(Opcodes.LD_TRUE));
            } else {
                ctx.data.addInstruction(new Instruction(Opcodes.LD_FALSE));
            }
            break;
        default:
            throw "Unsupported literal type" + typeof node.value;
    }
}

pipe["Identifier"] = (node: acorn.Identifier, ctx: PipeContext) => {

    if (node.name == "undefined") {
        ctx.data.addInstruction(new Instruction(Opcodes.LD_UNDF));
        return;
    }
    const idx = ctx.stringTable.registerString(node.name);
    ctx.data.addInstruction(new Instruction(Opcodes.LOAD_LOCAL).addOperand(new ConstantUNumberOperand(idx, "short")));
}

pipe["ExpressionStatement"] = (node: acorn.ExpressionStatement, ctx: PipeContext) => {
    pipeNode(node.expression, ctx);
    ctx.data.addInstruction(new Instruction(Opcodes.POP));
}

pipe["BinaryExpression"] = (node: acorn.BinaryExpression, ctx: PipeContext) => {
    pipeNode(node.left, ctx);
    pipeNode(node.right, ctx);
    switch (node.operator) {
        case "+":
            ctx.data.addInstruction(new Instruction(Opcodes.ADD));
            break;
        case "-":
            ctx.data.addInstruction(new Instruction(Opcodes.MINUS));
            break;
        case "|":
            ctx.data.addInstruction(new Instruction(Opcodes.BINARY_OR));
            break;
        default:
            throw "Unsupported operator " + node.operator;
    }
}

pipe["CallExpression"] = (node: acorn.CallExpression, ctx: PipeContext) => {
    for (const argument of node.arguments.reverse()) {
        pipeNode(argument, ctx);
    }
    pipeNode(node.callee, ctx);
    ctx.data.addInstruction(new Instruction(Opcodes.CALL).addOperand(new ConstantUNumberOperand(node.arguments.length, "short")));
}

pipe["ObjectExpression"] = (node: acorn.ObjectExpression, ctx: PipeContext) => {
    ctx.data.addInstruction(new Instruction(Opcodes.OBJ_ALLOC));
    for (const property of node.properties as acorn.Property[]) {
        if (property.key.type != "Identifier") {
            throw "Undefined key type";
        }
        ctx.data.addInstruction(new Instruction(Opcodes.DUP));
        const idx: number = ctx.stringTable.registerString(property.key.name);
        pipeNode(property.value, ctx);
        ctx.data.addInstruction(new Instruction(Opcodes.OBJ_STORE).addOperand(new ConstantUNumberOperand(idx, "short")));
    }
}

pipe["MemberExpression"] = (node: acorn.MemberExpression, ctx: PipeContext) => {
    pipeNode(node.object, ctx);
    if (node.computed) {
        throw "Computed property are not supported";
    }
    if (node.property.type != "Identifier") {
        throw "Undefined property";
    }
    const idx: number = ctx.stringTable.registerString(node.property.name);
    ctx.data.addInstruction(new Instruction(Opcodes.OBJ_LOAD).addOperand(new ConstantUNumberOperand(idx, "short")));
}

pipe["AssignmentExpression"] = (node: acorn.AssignmentExpression, ctx: PipeContext) => {
    if (node.left.type == "Identifier") {
        pipeNode(node.right, ctx);
        ctx.data.addInstruction(new Instruction(Opcodes.DUP));
        const idx: number = ctx.stringTable.registerString(node.left.name);
        ctx.data.addInstruction(new Instruction(Opcodes.STORE_LOCAL).addOperand(new ConstantUNumberOperand(idx, "short")));
        return;
    }
    if (node.left.type == "MemberExpression") {
        if (node.left.computed) {
            throw "Computed property are not supported";
        }
        if (node.left.property.type != "Identifier") {
            throw "Undefined property";
        }

        pipeNode(node.right, ctx);
        ctx.data.addInstruction(new Instruction(Opcodes.DUP));
        pipeNode(node.left.object, ctx);
        ctx.data.addInstruction(new Instruction(Opcodes.SWAP));
        const idx: number = ctx.stringTable.registerString(node.left.property.name);
        ctx.data.addInstruction(new Instruction(Opcodes.OBJ_STORE).addOperand(new ConstantUNumberOperand(idx, "short")));
    }
}

pipe["VariableDeclaration"] = (node: acorn.VariableDeclaration, ctx: PipeContext) => {
    for (const declarator of node.declarations) {
        pipeNode(declarator, ctx);
    }
}

pipe["VariableDeclarator"] = (node: acorn.VariableDeclarator, ctx: PipeContext) => {
    if (node.init) {
        pipeNode(node.init, ctx);
    } else {
        ctx.data.addInstruction(new Instruction(Opcodes.LD_UNDF));
    }

    if (node.id.type == "Identifier") {
        const idx = ctx.stringTable.registerString(node.id.name);
        ctx.data.addInstruction(new Instruction(Opcodes.ALLOC_LOCAL).addOperand(new ConstantUNumberOperand(idx, "short")));
    } else {
        throw "Unsupported identifier " + node.id.type;
    }
}

pipe["FunctionExpression"] = pipe["FunctionDeclaration"] = (node: acorn.FunctionDeclaration | acorn.FunctionExpression, ctx: PipeContext) => {
    const funcStart: number = ctx.data.addInstruction(new Instruction(Opcodes.NOP));
    const idx: number = node.id
        ? ctx.stringTable.registerString(node.id.name)
        : -1;
    for (let i: number = 0; i < node.params.length; i++) {
        const param: acorn.Pattern = node.params[i];
        if (param.type != "Identifier") {
            throw "Unsupported param type";
        }
        ctx.data.addInstruction(new Instruction(Opcodes.LOAD_ARG).addOperand(new ConstantUNumberOperand(i, "short")));
        const idx: number = ctx.stringTable.registerString(param.name);
        ctx.data.addInstruction(new Instruction(Opcodes.ALLOC_LOCAL).addOperand(new ConstantUNumberOperand(idx, "short")));
    }

    pipeNode(node.body, ctx);
    const funcEnd: number = ctx.data.getCount();

    if (idx != -1) {
        ctx.data.replaceInstruction(
            funcStart,
            new Instruction(Opcodes.DECLARE_FUNC)
                .addOperand(new ConstantUNumberOperand(idx, "short"))
                .addOperand(new ConstantUNumberOperand(funcEnd - funcStart - 1, "short"))
        );
    } else {
        ctx.data.replaceInstruction(funcStart, new Instruction(Opcodes.DECLARE_FUNC_E).addOperand(new ConstantUNumberOperand(funcEnd - funcStart - 1, "short")));
    }
}

pipe["BlockStatement"] = (node: acorn.BlockStatement, ctx: PipeContext) => {
    ctx.data.addInstruction(new Instruction(Opcodes.PUSH_SCOPE));
    for (const item of node.body) {
        pipeNode(item, ctx);
        if (item.type == "FunctionDeclaration") {
            ctx.data.addInstruction(new Instruction(Opcodes.POP));
        }
    }
    ctx.data.addInstruction(new Instruction(Opcodes.POP_SCOPE));
}

pipe["ReturnStatement"] = (node: acorn.ReturnStatement, ctx: PipeContext) => {
    if (node.argument) {
        pipeNode(node.argument, ctx);
    }

    ctx.data.addInstruction(new Instruction(Opcodes.RETURN));
}

pipe["IfStatement"] = (node: acorn.IfStatement, ctx: PipeContext) => {
    pipeNode(node.test, ctx);
    const jmpToElseOrEnd: number = ctx.data.addInstruction(new Instruction(Opcodes.NOP));
    pipeNode(node.consequent, ctx);

    let jmpToEnd: number = -1
    if (node.alternate) {
        jmpToEnd = ctx.data.addInstruction(new Instruction(Opcodes.NOP));
        ctx.data.replaceInstruction(jmpToElseOrEnd, new Instruction(Opcodes.JMP_F).addOperand(new ConstantUNumberOperand(jmpToEnd + 1, "short")));
        pipeNode(node.alternate, ctx);
    }

    if (jmpToEnd == -1) {
        ctx.data.replaceInstruction(jmpToElseOrEnd, new Instruction(Opcodes.JMP_F).addOperand(new ConstantUNumberOperand(ctx.data.getCount(), "short")));
    } else {
        ctx.data.replaceInstruction(jmpToEnd, new Instruction(Opcodes.JMP).addOperand(new ConstantUNumberOperand(ctx.data.getCount(), "short")));
    }
}