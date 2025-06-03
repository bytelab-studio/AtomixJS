class A {
    foo() {
        print("A.foo");
    }

    static bar() {
        print("A.bar");
    }
}

class B extends A {
    constructor() {
        super();
    }

    foo() {
        print("B.foo");
        super.foo();
    }

    static bar() {
        print("B.bar");
        super.bar();
    }
}

const b = new B();
b.foo();
B.bar();