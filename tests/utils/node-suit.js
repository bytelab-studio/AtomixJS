const os = require("os");
// This file is to create mokup functions of the AtomixJS runtime functions.

globalThis.print = function (...args) {
    for (const arg of args) {
        switch (typeof arg) {
            case "number":
                let _ = arg.toString();
                _ = _.indexOf(".") == -1
                    ? _
                    : _.padEnd(_.indexOf(".") + 7, "0");
                process.stdout.write(_);
                process.stdout.write("\n");
                break;
            case "string":
                process.stdout.write(arg.padEnd(arg.indexOf(".") + 7, "0"));
                process.stdout.write("\n");
                break;
            case "object":
                if (arg == null) {
                    process.stdout.write("null\n");
                    break;
                }
                process.stdout.write("[Object]\n");
                break;
            case "function":
                process.stdout.write("[Function]\n");
                break;
            case "undefined":
                process.stdout.write("undefined\n");
                break;
            case "boolean":
                if (arg) {
                    process.stdout.write("true\n");
                } else {
                    process.stdout.write("false\n");
                }
                break;
        }
    }
}
if (process.argv.length < 2) {
    throw "Missing argument";
}

require(process.argv[2]);