import {StringTable} from "./string-table";
import {Size} from "../size";
import {DataSection} from "./data";

const MAGIC: [number, number, number] = [46, 65, 120];
const VERSION: number = 1;

export interface FileFormat {
    header: FileHeader;
    stringTable: StringTable;
    dataSection: DataSection;
}

export interface FileHeader {
    magic: [number, number, number];
    version: number;
    stringTableStart: number;
    dataSectionStart: number;
}

export function buildFile(stringTable: StringTable, dataSection: DataSection): FileFormat {
    const baseStart: Size = Size.new(3, "byte").add(1, "short").add(2, "int");

    const header: FileHeader = {
        magic: MAGIC,
        version: VERSION,
        stringTableStart: baseStart.inBytes(),
        dataSectionStart: baseStart.add(stringTable.length, "bytes").inBytes()
    }

    return {
        header: header,
        stringTable: stringTable,
        dataSection: dataSection
    }
}