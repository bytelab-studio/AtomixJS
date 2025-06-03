import {OptionSet, SubCommandSet} from "@koschel-christoph/node.options";

function* bundle(handler: SubCommandSet): Generator<OptionSet> {
    let help: boolean = false;

    const set: OptionSet = new OptionSet(
        "Usage: atomixc bundle",
        ["h|help", "Prints this help text", () => help = true]
    );

    yield set;

    if (help) {
        set.printHelpString(process.stdout);
        process.exit(0);
    }
}

const command: [string, string, (handler: SubCommandSet) => Generator<OptionSet | SubCommandSet>] = ["bundle", "Bundle multiple modules into a bundle", bundle];
export default command;