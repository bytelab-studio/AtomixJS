import * as fs from "fs";
import * as babel from "@babel/parser";
import type * as nodes from "@babel/types";
import {StringTableBuilder} from "./format/string-table";
import {DataSectionBuilder} from "./format/data";
import {beginPipe} from "./pipe";
import {buildFile, FileFormat} from "./format/file";
import {FormatWriter} from "./writer";
import * as dumper from "./dumper";
import * as transform from "./transform";

export function parseFile(input: string, output: string): void {
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

    transform.transformFile(result);

    const stringTable: StringTableBuilder = new StringTableBuilder();
    const dataSection: DataSectionBuilder = new DataSectionBuilder();
    beginPipe(result.program, {
        stringTable: stringTable,
        data: dataSection
    });

    const file: FileFormat = buildFile(stringTable.build(), dataSection.build());
    dumper.dumpFormat(file);
    const writer = new FormatWriter(output);
    writer.writeFile(file);
    writer.close();
}