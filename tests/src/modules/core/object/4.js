const obj1 = Object.create();
print(obj1);

obj1.a = 1;
print(obj1.a);

const obj2 = Object.create(obj1);
print(obj2);
print(obj2.a);
