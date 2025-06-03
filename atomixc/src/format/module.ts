import { STableSection } from "./stable";
import { Size } from "../size";
import { DataSection } from "./data";
import { Section } from "./section";
import { BinaryReader, BinaryWriter } from "../binary";

const MAGIC: [number, number, number, number] = [46, 65, 120, 77];
const VERSION: number = 2;

export class ModuleFormat implements Section {
    public header: ModuleHeader;
    public stableSection: STableSection;
    public dataSection: DataSection;

    public constructor(header: ModuleHeader, stableSection: STableSection, dataSection: DataSection) {
        this.header = header;
        this.stableSection = stableSection;
        this.dataSection = dataSection;
    }

    public getLength(): number {
        return Size.new()
            .add(4, "bytes")
            .add(1, "short")
            .add(1, "long")
            .add(2, "ints")
            .add(this.stableSection.getLength(), "bytes")
            .add(this.dataSection.getLength(), "bytes")
            .inBytes();
    }

    public writeTo(writer: BinaryWriter): void {
        writer.writeU8(this.header.magic[0]);
        writer.writeU8(this.header.magic[1]);
        writer.writeU8(this.header.magic[2]);
        writer.writeU8(this.header.magic[3]);
        writer.writeU16(this.header.version);
        writer.writeU32(this.header.hash[0]);
        writer.writeU32(this.header.hash[1]);
        writer.writeU32(this.header.stableSectionStart);
        writer.writeU32(this.header.dataSectionStart);

        this.stableSection.writeTo(writer);
        this.dataSection.writeTo(writer);
    }

    public readFrom(reader: BinaryReader): void {
        this.header.magic[0] = reader.readU8();
        this.header.magic[1] = reader.readU8();
        this.header.magic[2] = reader.readU8();
        this.header.magic[3] = reader.readU8();
        this.header.version = reader.readU16();
        this.header.hash[0] = reader.readU32();
        this.header.hash[1] = reader.readU32();
        this.header.stableSectionStart = reader.readU32();
        this.header.dataSectionStart = reader.readU32();
        this.stableSection.readFrom(reader);
        this.dataSection.readFrom(reader);
    }
}

export interface ModuleHeader {
    magic: [number, number, number, number];
    version: number;
    hash: [number, number];
    stableSectionStart: number;
    dataSectionStart: number;
}

export function buildModule(hash: [number, number], stableSection: STableSection, dataSection: DataSection): ModuleFormat {
    const baseStart: Size = Size
        .new()
        .add(4, "byte")
        .add(1, "short")
        .add(1, "long")
        .add(2, "int");

    const header: ModuleHeader = {
        magic: MAGIC,
        version: VERSION,
        hash: hash,
        stableSectionStart: baseStart.inBytes(),
        dataSectionStart: baseStart.add(stableSection.getLength(), "bytes").inBytes()
    }

    return new ModuleFormat(header, stableSection, dataSection);
}