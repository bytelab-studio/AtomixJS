import {OptionSet, SubCommandSet} from "@koschel-christoph/node.options";
import compile from "./compile";

function* base(handler: SubCommandSet, commandNotFound: boolean): Generator<OptionSet> {
    let help: boolean = false;
    const set: OptionSet = new OptionSet(
        ["h|help", "Prints this help text", () => help = true]
    );

    yield set;

    if (help || commandNotFound) {
        handler.printHelpString(process.stdout);
    }
}

function* compiler(handler: SubCommandSet): Generator<OptionSet | SubCommandSet> {
    const set = new SubCommandSet(
        "Usage: atomixc compiler <subcommand> [<options>]",
        base,
        compile
    );

    yield set;
}

const command: [string, string, (handler: SubCommandSet) => Generator<OptionSet | SubCommandSet>] = ["compiler", "Compiler commands", compiler];

export default command;