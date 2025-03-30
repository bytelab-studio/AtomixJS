import * as acorn from "acorn";
import * as esttraverse from "estraverse";

export function transformProgram(program: acorn.Program): void {
    if (!Array.isArray(program.body)) {
        return;
    }

    for (let i = 0; i < program.body.length; i++) {
        program.body[i] = transformNode(program.body[i]) as any;
    }
}

function packNodes(nodes: acorn.Node[]): acorn.Node {
    return (<acorn.BlockStatement>({
        type: "BlockStatement",
        body: nodes,
        start: 0,
        end: 0,
        // Virtual block to ignore PUSH_SCOPE and POP_SCOPE instructions
        virtual: true
    }));
}

function transformNode(node: acorn.Node): acorn.Node {
    return esttraverse.replace(node as any, {
        // @ts-ignore
        enter(node) {
            console.log("Enter %s", node.type);
            if (node.type == "AssignmentExpression") {
                return transformAssignmentExpression(node as acorn.AssignmentExpression);
            }
            return node;
        },
        leave(node) {
            console.log("Leave %s", node.type);
        }
    }) as acorn.Node;
}

function transformAssignmentExpression(node: acorn.AssignmentExpression): acorn.Node {
    if (node.operator == "=") {
        return node;
    }

    const binaryExpression = (<acorn.BinaryExpression>{
        type: "BinaryExpression",
        left: node.left,
        operator: node.operator.substring(0, node.operator.length - 1) as acorn.BinaryOperator,
        right: node.right,
        start: node.start,
        end: node.end,
        loc: node.loc,
        range: node.range
    });
    return (<acorn.AssignmentExpression>{
        type: "AssignmentExpression",
        left: node.left,
        operator: "=",
        right: binaryExpression,
        start: node.start,
        end: node.end,
        loc: node.loc,
        range: node.range
    });
}