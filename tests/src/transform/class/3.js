class A {
    constructor(x, y) {
        print(x + y);   
    }
}

class B extends A {
    constructor(x) {
        super(x, x);
        print("Outer");
    }
}

const b = new B(20);