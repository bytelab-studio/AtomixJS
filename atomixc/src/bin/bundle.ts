import * as fs from "fs";
import {OptionSet, SubCommandSet} from "@koschel-christoph/node.options";
import {ModuleFormat} from "../format/module";
import {BinaryReader, BinaryWriter} from "../binary";
import {buildBundle, BundleFormat} from "../format/bundle";

function* bundle(handler: SubCommandSet): Generator<OptionSet> {
    let help: boolean = false;
    const files: string[] = [];
    let entryPoint: string | null = null;
    let output: string | null = null;

    const set: OptionSet = new OptionSet(
        "Usage: atomixc bundle <file> -o <output> [<option>]",
        ["<>", "Modules used for bundling", v => files.push(v)],
        ["e=|entry=", "Module that acts as the {entry} point", v => entryPoint = v],
        ["o=|output=", "The output {file} of the bundle", v => output = v],
        ["h|help", "Prints this help text", () => help = true]
    );

    yield set;

    if (help) {
        set.printHelpString(process.stdout);
        process.exit(0);
    }
    if (files.length == 0 && entryPoint == null) {
        console.log("Need at least one file to create bundle");
        console.log("Add one by passing it or set it as an entry point");
        console.log();

        set.printHelpString(process.stdout);
        process.exit(1);
    }
    if (output == null) {
        console.log("No output specified");
        console.log();

        set.printHelpString(process.stdout);
        process.exit(1);
    }

    if (entryPoint != null && (!fs.existsSync(entryPoint) || !fs.statSync(entryPoint).isFile())) {
        console.log(`File '${entryPoint}' does not exist or is not a file`);
        process.exit(1);
    }
    for (const file of files) {
        if (!fs.existsSync(file) || !fs.statSync(file).isFile()) {
            console.log(`File '${file}' does not exist or is not a file`);
            process.exit(1);
        }
    }

    const entryPointModule: ModuleFormat | null = !entryPoint
        ? null
        : ModuleFormat.readFrom(new BinaryReader(entryPoint));

    const modules: ModuleFormat[] = [];
    for (const file of files) {
        modules.push(ModuleFormat.readFrom(new BinaryReader(file)));
    }
    if (entryPointModule) {
        modules.push(entryPointModule);
    }
    const seenHashes: [number, number][] = [];
    for (const module of modules) {
        for (const hashes of seenHashes) {
            if (hashes[0] == module.header.hash[0] &&
                hashes[1] == module.header.hash[1]) {
                console.log("Modules with the same hash cannot be added to the same bundle");
                process.exit(1);
            }
        }
        seenHashes.push(module.header.hash);
    }

    const bundle: BundleFormat = buildBundle(
        entryPointModule == null ? [0, 0] : entryPointModule.header.hash,
        modules
    );

    const writer: BinaryWriter = new BinaryWriter(output);
    bundle.writeTo(writer);
}

const command: [string, string, (handler: SubCommandSet) => Generator<OptionSet | SubCommandSet>] = ["bundle", "Bundle multiple modules into a bundle", bundle];
export default command;