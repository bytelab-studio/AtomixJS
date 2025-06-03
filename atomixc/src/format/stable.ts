import { Section } from "./section";
import { Size } from "../size";
import { BinaryReader, BinaryWriter } from "../binary";

export class STableSection implements Section {
    private length: Size;
    private count: number;
    private offsets: Size[];
    private strings: string[];

    public constructor() {
        this.length = Size.new().add(2, "ints");
        this.count = 0;
        this.offsets = [];
        this.strings = [];
    }

    public registerString(x: string): number {
        let idx: number = this.strings.indexOf(x);
        if (idx != -1) {
            return idx;
        }

        let offset : Size = this.count == 0
            ? Size.new()
            : this.offsets[this.count - 1]
                .copy()
                .add(this.strings[this.count - 1].length, "bytes");

        idx = this.count++;
        this.strings.push(x);
        this.offsets.push(offset);
        this.length.add(1, "int").add(x.length, "bytes");
        return idx;
    }

    public getLength(): number {
        return this.length.inBytes();
    }

    public writeTo(writer: BinaryWriter): void {
        writer.writeU32(this.length.inBytes());
        writer.writeU32(this.count);
        for (const offset of this.offsets) {
            writer.writeU32(offset.inBytes());
        }
        for (const string of this.strings) {
            writer.writeString(string);
        }
    }

    public readFrom(reader: BinaryReader): void {
        this.length = Size.new(reader.readU32(), "bytes");
        this.count = reader.readU32();
        for (let i: number = 0; i < this.count; i++) {
            this.offsets.push(Size.new(reader.readU32(), "bytes"));
        }

        for (let i: number = 0; i < this.count; i++) {
            const start: Size = this.offsets[i];
            const end: Size = i + 1 < this.count ? this.offsets[i + 1] : this.length;
            const length: number = end.inBytes() - start.inBytes();
            this.strings.push(reader.readString(length));
        }
    }

    public *[Symbol.iterator](): Generator<[number, string, number]> {
        for (let i: number = 0; i < this.count; i++) {
            yield [this.offsets[i].inBytes(), this.strings[i], i];
        }
    }
}