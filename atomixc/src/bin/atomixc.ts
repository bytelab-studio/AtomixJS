import {OptionSet, SubCommandSet} from "@koschel-christoph/node.options";
import engine from "./engine";
import compiler from "./compiler";

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

const set = new SubCommandSet(
    "Usage: atomixc <subcommand> [<options>]",
    base,
    engine,
    compiler
)
set.parse(process.argv);