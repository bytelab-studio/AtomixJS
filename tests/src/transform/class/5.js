
class Bar {
    constructor(bar) {
        this.bar = bar;
    }

    static foo = 20;
}

class Foo extends Bar {
    constructor(bar) {
        super(bar);
    }

    myMethod() {
        return this.bar;
    }

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
