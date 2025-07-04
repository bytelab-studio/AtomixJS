const os = require("os");
// This file is to create mokup functions of the AtomixJS runtime functions.

globalThis.print = function (...args) {
    if (args.length == 0) {
        process.stdout.write("\n");
        return;
    }
    for (const arg of args) {
        process.stdout.write(String(arg));
        process.stdout.write("\n");
    }
}

Object.instantiate = function (constructor, ...args) {
    return new constructor(...args);
}

const _create = Object.create;
Object.create = function (proto) {
    if (!proto) {
        return {}
    }
    return _create(proto);
}

if (process.argv.length < 2) {
    throw "Missing argument";
}

require(process.argv[2]);