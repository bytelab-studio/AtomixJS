"use strict"
function foo() {
    print(this);
}

foo();
foo.call();
foo.call({});