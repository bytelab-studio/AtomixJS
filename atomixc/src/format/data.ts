import {Section} from "./section";
import {Size, Unit} from "../size";
import {
    ConstantDoubleOperand,
    ConstantIntegerOperand,
    ConstantUNumberOperand,
    Instruction,
    OPCODE_SIZE,
    Opcodes,
    Operand
} from "../opcodes";
import {BinaryReader, BinaryWriter} from "../binary";


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
        const uConstOperand = (unit: Unit) => () => {
            switch (unit) {
                case "byte":
                case "bytes":
                    return new ConstantUNumberOperand(reader.readU8(), unit);
                case "short":
                case "shorts":
                    return new ConstantUNumberOperand(reader.readU16(), unit);
                case "int":
                case "ints":
                    return new ConstantUNumberOperand(reader.readU32(), unit);
                case "long":
                case "longs":
                    throw "Not implemented";
            }
        }

        const operandDictionary: Partial<Record<Opcodes, Array<() => Operand>>> = {
            [Opcodes.LD_INT]: [() => new ConstantIntegerOperand(reader.readI32())],
            [Opcodes.LD_DOUBLE]: [() => new ConstantDoubleOperand(reader.readDouble())],
            [Opcodes.LD_STRING]: [uConstOperand("short")],
            [Opcodes.ALLOC_LOCAL]: [uConstOperand("short")],
            [Opcodes.STORE_LOCAL]: [uConstOperand("short")],
            [Opcodes.LOAD_LOCAL]: [uConstOperand("short")],
            [Opcodes.LOAD_ARG]: [uConstOperand("short")],
            [Opcodes.DECLARE_FUNC]: [uConstOperand("short"), uConstOperand("short")],
            [Opcodes.DECLARE_FUNC_E]: [uConstOperand("short")],
            [Opcodes.CALL]: [uConstOperand("short")],
            [Opcodes.OBJ_STORE]: [uConstOperand("short")],
            [Opcodes.OBJ_LOAD]: [uConstOperand("short")],
            [Opcodes.JMP]: [uConstOperand("short")],
            [Opcodes.JMP_F]: [uConstOperand("short")],
            [Opcodes.JMP_T]: [uConstOperand("short")],
            [Opcodes.EXPORT]: [uConstOperand("short")]
        }

        this.length = Size.new(reader.readU32(), "bytes");
        this.count = reader.readU32();
        for (let i: number = 0; i < this.count; i++) {
            const instruction: Instruction = new Instruction(reader.readU8());
            if (instruction.opcode in operandDictionary) {
                for (const operand of operandDictionary[instruction.opcode]!) {
                    instruction.addOperand(operand());
                }
            }
            this.instructions.push(instruction);
        }
    }

    public* [Symbol.iterator](): Generator<[Instruction, number]> {
        for (let i: number = 0; i < this.count; i++) {
            yield [this.instructions[i], i];
        }
    }
}
