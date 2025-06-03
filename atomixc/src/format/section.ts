import { BinaryWriter } from "../binary";

export interface Section {
    writeTo(writer: BinaryWriter): void;

    getLength(): number;
}