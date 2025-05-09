import {OptionSet, SubCommandSet} from "@koschel-christoph/node.options";
import init from "./init";
import cdf from "./cdf";

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

function* engine(handler: SubCommandSet): Generator<OptionSet | SubCommandSet> {
    const set = new SubCommandSet(
        "Usage: atomixc engine <subcommand> [<options>]",
        base,
        init,
        cdf
    );

    yield set;
}

const command: [string, string, (handler: SubCommandSet) => Generator<OptionSet | SubCommandSet>] = ["engine", "Engine commands", engine];

export default command;