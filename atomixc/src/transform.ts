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
            if (node.type == "ExpressionStatement" && node.expression.type == "Literal" && typeof node.expression.value == "string" && node.expression.value == "use strict") {
                return packNodes([]);
            }
            if (node.type == "AssignmentExpression") {
                return transformAssignmentExpression(node as acorn.AssignmentExpression);
            }
            if (node.type == "NewExpression") {
                return transformNewExpression(node as acorn.NewExpression);
            }
            if (node.type == "ForStatement") {
                return transformForStatement(node as acorn.ForStatement);
            }
            if (node.type == "ClassDeclaration") {
                return transformClassDeclaration(node as acorn.ClassDeclaration);
            }
            if (node.type == "ClassExpression") {
                return transformClassExpression(node as acorn.ClassExpression);
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

function transformNewExpression(node: acorn.NewExpression): acorn.Node {
    /*
        Before:
        new Person(1, 2, 3)

        After:
        Object.instantiate(Person, 1, 2, 3)
     */
    return (<acorn.CallExpression>{
        type: "CallExpression",
        callee: (<acorn.MemberExpression>{
            type: "MemberExpression",
            computed: false,
            object: (<acorn.Identifier>{
                type: "Identifier",
                name: "Object",
                start: node.start,
                end: node.end,
                loc: node.loc,
                range: node.range
            }),
            property: (<acorn.Identifier>{
                type: "Identifier",
                name: "instantiate",
                start: node.start,
                end: node.end,
                loc: node.loc,
                range: node.range
            }),
            start: node.start,
            end: node.end,
            loc: node.loc,
            range: node.range
        }),
        arguments: [
            node.callee,
            ...node.arguments
        ],
        start: node.start,
        end: node.end,
        loc: node.loc,
        range: node.range
    });
}

function transformForStatement(node: acorn.ForStatement): acorn.Node {
    const nodes: acorn.Node[] = [];
    if (node.init) {
        nodes.push(node.init);
    }

    const test: acorn.Expression = node.test
        ? node.test
        : (<acorn.Literal>{
            type: "Literal",
            value: true,
            raw: "true",
            start: node.start,
            end: node.end,
            loc: node.loc,
            range: node.range
        });

    const body: acorn.Node = node.update
        ? packNodes([
            node.body,
            (<acorn.ExpressionStatement>{
                type: "ExpressionStatement",
                expression: node.update,
                start: node.update.start,
                end: node.update.end,
                loc: node.update.loc,
                range: node.update.range
            })
        ]) : node.body;

    nodes.push((<acorn.WhileStatement>{
        type: "WhileStatement",
        test: test,
        body: body,
        start: node.start,
        end: node.end,
        loc: node.loc,
        range: node.range
    }));


    return packNodes(nodes);
}

function transformClassExpression(node: acorn.ClassExpression): acorn.Node {
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
    return transformClass(node, {
        type: "Identifier",
        name: "__C__",
        start: node.start,
        end: node.end,
        loc: node.loc,
        range: node.range
    });
}

function transformClassDeclaration(node: acorn.ClassDeclaration): acorn.Node {
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
    const classDeclaration = transformClass(node, node.id);

    return (<acorn.VariableDeclaration>{
        type: "VariableDeclaration",
        kind: "const",
        declarations: [
            (<acorn.VariableDeclarator>{
                type: "VariableDeclarator",
                id: node.id,
                init: classDeclaration,
                start: node.start,
                end: node.end,
                loc: node.loc,
                range: node.range
            })
        ],
        start: node.start,
        end: node.end,
        loc: node.loc,
        range: node.range
    });
}

function transformClass(node: acorn.ClassDeclaration | acorn.ClassExpression, identifier: acorn.Identifier): acorn.Expression {
    const constructorMethod: acorn.MethodDefinition | undefined = node.body.body.find(node =>
        node.type == "MethodDefinition" &&
        node.kind == "constructor") as acorn.MethodDefinition | undefined;

    const methods: acorn.MethodDefinition[] = node.body.body.filter(node => node.type == "MethodDefinition" && node.kind == "method" && !node.static) as acorn.MethodDefinition[];
    const staticMethods: acorn.MethodDefinition[] = node.body.body.filter(node => node.type == "MethodDefinition" && node.kind == "method" && node.static) as acorn.MethodDefinition[];
    const properties = node.body.body.filter(node => node.type == "PropertyDefinition" && !node.static && node.value) as acorn.PropertyDefinition[];
    const staticProperties = node.body.body.filter(node => node.type == "PropertyDefinition" && node.static && node.value) as acorn.PropertyDefinition[];
    const staticBlocks: acorn.StaticBlock[] = node.body.body.filter(node => node.type == "StaticBlock");

    return (<acorn.CallExpression>{
        type: "CallExpression",
        callee: (<acorn.FunctionExpression>{
            type: "FunctionExpression",
            id: undefined,
            params: [],
            body: (<acorn.BlockStatement>{
                type: "BlockStatement",
                body: [
                    (<acorn.FunctionDeclaration>{
                        type: "FunctionDeclaration",
                        id: identifier,
                        params: constructorMethod ? constructorMethod.value.params : [],
                        body: (<acorn.BlockStatement>{
                            type: "BlockStatement",
                            body: [
                                ...properties.map(property => (<acorn.ExpressionStatement>{
                                    type: "ExpressionStatement",
                                    expression: (<acorn.AssignmentExpression>{
                                        type: "AssignmentExpression",
                                        left: (<acorn.MemberExpression>{
                                            type: "MemberExpression",
                                            object: (<acorn.ThisExpression>{
                                                type: "ThisExpression",
                                                start: node.start,
                                                end: node.end,
                                                loc: node.loc,
                                                range: node.range
                                            }),
                                            property: property.key,
                                            computed: property.key.type != "Identifier",
                                            optional: false,
                                            start: node.start,
                                            end: node.end,
                                            loc: node.loc,
                                            range: node.range
                                        }),
                                        operator: "=",
                                        right: property.value,
                                        start: node.start,
                                        end: node.end,
                                        loc: node.loc,
                                        range: node.range
                                    }),
                                    start: node.start,
                                    end: node.end,
                                    loc: node.loc,
                                    range: node.range
                                })),
                                ...(constructorMethod ? constructorMethod.value.body.body : []),
                            ],
                            start: node.start,
                            end: node.end,
                            loc: node.loc,
                            range: node.range
                        }),
                        expression: false,
                        async: false,
                        generator: false,
                        start: node.start,
                        end: node.end,
                        loc: node.loc,
                        range: node.range
                    }),
                    ...methods.map(method => (<acorn.ExpressionStatement>{
                        type: "ExpressionStatement",
                        expression: (<acorn.AssignmentExpression>{
                            type: "AssignmentExpression",
                            left: (<acorn.MemberExpression>{
                                type: "MemberExpression",
                                object: (<acorn.MemberExpression>{
                                    type: "MemberExpression",
                                    object: identifier,
                                    property: (<acorn.Identifier>{
                                        type: "Identifier",
                                        name: "prototype",    
                                        start: node.start,
                                        end: node.end,
                                        loc: node.loc,
                                        range: node.range
                                    }),
                                    computed: false,
                                    optional: false,
                                    start: node.start,
                                    end: node.end,
                                    loc: node.loc,
                                    range: node.range
                                }),
                                property: method.key,
                                computed: method.key.type != "Identifier",
                                optional: false,
                                start: node.start,
                                end: node.end,
                                loc: node.loc,
                                range: node.range
                            }),
                            operator: "=",
                            right: method.value,
                            start: node.start,
                            end: node.end,
                            loc: node.loc,
                            range: node.range
                        }),
                        start: node.start,
                        end: node.end,
                        loc: node.loc,
                        range: node.range
                    })),
                    ...staticMethods.map(method => (<acorn.ExpressionStatement>{
                        type: "ExpressionStatement",
                        expression: (<acorn.AssignmentExpression>{
                            type: "AssignmentExpression",
                            left: (<acorn.MemberExpression>{
                                type: "MemberExpression",
                                object: identifier,
                                property: method.key,
                                computed: method.key.type != "Identifier",
                                optional: false,
                                start: node.start,
                                end: node.end,
                                loc: node.loc,
                                range: node.range
                            }),
                            operator: "=",
                            right: method.value,
                            start: node.start,
                            end: node.end,
                            loc: node.loc,
                            range: node.range
                        }),
                        start: node.start,
                        end: node.end,
                        loc: node.loc,
                        range: node.range
                    })),
                    ...staticProperties.map(property => (<acorn.ExpressionStatement>{
                        type: "ExpressionStatement",
                        expression: (<acorn.AssignmentExpression>{
                            type: "AssignmentExpression",
                            left: (<acorn.MemberExpression>{
                                type: "MemberExpression",
                                object: identifier,
                                property: property.key,
                                computed: property.key.type != "Identifier",
                                optional: false,
                                start: node.start,
                                end: node.end,
                                loc: node.loc,
                                range: node.range
                            }),
                            operator: "=",
                            right: property.value,
                            start: node.start,
                            end: node.end,
                            loc: node.loc,
                            range: node.range
                        }),
                        start: node.start,
                        end: node.end,
                        loc: node.loc,
                        range: node.range
                    })),
                    ...staticBlocks.map(node => node.body).flat(),
                    (<acorn.ReturnStatement>{
                        type: "ReturnStatement",
                        argument: identifier,
                        start: node.start,
                        end: node.end,
                        loc: node.loc,
                        range: node.range
                    })
                ],
                start: node.start,
                end: node.end,
                loc: node.loc,
                range: node.range
            }),
            expression: false,
            async: false,
            generator: false,
            start: node.start,
            end: node.end,
            loc: node.loc,
            range: node.range
        }),
        arguments: [],
        optional: false,
        start: node.start,
        end: node.end,
        loc: node.loc,
        range: node.range
    });
}