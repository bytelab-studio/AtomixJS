import * as fs from "fs";
import * as path from "path";
import * as crypto from "crypto";
import * as babel from "@babel/parser";
import type * as nodes from "@babel/types";
import { STableSection } from "./format/stable";
import { DataSection } from "./format/data";
import { beginPipe } from "./pipe";
import { buildModule, ModuleFormat } from "./format/module";
import { BinaryWriter } from "./binary";
import { Dumper } from "./dumper";
import * as transform from "./transform";

function hashString(s: string): [number, number] {
    const hash: Buffer = crypto.createHash("sha256").update(s).digest();

    return [
        hash.readUint32LE(0) >>> 0,
        hash.readUInt32LE(4) >>> 0
    ];
}

export function hashFilePath(file: string, root: string, prefix: string): [number, number] {
    if (!path.isAbsolute(file)) {
        file = path.posix.join(process.cwd(), file).replace(/\\/g, "/");
    }
    if (!path.isAbsolute(root)) {
        root = path.posix.join(process.cwd(), root).replace(/\\/g, "/");
    }

    const signature: string = prefix + file.replace(root, "");
    return hashString(signature);
}

export function parseFile(input: string, output: string, root: string, prefix: string): void {
    const content = fs.readFileSync(input, "utf-8");
    const result: babel.ParseResult<nodes.File> = babel.parse(content, {
        sourceFilename: input,
        sourceType: "module",
        errorRecovery: true
    });

    if (result.errors && result.errors.length > 0) {
        for (const error of result.errors) {
            console.log(error.reasonCode);
            console.log(error.code);
        }
        process.exit(1);
    }

    const hash: [number, number] = hashFilePath(input, root, prefix);
    transform.transformFile(result);

    const stableSection: STableSection = new STableSection();
    const dataSection: DataSection = new DataSection();
    beginPipe(result.program, {
        stable: stableSection,
        data: dataSection
    });

    const module: ModuleFormat = buildModule(hash, stableSection, dataSection);
    const dumper: Dumper = new Dumper();
    dumper.dumpModule(module);
    const writer: BinaryWriter = new BinaryWriter(output);
    module.writeTo(writer);
    writer.close();
}