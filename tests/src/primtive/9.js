const sym1 = Symbol("Foo");
const sym2 = Symbol("Foo");

const obj = {}
obj[sym1] = "Bar";
obj[sym2] = "Bar 2";

print(obj[sym1]);
print(obj[sym2]);