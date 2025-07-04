import {OptionSet, SubCommandSet} from "@koschel-christoph/node.options";
import * as parser from "../../parser";

function* compile(handler: SubCommandSet): Generator<OptionSet> {
    let input: null | string = null;
    let output: null | string = null;
    let root: null | string = null;
    let help: boolean = false;

    const set: OptionSet = new OptionSet(
        "Usage: atomixc compiler compile <input> -o <output> -r <root>",
        ["o=|output=", "{File} to write the output", v => output = v],
        ["r=|root=", "{Directory} to use as root", v => root = v],
        ["<>", "A input file", v => input = v],
        ["h|help", "Prints this help text", () => help = true]
    );

    yield set;

    if (help) {
        set.printHelpString(process.stdout);
        process.exit(0);
    }

    if (!input || !output || !root) {
        console.log("Missing arguments\n");
        set.printHelpString(process.stderr);
        process.exit(1);
    }

    parser.parseFile(input, output, root, "@program");
}

const command: [string, string, (handler: SubCommandSet) => Generator<OptionSet | SubCommandSet>] = ["compile", "Compile a JavaScript file", compile];
export default command;