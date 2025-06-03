import * as fs from "fs";

export class BinaryWriter {
    private stream: fs.WriteStream;

    public constructor(file: string) {
        this.stream = fs.createWriteStream(file);
    }

    public writeU8(x: number): void {
        const buf = Buffer.alloc(1);
        buf.writeUint8(x);
        this.write(buf);
    }

    public writeI8(x: number): void {
        this.writeU8(x);
    }

    public writeU16(x: number): void {
        const buf = Buffer.alloc(2);
        buf.writeUint16LE(x);
        this.write(buf);
    }

    public writeI16(x: number): void {
        const buf = Buffer.alloc(2);
        buf.writeInt16LE(x);
        this.write(buf);
    }

    public writeU32(x: number): void {
        const buf = Buffer.alloc(4);
        buf.writeUint32LE(x);
        this.write(buf);
    }

    public writeI32(x: number): void {
        const buf = Buffer.alloc(4);
        buf.writeInt32LE(x);
        this.write(buf);
    }

    public writeDouble(x: number): void {
        const buf = Buffer.alloc(8);
        buf.writeDoubleLE(x);
        this.write(buf);        
    }

    public writeString(str: string): void {
        for (const c of str) {
            this.writeU8(c.charCodeAt(0));
        }
    }

    public write(buff: Buffer): void {
        this.stream.write(buff);
    }

    public close(): void {
        this.stream.close();
    }
}

export class BinaryReader {
    private buffer: Buffer;
    private pos: number;

    public constructor(file: string | Buffer) {
        if (file instanceof Buffer) {
            this.buffer = file;
        } else {
            this.buffer = fs.readFileSync(file);
        }

        this.pos = 0;
    }

    public readU8(): number {
        const val: number = this.buffer.readUInt8(this.pos);
        this.pos += 1;
        return val;
    }

    public readI8(): number {
        const val: number = this.buffer.readInt8(this.pos);
        this.pos += 1;
        return val;
    }

    public readU16(): number {
        const val: number = this.buffer.readUInt16LE(this.pos);
        this.pos += 2;
        return val;
    }

    public readI16(): number {
        const val: number = this.buffer.readInt16LE(this.pos);
        this.pos += 2;
        return val;
    }

    public readU32(): number {
        const val: number = this.buffer.readUInt32LE(this.pos);
        this.pos += 4;
        return val;
    }

    public readI32(): number {
        const val: number = this.buffer.readInt32LE(this.pos);
        this.pos += 4;
        return val;
    }

    public readDouble(): number {
        const val: number = this.buffer.readDoubleLE(this.pos);
        this.pos += 8;
        return val;
    }

    public readString(length: number): string {
        const val: string = this.buffer.toString('utf8', this.pos, this.pos + length);
        this.pos += length;
        return val;
    }
}
