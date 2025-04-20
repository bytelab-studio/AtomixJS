function Person(name) {
    this.name = name;
}

const p = new Person("John");
/**
 * @type Person
 */
const p2 = Object.instantiate(Person, "Jane");
print(p);
print(p.name);
print(p2);
print(p2.name);