import * as fs from "fs";
import {OptionSet, SubCommandSet} from "@koschel-christoph/node.options";
import {BinaryReader} from "../binary";
import {Dumper} from "../dumper";
import {ModuleFormat} from "../format/module";
import {BundleFormat} from "../format/bundle";

function* dump(handler: SubCommandSet): Generator<OptionSet> {
    const files: string[] = [];
    let help: boolean = false;

    const set: OptionSet = new OptionSet(
        "Usage: atomixc dump <file>",
        ["<>", "The files to dump", v => files.push(v)],
        ["h|help", "Prints this help text", () => help = true]
    );

    yield set;

    if (help) {
        set.printHelpString(process.stdout);
        process.exit(0);
    }

    for (const file of files) {
        if (!fs.existsSync(file) || !fs.statSync(file).isFile()) {
            console.log(`File '${file}' does not exist or is not a file`);
            process.exit(1);
        }
    }

    const dumper: Dumper = new Dumper();

    for (const file of files) {
        const reader: BinaryReader = new BinaryReader(file);
        if (ModuleFormat.isModule(reader)) {
            console.log("    MODULE: " + file);
            dumper.dumpModule(ModuleFormat.readFrom(reader));
            console.log()
        }

        console.log("    BUNDLE: " + file);
        dumper.dumpBundle(BundleFormat.readFrom(reader));
    }
}

const command: [string, string, (handler: SubCommandSet) => Generator<OptionSet | SubCommandSet>] = ["dump", "Dump information about a bundle or module", dump];
export default command;