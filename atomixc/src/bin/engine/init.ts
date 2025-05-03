import {OptionSet, SubCommandSet} from "@koschel-christoph/node.options";
import * as structure from "../../engine/structure"

function* init(handler: SubCommandSet): Generator<OptionSet> {
    let help: boolean = false;

    const set: OptionSet = new OptionSet(
        "Usage: atomixc engine init [<options>]",
        ["h|help", "Prints this help text", () => help = true]
    );

    yield set;

    if (help) {
        set.printHelpString(process.stdout);
        process.exit(0);
    }

    structure.initStructure(process.cwd());
    structure.initEngineBuild(process.cwd(), structure.EnginePlatform.WINDOWS, structure.EngineArchitecture.X64);
}

const command: [string, string, (handler: SubCommandSet) => Generator<OptionSet | SubCommandSet>] = ["init", "Init a new engine in the CWD", init];
export default command;