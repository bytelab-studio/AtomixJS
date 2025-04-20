function F(property) {
    this.myProperty = property;
    this.myMethod = function () {
        print(this.myProperty);
    }
}

const f = new F(10);
print(f.myProperty);
f.myMethod();