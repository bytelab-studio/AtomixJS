class A {
    constructor() {
        print("Inner");   
    }
}

class B extends A {
    constructor() {
        super();
        print("Outer");
    }
}

const b = new B();