import * as fs from "fs";
import * as path from "path";
import * as crypto from "crypto";
import * as babel from "@babel/parser";
import type * as nodes from "@babel/types";
import {StringTableBuilder} from "./format/string-table";
import {DataSectionBuilder} from "./format/data";
import {beginPipe} from "./pipe";
import {buildFile, FileFormat} from "./format/file";
import {FormatWriter} from "./writer";
import * as dumper from "./dumper";
import * as transform from "./transform";

function hashString(s: string): number {
    const hash: Buffer = crypto.createHash("sha256").update(s).digest();

    let result: number = 0;
    for (let i: number = 0; i < 32; i += 4) {
        const chunk: number = hash.readUint32LE(i);
        result ^= chunk;
    }

    return result >>> 0;
}

export function hashFilePath(file: string, root: string, prefix: string): number {
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

    const hash: number = hashFilePath(input, root, prefix);
    transform.transformFile(result);

    const stringTable: StringTableBuilder = new StringTableBuilder();
    const dataSection: DataSectionBuilder = new DataSectionBuilder();
    beginPipe(result.program, {
        stringTable: stringTable,
        data: dataSection
    });

    const file: FileFormat = buildFile(hash, stringTable.build(), dataSection.build());
    dumper.dumpFormat(file);
    const writer = new FormatWriter(output);
    writer.writeFile(file);
    writer.close();
}