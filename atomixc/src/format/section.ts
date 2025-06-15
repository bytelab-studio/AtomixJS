import { BinaryReader, BinaryWriter } from "../binary";

export interface Section {
    writeTo(writer: BinaryWriter): void;

    readFrom(reader: BinaryReader): void; 

    getLength(): number;
}