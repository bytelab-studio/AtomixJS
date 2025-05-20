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

pipe["ThisExpression"] = (node: acorn.ThisExpression, ctx: PipeContext) => {
    ctx.data.addInstruction(new Instruction(Opcodes.LD_THIS));
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
        case "*":
            ctx.data.addInstruction(new Instruction(Opcodes.MUL));
            break;
        case "/":
            ctx.data.addInstruction(new Instruction(Opcodes.DIV));
            break;
        case "%":
            ctx.data.addInstruction(new Instruction(Opcodes.MOD));
            break;
        case "&":
            ctx.data.addInstruction(new Instruction(Opcodes.BINARY_AND));
            break;
        case "|":
            ctx.data.addInstruction(new Instruction(Opcodes.BINARY_OR));
            break;
        case "^":
            ctx.data.addInstruction(new Instruction(Opcodes.BINARY_XOR));
            break;
        case "<<":
            ctx.data.addInstruction(new Instruction(Opcodes.BINARY_LSHFT));
            break;
        case ">>":
            ctx.data.addInstruction(new Instruction(Opcodes.BINARY_RSHFT));
            break;
        case ">>>":
            ctx.data.addInstruction(new Instruction(Opcodes.BINARY_ZRSHFT));
            break;
        case "===":
            ctx.data.addInstruction(new Instruction(Opcodes.TEQ));
            break;
        case "!==":
            ctx.data.addInstruction(new Instruction(Opcodes.NTEQ));
            break;
        case ">":
            ctx.data.addInstruction(new Instruction(Opcodes.GT));
            break;
        case ">=":
            ctx.data.addInstruction(new Instruction(Opcodes.GEQ));
            break;
        case "<":
            ctx.data.addInstruction(new Instruction(Opcodes.LT));
            break;
        case "<=":
            ctx.data.addInstruction(new Instruction(Opcodes.LEQ));
            break;
        default:
            throw "Unsupported operator " + node.operator;
    }
}

pipe["UnaryExpression"] = (node: acorn.UnaryExpression, ctx: PipeContext) => {
    // TODO think about delete and '+' operator
    pipeNode(node.argument, ctx);

    switch (node.operator) {
        case "-":
            ctx.data.addInstruction(new Instruction(Opcodes.NEGATE));
            break;
        case "!":
            ctx.data.addInstruction(new Instruction(Opcodes.NOT));
            break;
        case "~":
            ctx.data.addInstruction(new Instruction(Opcodes.BINARY_NOT))
            break;
        case "typeof":
            ctx.data.addInstruction(new Instruction(Opcodes.TYPEOF))
            break;
        case "void":
            ctx.data.addInstruction(new Instruction(Opcodes.POP));
            ctx.data.addInstruction(new Instruction(Opcodes.LD_UNDF));
            break;
        default:
            throw "Unsupported operator " + node.operator;
    }
}

