import * as path from "path";
import traverse, {type NodePath} from "@babel/traverse";
import * as nodes from "@babel/types";
import {hashFilePath, hashString} from "./hash";

export function transformFile(file: nodes.File, input: string, root: string, prefix: string): void {
    transformPreNode(file, input, root, prefix);
    transformPostNode(file, input, root, prefix);
}


function packNodes(_nodes: nodes.Statement[]): nodes.BlockStatement {
    const sth: nodes.BlockStatement = nodes.blockStatement(_nodes);
    // Virtual block to ignore PUSH_SCOPE and POP_SCOPE instructions
    if (!sth.extra) {
        sth.extra = {};
    }
    sth.extra.virtual = true;
    return sth;
}

function transformPreNode(file: nodes.File, input: string, root: string, prefix: string): void {
    traverse(file, {
        enter(ctx: NodePath<nodes.Node>): void {
            console.log("C1:Enter %s", ctx.type);
        },
        ExpressionStatement(ctx: NodePath<nodes.ExpressionStatement>): void {
            if (ctx.node.expression.type == "StringLiteral" && ctx.node.expression.value == "use strict") {
                ctx.remove();
            }
        },
        CallExpression(ctx: NodePath<nodes.CallExpression>): void {
            if (ctx.node.callee.type != "Super") {
                return;
            }
            /*
             * Before:
             * super(arg0, arg1, argN)
             *
             * After:
             * SuperClass.call(this, arg0, arg1, argN)
             */
            const classContext: NodePath<nodes.ClassDeclaration | nodes.ClassExpression> | null = ctx.findParent(node => node.type == "ClassDeclaration" || node.type == "ClassExpression") as NodePath<nodes.ClassDeclaration | nodes.ClassExpression> | null;
            if (!classContext) {
                throw "Missing class context";
            }
            if (!classContext.node.superClass) {
                throw "Class has no superClass";
            }
            if (classContext.node.superClass.type != "Identifier") {
                throw "SuperClass must be an identifier";
            }
            ctx.replaceWith(nodes.callExpression(
                nodes.memberExpression(
                    classContext.node.superClass,
                    nodes.identifier("call")
                ),
                [
                    nodes.thisExpression(),
                    ...ctx.node.arguments
                ]
            ));
        },
        Super(ctx: NodePath<nodes.Super>): void {
            const isStatic: boolean = !!ctx.findParent(node => node.node.type == "ClassMethod" && node.node.static || node.node.type == "StaticBlock");
            const classContext: NodePath<nodes.ClassDeclaration | nodes.ClassExpression> | null = ctx.findParent(node => node.type == "ClassDeclaration" || node.type == "ClassExpression") as NodePath<nodes.ClassDeclaration | nodes.ClassExpression> | null;
            if (!classContext) {
                throw "Missing class context";
            }
            if (!classContext.node.superClass) {
                throw "Class has no superClass";
            }
            if (classContext.node.superClass.type != "Identifier") {
                throw "SuperClass must be an identifier";
            }
            if (!ctx.node.extra) {
                ctx.node.extra = {};
            }
            ctx.node.extra.isStatic = isStatic;
            ctx.node.extra.targetName = classContext.node.superClass.name;
        }
    });
}

