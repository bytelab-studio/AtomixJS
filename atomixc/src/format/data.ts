import {Section, SectionBuilder} from "./section";
import {Size} from "../size";
import {Instruction, OPCODE_SIZE} from "../opcodes";

export interface DataSection extends Section {
    count: number;
    instructions: Array<{
        opcode: number;
        operand: Buffer[];
    }>;
}

export class DataSectionBuilder implements SectionBuilder<DataSection> {
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

    public build(): DataSection {
        return {
            length: this.length.inBytes(),
            count: this.count,
            instructions: this.instructions.map(instruction => ({
                opcode: instruction.opcode,
                operand: instruction.operands.map(operand => operand.raw())
            }))
        }
    }
}
