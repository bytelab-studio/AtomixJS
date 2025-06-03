import {FileFormat, FileHeader} from "./format/file";
import {StringTable} from "./format/string-table";
import {DataSection} from "./format/data";
import {Opcodes} from "./opcodes";

function writeIntend(str: string | number): void {
    console.log("    " + str);
}

export function dumpFormat(file: FileFormat): void {
    dumpHeader(file.header);
    dumpStringTable(file.stringTable);
    dumpDataSection(file.dataSection);
}

function dumpHeader(header: FileHeader): void {
    console.log("$HEADER:");
    writeIntend(`MAGIC: ${header.magic[0]} ${header.magic[1]} ${header.magic[2]}`);
    writeIntend(`VERSION: ${header.version}`);
    writeIntend(`HASH: 0x${header.hash[0].toString(16)}${header.hash[1].toString(16)}`);
    writeIntend(`$STABLE: 0x${header.stringTableStart.toString(16)}`);
    writeIntend(`$DATA: 0x${header.dataSectionStart.toString(16)}`);
    console.log();
}

function dumpStringTable(table: StringTable): void {
    console.log("$STABLE:");
    for (let i: number = 0; i < table.count; i++) {
        writeIntend(`${i}: "${table.strings[i]}" at 0x${table.offsets[i].toString(16)}`);
    }
    console.log();
}

function dumpDataSection(data: DataSection): void {
    console.log("$DATA:");
    for (let i: number = 0; i < data.count; i++) {
        writeIntend(`${i}: ${Opcodes[data.instructions[i].opcode]}`);
    }
}