pipe["CallExpression"] = (node: acorn.CallExpression, ctx: PipeContext) => {
    for (const argument of node.arguments.reverse()) {
        pipeNode(argument, ctx);
    }
    if (node.callee.type == "MemberExpression") {
        pipeMemberExpression(node.callee, ctx, true);
    } else {
        ctx.data.addInstruction(new Instruction(Opcodes.LD_UNDF));
        pipeNode(node.callee, ctx);
    }
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

pipe["ArrayExpression"] = (node: acorn.ArrayExpression, ctx: PipeContext) => {
    ctx.data.addInstruction(new Instruction(Opcodes.ARR_ALLOC));
    ctx.data.addInstruction(new Instruction(Opcodes.DUP));
    const lengthIdx = ctx.stringTable.registerString("length");
    ctx.data.addInstruction(new Instruction(Opcodes.LD_INT).addOperand(new ConstantIntegerOperand(node.elements.length)));
    ctx.data.addInstruction(new Instruction(Opcodes.OBJ_STORE).addOperand(new ConstantUNumberOperand(lengthIdx, "short")));

    let i = 0;
    for (const element of node.elements) {
        ctx.data.addInstruction(new Instruction(Opcodes.DUP));
        pipeNode(element, ctx);
        const indexIdx = ctx.stringTable.registerString((i++).toString());
        ctx.data.addInstruction(new Instruction(Opcodes.OBJ_STORE).addOperand(new ConstantUNumberOperand(indexIdx, "short")));
    }
}

function pipeMemberExpression(node: acorn.MemberExpression, ctx: PipeContext, doubleObject: boolean) {
    if (node.object.type == "Super") {
        ctx.data.addInstruction(new Instruction(Opcodes.LD_THIS));
        
        if (doubleObject) {
            ctx.data.addInstruction(new Instruction(Opcodes.DUP));
        }
        
        ctx.data.addInstruction(new Instruction(Opcodes.LD_PROTO));
    } else {
        pipeNode(node.object, ctx);

        if (doubleObject) {
            ctx.data.addInstruction(new Instruction(Opcodes.DUP));
        }
    }

    if (node.computed) {
        pipeNode(node.property, ctx);
        ctx.data.addInstruction(new Instruction(Opcodes.OBJ_CLOAD));
        return;
    }
    if (node.property.type != "Identifier") {
        throw "Undefined property";
    }
    const idx: number = ctx.stringTable.registerString(node.property.name);
    ctx.data.addInstruction(new Instruction(Opcodes.OBJ_LOAD).addOperand(new ConstantUNumberOperand(idx, "short")));
}

pipe["MemberExpression"] = (node: acorn.MemberExpression, ctx: PipeContext) => pipeMemberExpression(node, ctx, false);

pipe["AssignmentExpression"] = (node: acorn.AssignmentExpression, ctx: PipeContext) => {
    if (node.operator != "=") {
        throw "Unsupported operator";
    }

    if (node.left.type == "Identifier") {
        pipeNode(node.right, ctx);
        ctx.data.addInstruction(new Instruction(Opcodes.DUP));
        const idx: number = ctx.stringTable.registerString(node.left.name);
        ctx.data.addInstruction(new Instruction(Opcodes.STORE_LOCAL).addOperand(new ConstantUNumberOperand(idx, "short")));
        return;
    }

    if (node.left.type == "MemberExpression") {
        pipeNode(node.right, ctx);
        ctx.data.addInstruction(new Instruction(Opcodes.DUP));

        if (node.left.computed || node.left.property.type != "Identifier") {
            pipeNode(node.left.object, ctx);
            pipeNode(node.left.property, ctx);
            ctx.data.addInstruction(new Instruction(Opcodes.OBJ_CSTORE));
            return;
        }

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
        ctx.data.addInstruction(new Instruction(Opcodes.LOAD_ARG).addOperand(new ConstantUNumberOperand(i + 1, "short")));
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
    if (!("virtual" in node) || !node.virtual) {
        ctx.data.addInstruction(new Instruction(Opcodes.PUSH_SCOPE));
    }
    for (const item of node.body) {
        pipeNode(item, ctx);
        if (item.type == "FunctionDeclaration") {
            ctx.data.addInstruction(new Instruction(Opcodes.POP));
        }
    }
    if (!("virtual" in node) || !node.virtual) {
        ctx.data.addInstruction(new Instruction(Opcodes.POP_SCOPE));
    }
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

pipe["WhileStatement"] = (node: acorn.WhileStatement, ctx: PipeContext) => {
    /*
        0: <condition>
        1: jmp_f 4
        2: <body>
        3: jmp 0
        4: ...
     */

    const start = ctx.data.getCount();
    pipeNode(node.test, ctx);
    const jmpEnd = ctx.data.addInstruction(new Instruction(Opcodes.POP));
    pipeNode(node.body, ctx);
    ctx.data.addInstruction(new Instruction(Opcodes.JMP).addOperand(new ConstantUNumberOperand(start, "short")));
    ctx.data.replaceInstruction(jmpEnd, new Instruction(Opcodes.JMP_F).addOperand(new ConstantUNumberOperand(ctx.data.getCount(), "short")));
}

pipe["ExportNamedDeclaration"] = (node: acorn.ExportNamedDeclaration, ctx: PipeContext) => {
    if (node.declaration) {
        pipeNode(node.declaration, ctx);
        if (node.declaration.type == "VariableDeclaration") {
            for (const declarator of node.declaration.declarations) {
                if (declarator.id.type != "Identifier") {
                    throw "Unexpected variable id";
                }
                pipeNode(declarator.id, ctx);
                const idx: number = ctx.stringTable.registerString(declarator.id.name);
                ctx.data.addInstruction(new Instruction(Opcodes.EXPORT).addOperand(new ConstantUNumberOperand(idx, "short")));
            }
        } else {
            const idx: number = ctx.stringTable.registerString(node.declaration.id.name);
            ctx.data.addInstruction(new Instruction(Opcodes.EXPORT).addOperand(new ConstantUNumberOperand(idx, "short")));
        }
    }

    for (const specifier of node.specifiers) {
        pipeNode(specifier.local, ctx);

        let exportIdx: number;
        if (specifier.exported.type == "Literal") {
            if (typeof specifier.exported.value != "string") {
                throw "Expected a string";
            }
            exportIdx = ctx.stringTable.registerString(specifier.exported.value);
        } else {
            exportIdx = ctx.stringTable.registerString(specifier.exported.name);
        }

        ctx.data.addInstruction(new Instruction(Opcodes.EXPORT).addOperand(new ConstantUNumberOperand(exportIdx, "short")));
    }
}

pipe["ExportDefaultDeclaration"] = (node: acorn.ExportDefaultDeclaration, ctx: PipeContext) => {
    pipeNode(node.declaration, ctx);
    // TODO may use a symbol for default exports
    const idx: number = ctx.stringTable.registerString("default");
    ctx.data.addInstruction(new Instruction(Opcodes.EXPORT).addOperand(new ConstantUNumberOperand(idx, "short")));
}