function transformPostNode(file: nodes.File, input: string, root: string, prefix: string): void {
    traverse(file, {
        enter(ctx: NodePath<nodes.Node>): void {
            console.log("C2:Enter %s", ctx.type);
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
        },
        NewExpression(ctx: NodePath<nodes.NewExpression>): void {
            /*
             * Before:
             * new Person(1, 2, 3)
             *
             * After:
             * Object.instantiate(Person, 1, 2, 3)
             */
            ctx.replaceWith(nodes.callExpression(
                nodes.memberExpression(
                    nodes.identifier("Object"),
                    nodes.identifier("instantiate")
                ),
                [
                    ctx.node.callee as nodes.Expression,
                    ...ctx.node.arguments
                ]
            ));
        },
        ClassExpression(ctx: NodePath<nodes.ClassExpression>): void {
            /*
             * Before:
             * const Foo = class {
             *      constructor(bar) {
             *          this.bar = bar;
             *      }
             *      myMethod() {
             *          return this.bar;
             *      }
             *
             *      static foo = 20;
             *      static myStaticMethod() {
             *          return Foo.foo;
             *      }
             * }
             *
             * After:
             * const Foo = (function() {
             *      function __C__(bar) {
             *          this.bar = bar;
             *      }
             *      __C__.prototype.myMethod() {
             *          return this.bar;
             *      }
             *      __C__.myStaticMethod = function() {
             *          return Foo.foo;
             *      }
             *      __C__.foo = 20;
             *      return __C__;
             * })();
             */
            ctx.replaceWith(transformClass(ctx.node, nodes.identifier("__C__")));
        },
        ClassDeclaration(ctx: NodePath<nodes.ClassDeclaration>): void {
            /*
             * Before:
             * class Foo {
             *      constructor(bar) {
             *          this.bar = bar;
             *      }
             *      myMethod() {
             *          return this.bar;
             *      }
             *
             *      static foo = 20;
             *      static myStaticMethod() {
             *          return Foo.foo;
             *      }
             * }
             *
             * After:
             * const Foo = (function() {
             *      function Foo(bar) {
             *          this.bar = bar;
             *      }
             *      Foo.prototype.myMethod() {
             *          return this.bar;
             *      }
             *      Foo.myStaticMethod = function() {
             *          return Foo.foo;
             *      }
             *      Foo.foo = 20;
             *      return Foo;
             * })();
             */
            if (!ctx.node.id) {
                throw "Missing class identifier";
            }

            const classDeclaration: nodes.CallExpression = transformClass(ctx.node, ctx.node.id);
            ctx.replaceWith(nodes.variableDeclaration("const", [
                nodes.variableDeclarator(ctx.node.id, classDeclaration)
            ]));
        },
        ImportDeclaration(ctx: NodePath<nodes.ImportDeclaration>): void {
            let source: string = ctx.node.source.value;
            let sourceHash: [number, number];
            if (source.startsWith(".")) {
                source = path.join(path.basename(input), source);
                sourceHash = hashFilePath(source, root, prefix);
            } else {
                sourceHash = hashString(source);
            }
            const defaultSpec: nodes.ImportDefaultSpecifier | undefined = ctx.node.specifiers.find(s => s.type == "ImportDefaultSpecifier");
            const namespaceSpec: nodes.ImportNamespaceSpecifier | undefined = ctx.node.specifiers.find(s => s.type == "ImportNamespaceSpecifier");
            const namedSpecs: nodes.ImportSpecifier[] = ctx.node.specifiers.filter(s => s.type == "ImportSpecifier");

            const statements: nodes.Statement[] = [];
            let initializer: nodes.Expression;

            if (namespaceSpec) {
                initializer = namespaceSpec.local;
                statements.push(
                    nodes.variableDeclaration(
                        "const",
                        [
                            nodes.variableDeclarator(
                                initializer,
                                nodes.callExpression(
                                    nodes.memberExpression(
                                        nodes.identifier("Module"),
                                        nodes.identifier("importModule"),
                                    ),
                                    [
                                        nodes.numericLiteral(sourceHash[0]),
                                        nodes.numericLiteral(sourceHash[1])
                                    ]
                                )
                            )
                        ]
                    )
                )
            } else {
                initializer = nodes.callExpression(
                    nodes.memberExpression(
                        nodes.identifier("Module"),
                        nodes.identifier("importModule"),
                    ),
                    [
                        nodes.numericLiteral(sourceHash[0]),
                        nodes.numericLiteral(sourceHash[1])
                    ]
                );
            }

            const props: nodes.ObjectProperty[] = [];
            if (defaultSpec) {
                props.push(
                    nodes.objectProperty(
                        nodes.identifier("default"),
                        defaultSpec.local
                    )
                )
            }

            for (const namedSpec of namedSpecs) {
                props.push(
                    nodes.objectProperty(
                        namedSpec.imported,
                        namedSpec.local
                    )
                );
            }

            if (props.length > 0) {
                statements.push(
                    nodes.variableDeclaration(
                        "const",
                        [
                            nodes.variableDeclarator(
                                nodes.objectPattern(props),
                                initializer
                            )
                        ]
                    )
                );
            } else if (!namespaceSpec) {
                statements.push(
                    nodes.expressionStatement(initializer)
                );
            }

            ctx.replaceWithMultiple(statements);
        }
    });
}

