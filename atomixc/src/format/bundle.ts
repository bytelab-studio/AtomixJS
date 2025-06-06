import {Section} from "./section";
import {ModuleFormat} from "./module";
import {BinaryReader, BinaryWriter} from "../binary";
import {Size} from "../size";

export interface BundleHeader {
    magic: [number, number, number, number];
    version: number;
    entryHash: [number, number];
    count: number;
}

const MAGIC: [number, number, number, number] = [46, 65, 120, 66];
const VERSION: number = 1;

export class BundleFormat implements Section {
    public header: BundleHeader;
    public modules: ModuleFormat[];

    public constructor(header: BundleHeader, modules: ModuleFormat[]) {
        this.header = header;
        this.modules = modules;
    }

    public writeTo(writer: BinaryWriter): void {
        writer.writeU8(this.header.magic[0]);
        writer.writeU8(this.header.magic[1]);
        writer.writeU8(this.header.magic[2]);
        writer.writeU8(this.header.magic[3]);
        writer.writeU16(this.header.version);
        writer.writeU32(this.header.entryHash[0]);
        writer.writeU32(this.header.entryHash[1]);
        writer.writeU16(this.header.count);

        for (const module of this.modules) {
            module.writeTo(writer);
        }
    }

    public getLength(): number {
        return Size.new()
            .add(4, "bytes")
            .add(1, "short")
            .add(1, "long")
            .add(1, "short")
            .inBytes() + this.modules.reduce((a: number, b: ModuleFormat): number => a + b.getLength(), 0);
    }

    public readFrom(reader: BinaryReader): void {
        this.header.magic[0] = reader.readU8();
        this.header.magic[1] = reader.readU8();
        this.header.magic[2] = reader.readU8();
        this.header.magic[3] = reader.readU8();
        this.header.version = reader.readU16();
        this.header.entryHash[0] = reader.readU32();
        this.header.entryHash[1] = reader.readU32();
        this.header.count = reader.readU16();

        for (let i: number = 0; i < this.header.count; i++) {
            this.modules.push(ModuleFormat.readFrom(reader));
        }
    }

    public static readFrom(reader: BinaryReader): BundleFormat {
        const bundle: BundleFormat = new BundleFormat({
            magic: [0, 0, 0, 0],
            version: 0,
            entryHash: [0, 0],
            count: 0
        }, []);
        bundle.readFrom(reader);
        return bundle;
    }

    public static isBundle(reader: BinaryReader):  boolean {
        for (let i: number = 0; i < MAGIC.length; i++) {
            if (reader.at(i) != MAGIC[i]) {
                return false;
            }
        }

        return true;
    }
}

export function buildBundle(entryHash: [number, number], modules: ModuleFormat[]): BundleFormat {
    modules.sort((a: ModuleFormat, b: ModuleFormat) => {
        if (a.header.hash[0] < b.header.hash[0]) return -1;
        if (a.header.hash[0] > b.header.hash[0]) return 1;
        if (a.header.hash[1] < b.header.hash[1]) return -1;
        if (a.header.hash[1] > b.header.hash[1]) return 1;
        return 0;
    });

    const header: BundleHeader = {
        magic: MAGIC,
        version: VERSION,
        entryHash: entryHash,
        count: modules.length
    }

    return new BundleFormat(header, modules);
}
