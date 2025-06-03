import { Section } from "./section";
import { Size } from "../size";
import { Instruction, OPCODE_SIZE, Operand } from "../opcodes";
import { BinaryReader, BinaryWriter } from "../binary";

export class DataSection implements Section {
    private length: Size;
    private count: number;
    private instructions: Instruction[];

    public constructor() {
        this.length = Size.new(2, "int");
        this.count = 0;
        this.instructions = [];
    }

    public addInstruction(instruction: Instruction): number {
        this.length.add(OPCODE_SIZE.inBytes(), "byte");
        for (const operand of instruction.operands) {
            this.length.add(operand.length.inBytes(), "bytes");
        }
        const idx: number = this.count++;
        this.instructions.push(instruction);
        return idx;
    }

    public replaceInstruction(index: number, instruction: Instruction): void {
        const oldInstruction = this.instructions[index];
        for (const operand of oldInstruction.operands) {
            this.length.subtract(operand.length.inBytes(), "bytes");
        }
        for (let operand of instruction.operands) {
            this.length.add(operand.length.inBytes(), "bytes");
        }
        this.instructions[index] = instruction;
    }

    public getCount(): number {
        return this.count;
    }

    public getLength(): number {
        return this.length.inBytes();
    }

    public writeTo(writer: BinaryWriter): void {
        writer.writeU32(this.length.inBytes());
        writer.writeU32(this.count);
        for (const instruction of this.instructions) {
            writer.writeU8(instruction.opcode);
            for (const operand of instruction.operands) {
                operand.writeTo(writer);
            }
        }
    }

    public readFrom(reader: BinaryReader): void {
        this.length = Size.new(reader.readU32(), "bytes");
        this.count = reader.readU32();
        for (let i: number = 0; i < this.count; i++) {
            const operand: Operand = reader.readU8();
        }
    }

    public *[Symbol.iterator](): Generator<[Instruction, number]> {
        for (let i: number = 0; i < this.count; i++) {
            yield [this.instructions[i], i];
        }
    }
}
