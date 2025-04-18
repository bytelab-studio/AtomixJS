const arr1 = [1, 2, 3, 4, 5];
// TODO replace with destruct operation
const arr2 = Array(arr1[0], arr1[1], arr1[2], arr1[3], arr1[4]);

print(Array.isArray(arr1));
print(Array.isArray(arr2));
print(arr1.length);
print(arr2.length);
print(arr1.length === arr2.length);
