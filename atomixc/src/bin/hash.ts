import {OptionSet, SubCommandSet} from "@koschel-christoph/node.options";
import {hashString} from "../hash";

function* hash(handler: SubCommandSet): Generator<OptionSet> {
    let help: boolean = false;
    const strings: string[] = [];

    const set: OptionSet = new OptionSet(
        "Usage: atomixc hash <string>",
        ["<>", "Strings that should be hashed", v => strings.push(v)],
        ["h|help", "Prints this help text", () => help = true]
    );

    yield set;

    if (help) {
        set.printHelpString(process.stdout);
        process.exit(0);
    }

    if (strings.length == 0) {
        console.log("Need at least one string to hash");
        console.log();

        set.printHelpString(process.stdout);
        process.exit(1);
    }

    for (const s of strings) {
        const [high, low] = hashString(s);
        console.log(`0x${high.toString(16)}${low.toString(16)}`);
    }
}

const command: [string, string, (handler: SubCommandSet) => Generator<OptionSet | SubCommandSet>] = ["hash", "Hash strings with FNV-1a", hash];
export default command;