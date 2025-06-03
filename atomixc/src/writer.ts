import * as fs from "fs";
import {FileFormat, FileHeader} from "./format/file";
import {StringTable} from "./format/string-table";
import {DataSection} from "./format/data";

export class BinaryWriter {
    private stream: fs.WriteStream;

    protected constructor(file: string) {
        this.stream = fs.createWriteStream(file);
    }

    protected writeU8(x: number): void {
        const buf = Buffer.alloc(1);
        buf.writeUint8(x);
        this.write(buf);
    }

    protected writeI8(x: number): void {
        this.writeU8(x);
    }

    protected writeU16(x: number): void {
        const buf = Buffer.alloc(2);
        buf.writeUint16LE(x);
        this.write(buf);
    }

    protected writeI16(x: number): void {
        const buf = Buffer.alloc(2);
        buf.writeInt16LE(x);
        this.write(buf);
    }

    protected writeU32(x: number): void {
        const buf = Buffer.alloc(4);
        buf.writeUint32LE(x);
        this.write(buf);
    }

    protected writeI32(x: number): void {
        const buf = Buffer.alloc(4);
        buf.writeInt32LE(x);
        this.write(buf);
    }

    protected writeString(str: string): void {
        for (const c of str) {
            this.writeU8(c.charCodeAt(0));
        }
    }

    protected write(buff: Buffer): void {
        this.stream.write(buff);
    }

    public close(): void {
        this.stream.close();
    }
}

export class FormatWriter extends BinaryWriter {
    public constructor(file: string) {
        super(file);
    }

    public writeFile(file: FileFormat): void {
        this.writeHeader(file.header);
        this.writeStringTable(file.stringTable);
        this.writeDataTable(file.dataSection);
    }

    private writeHeader(header: FileHeader) {
        this.writeU8(header.magic[0]);
        this.writeU8(header.magic[1]);
        this.writeU8(header.magic[2]);
        this.writeU16(header.version);
        this.writeU32(header.hash);
        this.writeU32(header.stringTableStart);
        this.writeU32(header.dataSectionStart);
    }

    private writeStringTable(table: StringTable): void {
        this.writeU32(table.length);
        this.writeU32(table.count);
        for (const offset of table.offsets) {
            this.writeU32(offset);
        }
        for (const string of table.strings) {
            this.writeString(string)
        }
    }

    private writeDataTable(section: DataSection): void {
        this.writeU32(section.length);
        this.writeU32(section.count);
        for (const instruction of section.instructions) {
            this.writeU8(instruction.opcode);
            for (const operand of instruction.operand) {
                this.write(operand);
            }
        }
    }
}