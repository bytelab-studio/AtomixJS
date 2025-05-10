let a = 0;
let b = 1;

for (let i = 0; i < 20; i = i + 1) {
    let c = a + b;
    print(c);
    a = b;
    b = c;
}