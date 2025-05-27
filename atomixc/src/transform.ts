import traverse, {type NodePath} from "@babel/traverse";
import * as nodes from "@babel/types";

export function transformFile(file: nodes.File): void {
    transformNode(file);
}


function packNodes(_nodes: nodes.Statement[]): nodes.BlockStatement {
    const sth: nodes.BlockStatement = nodes.blockStatement(_nodes);
    // Virtual block to ignore PUSH_SCOPE and POP_SCOPE instructions
    (sth as any).virtual = true;
    return sth;
}

function transformNode(file: nodes.File): void {
    traverse(file, {
        enter(ctx: NodePath<nodes.Node>): void {
            console.log("Enter %s", ctx.type);
        },
        AssignmentExpression(ctx: NodePath<nodes.AssignmentExpression>): void {
            if (ctx.node.operator == "=") {
                return;
            }
            const binaryExpression: nodes.BinaryExpression = nodes.binaryExpression(
                ctx.node.operator.substring(0, ctx.node.operator.length - 1) as nodes.BinaryExpression["operator"],
                ctx.node.left as nodes.Expression,
                ctx.node.right
            );
            ctx.replaceWith(nodes.assignmentExpression(
                "=",
                ctx.node.left,
                binaryExpression
            ));
        },
        ForStatement(ctx: NodePath<nodes.ForStatement>): void {
            if (ctx.node.init) {
                ctx.insertBefore(ctx.node.init);
            }
            const test: nodes.Expression = ctx.node.test
                ? ctx.node.test
                : nodes.booleanLiteral(true);

            const body: nodes.Node = ctx.node.update
                ? packNodes([
                    ctx.node.body,
                    nodes.expressionStatement(ctx.node.update)
                ]) : ctx.node.body;
            ctx.replaceWith(nodes.whileStatement(test, body));
        }
    });
}