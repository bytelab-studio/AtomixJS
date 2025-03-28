import * as acorn from "acorn";
import * as fs from "fs";
import {StringTableBuilder} from "./format/string-table";
import {DataSectionBuilder} from "./format/data";
import {beginPipe} from "./pipe";
import {buildFile, FileFormat} from "./format/file";
import {FormatWriter} from "./writer";
import * as dumper from "./dumper";
import * as transform from "./transform";

export function parseFile(input: string, output: string): void {
    const content = fs.readFileSync(input, "utf-8");
    const program: acorn.Program = acorn.parse(content, {
        ecmaVersion: 6,
        sourceType: "module"
    });

    transform.transformProgram(program);

    const stringTable: StringTableBuilder = new StringTableBuilder();
    const dataSection: DataSectionBuilder = new DataSectionBuilder();
    beginPipe(program, {
        stringTable: stringTable,
        data: dataSection
    });

    const file: FileFormat = buildFile(stringTable.build(), dataSection.build());
    dumper.dumpFormat(file);
    const writer = new FormatWriter(output);
    writer.writeFile(file);
    writer.close();
}