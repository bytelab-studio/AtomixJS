const os = require("os");
// This file is to create mokup functions of the AtomixJS runtime functions.

const lineBreak = os.platform() == "win32"
    ? "\r\n"
    : "\n";

globalThis.print = function (...args) {
    for (const arg of args) {
        switch (typeof arg) {
            case "number":
                let _ = arg.toString();
                _ = _.indexOf(".") == -1
                    ? _
                    : _.padEnd(_.indexOf(".") + 7, "0");
                process.stdout.write(_);
                process.stdout.write(lineBreak);
                break;
            case "string":
                process.stdout.write(arg.padEnd(arg.indexOf(".") + 7, "0"));
                process.stdout.write(lineBreak);
                break;
            case "object":
                if (arg == null) {
                    process.stdout.write("null");
                    process.stdout.write(lineBreak);
                    break;
                }
                process.stdout.write("[Object]");
                process.stdout.write(lineBreak);
                break;
            case "function":
                process.stdout.write("[Function]");
                process.stdout.write(lineBreak);
                break;
            case "undefined":
                process.stdout.write("undefined");
                process.stdout.write(lineBreak);
                break;
            case "boolean":
                if (arg) {
                    process.stdout.write("true");
                    process.stdout.write(lineBreak);
                } else {
                    process.stdout.write("false");
                    process.stdout.write(lineBreak);
                }
                break;
        }
    }
}
if (process.argv.length < 2) {
    throw "Missing argument";
}

require(process.argv[2]);
const os = require("node:os");