function transformClass(node: nodes.ClassDeclaration | nodes.ClassExpression, identifier: nodes.Identifier): nodes.CallExpression {
    const constructorMethod: nodes.ClassMethod | undefined = node.body.body.find(node =>
        node.type == "ClassMethod" &&
        node.kind == "constructor") as nodes.ClassMethod | undefined;

    const methods: nodes.ClassMethod[] = node.body.body.filter(node => node.type == "ClassMethod" && node.kind == "method" && !node.static) as nodes.ClassMethod[];
    const staticMethods: nodes.ClassMethod[] = node.body.body.filter(node => node.type == "ClassMethod" && node.kind == "method" && node.static) as nodes.ClassMethod[];
    const properties: nodes.ClassProperty[] = node.body.body.filter(node => node.type == "ClassProperty" && !node.static && node.value) as nodes.ClassProperty[];
    const staticProperties: nodes.ClassProperty[] = node.body.body.filter(node => node.type == "ClassProperty" && node.static && node.value) as nodes.ClassProperty[];
    const staticBlocks: nodes.StaticBlock[] = node.body.body.filter(node => node.type == "StaticBlock") as nodes.StaticBlock[];

    const extendsNodes: nodes.Statement[] = [];
    if (node.superClass) {
        if (node.superClass.type != "Identifier") {
            throw "SuperClass must be an identifier";
        }

        extendsNodes.push(
            nodes.expressionStatement(
                nodes.assignmentExpression(
                    "=",
                    nodes.memberExpression(
                        identifier,
                        nodes.identifier("prototype")
                    ),
                    nodes.callExpression(
                        nodes.memberExpression(
                            nodes.identifier("Object"),
                            nodes.identifier("create")
                        ),
                        [
                            nodes.memberExpression(
                                node.superClass,
                                nodes.identifier("prototype")
                            )
                        ]
                    )
                )
            ),
            nodes.expressionStatement(
                nodes.callExpression(
                    nodes.memberExpression(
                        nodes.identifier("Object"),
                        nodes.identifier("setPrototypeOf")
                    ),
                    [
                        identifier,
                        node.superClass
                    ]
                )
            )
        );
    }

    return nodes.callExpression(
        nodes.functionExpression(
            undefined,
            [],
            nodes.blockStatement([
                nodes.functionDeclaration(
                    identifier,
                    constructorMethod ? constructorMethod.params as Array<nodes.Identifier | nodes.Pattern | nodes.RestElement> : [],
                    nodes.blockStatement([
                        ...properties.map(property => nodes.expressionStatement(
                            nodes.assignmentExpression(
                                "=",
                                nodes.memberExpression(
                                    nodes.thisExpression(),
                                    property.key
                                ),
                                property.value ? property.value : nodes.identifier("undefined")
                            )
                        )),
                        ...constructorMethod ? constructorMethod.body.body : [],
                    ])
                ),
                ...extendsNodes,
                nodes.expressionStatement(
                    nodes.assignmentExpression(
                        "=",
                        nodes.memberExpression(
                            nodes.memberExpression(
                                identifier,
                                nodes.identifier("prototype")
                            ),
                            nodes.identifier("constructor")
                        ),
                        identifier
                    )
                ),
                ...methods.map(method => nodes.expressionStatement(
                    nodes.assignmentExpression(
                        "=",
                        nodes.memberExpression(
                            nodes.memberExpression(
                                identifier,
                                nodes.identifier("prototype")
                            ),
                            method.key,
                            method.key.type != "Identifier"
                        ),
                        nodes.functionExpression(
                            undefined,
                            method.params as Array<nodes.Identifier | nodes.Pattern | nodes.RestElement>,
                            method.body,
                            method.generator,
                            method.async
                        )
                    )
                )),
                ...staticMethods.map(method => nodes.expressionStatement(
                    nodes.assignmentExpression(
                        "=",
                        nodes.memberExpression(
                            identifier,
                            method.key,
                            method.key.type != "Identifier"
                        ),
                        nodes.functionExpression(
                            undefined,
                            method.params as Array<nodes.Identifier | nodes.Pattern | nodes.RestElement>,
                            method.body,
                            method.generator,
                            method.async
                        )
                    )
                )),
                ...staticProperties.map(property => nodes.expressionStatement(
                    nodes.assignmentExpression(
                        "=",
                        nodes.memberExpression(
                            identifier,
                            property.key,
                            property.key.type != "Identifier"
                        ),
                        property.value ? property.value : nodes.identifier("undefined")
                    )
                )),
                ...staticBlocks.map(block => block.body).flat(),
                nodes.returnStatement(identifier)
            ])
        ),
        []
    )
}