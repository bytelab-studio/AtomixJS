class A {
    constructor(x) {
        this.x = x;
    }
}

class B extends A {
    constructor(x) {
        super(x);
    }

    myFoo() {
        print(this.x);
    }
}


const b = new B(20);
b.myFoo();