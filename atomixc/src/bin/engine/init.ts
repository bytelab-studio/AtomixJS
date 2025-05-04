import {OptionSet, SubCommandSet} from "@koschel-christoph/node.options";
import * as structure from "../../engine/structure";

function* init(handler: SubCommandSet): Generator<OptionSet> {
    const PLATFORMS: Record<string, structure.EnginePlatform> = {
        "windows": structure.EnginePlatform.WINDOWS,
        "linux": structure.EnginePlatform.LINUX
    }
    const ARCHITECTURES: Record<string, structure.EngineArchitecture> = {
        "x86_64": structure.EngineArchitecture.X64,
        "arm64": structure.EngineArchitecture.ARM64,
    }

    let help: boolean = false;
    let platform: string | null = null;
    let architecture: string | null = null;

    const set: OptionSet = new OptionSet(
        "Usage: atomixc engine init -p <platform> -a <arch> [<options>]",
        ["p=|platform=", "The target engine {platform}", v => platform = v],
        ["a=|arch=", "The target engine {architecture}", v => architecture = v],
        ["h|help", "Prints this help text", () => help = true]
    );

    yield set;

    if (help) {
        set.printHelpString(process.stdout);
        process.exit(0);
    }

    if (!platform || !(platform in PLATFORMS)) {
        console.log("Missing platform argument");
        console.log(`Available platforms: ${Object.keys(PLATFORMS).join(", ")}`);
        console.log();

        set.printHelpString(process.stdout);
        process.exit(1);
    }

    if (!architecture || !(architecture in ARCHITECTURES)) {
        console.log("Missing architecture argument.");
        console.log(`Available architectures: ${Object.keys(ARCHITECTURES).join(", ")}`);
        console.log();

        set.printHelpString(process.stdout);
        process.exit(1);
    }

    structure.initStructure(process.cwd());
    structure.initEngineBuild(process.cwd(), PLATFORMS[platform], ARCHITECTURES[architecture]);
}

const command: [string, string, (handler: SubCommandSet) => Generator<OptionSet | SubCommandSet>] = ["init", "Init a new engine in the CWD", init];
export default command;