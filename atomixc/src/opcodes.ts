import { Size, Unit } from "./size";
import { BinaryWriter } from "./binary";

export enum Opcodes {
    NOP,
    LD_INT,
    LD_DOUBLE,
    LD_STRING,
    LD_UNDF,
    LD_NULL,
    LD_TRUE,
    LD_FALSE,
    LD_THIS,
    ADD,
    MINUS,
    MUL,
    DIV,
    MOD,
    BINARY_AND,
    BINARY_OR,
    BINARY_XOR,
    BINARY_LSHFT,
    BINARY_RSHFT,
    BINARY_ZRSHFT,
    BINARY_NOT,
    NOT,
    NEGATE,
    TYPEOF,
    TEQ,
    NTEQ,
    GT,
    GEQ,
    LT,
    LEQ,
    POP,
    DUP,
    SWAP,
    ALLOC_LOCAL,
    STORE_LOCAL,
    LOAD_LOCAL,
    LOAD_ARG,
    DECLARE_FUNC,
    DECLARE_FUNC_E,
    CALL,
    ARR_ALLOC,
    OBJ_ALLOC,
    OBJ_STORE,
    OBJ_LOAD,
    OBJ_CLOAD,
    OBJ_CSTORE,
    RETURN,
    PUSH_SCOPE,
    POP_SCOPE,
    JMP,
    JMP_F,
    JMP_T,
    EXPORT
}

export const OPCODE_SIZE = Size.new(1, "byte");

export interface Operand {
    value: number;
    length: Size;

    writeTo(writer: BinaryWriter): void;
}

export class ConstantIntegerOperand implements Operand {
    value: number;
    length: Size;

    public constructor(value: number) {
        this.value = value;
        this.length = Size.new(1, "int");
    }

    public writeTo(writer: BinaryWriter): void {
        writer.writeI32(this.value >> 0);
    }
}

export class ConstantUNumberOperand implements Operand {
    value: number;
    length: Size;

    private unit: Unit;

    public constructor(value: number, unit: Unit) {
        this.value = value;
        this.length = Size.new(1, unit);
        this.unit = unit;
    }

    public writeTo(writer: BinaryWriter): void {
        switch (this.unit) {
            case "byte":
            case "bytes":
                writer.writeU8(this.value);
                break;
            case "short":
            case "shorts":
                writer.writeU16(this.value);
                break;
            case "int":
            case "ints":
                writer.writeU32(this.value);
                break;
            case "long":
            case "longs":
                throw "Not implemented";
        }
    }
}

export class ConstantDoubleOperand implements Operand {
    value: number;
    length: Size;

    public constructor(value: number) {
        this.value = value;
        this.length = Size.new(1, "int");
    }

    public writeTo(writer: BinaryWriter): void {
        writer.writeDouble(this.value);
    }
}

export class Instruction {
    public readonly opcode: Opcodes;
    public readonly operands: Operand[];

    public constructor(opcode: Opcodes) {
        this.opcode = opcode;
        this.operands = [];
    }

    public addOperand(operand: Operand): this {
        this.operands.push(operand);
        return this;
    }
}