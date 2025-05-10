import {OptionSet, SubCommandSet} from "@koschel-christoph/node.options";
import * as structure from "../../engine/structure";

function* cdf(handler: SubCommandSet): Generator<OptionSet> {
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
    let output: string | null = null;
    let modules: string[] = [];

    const set: OptionSet = new OptionSet(
        "Usage: atomixc engine cdf -p <platform> -a <arch> -o <file> [<options>]",
        ["p=|platform=", "The target engine {platform}", v => platform = v],
        ["a=|arch=", "The target engine {architecture}", v => architecture = v],
        ["o=|output=", "{File} to write the cdf json", v => output = v],
        ["m=|modules=", "A module {name} to include", v => modules.push(v)],
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

    if (!output) {
        console.log("Missing output argument.");
        console.log();

        set.printHelpString(process.stdout);
        process.exit(1);
    }

    structure.generateCDF(output, PLATFORMS[platform], ARCHITECTURES[architecture], modules);
}

const command: [string, string, (handler: SubCommandSet) => Generator<OptionSet | SubCommandSet>] = ["cdf", "Generates a compilation database format file", cdf];
export default command;