class Foo {
    constructor(bar) {
        this.bar = bar;
    }

    myMethod() {
        return this.bar;
    }

    static foo = 20;

    static myStaticMethod() {
        return Foo.foo;
    }

    static {
        print(Foo.foo);
    }
}

const foo = new Foo(10);
print(foo.myMethod());
print(foo.bar);
print(Foo.myStaticMethod());
print(Foo.foo);