import {Size, Unit} from "./size";

export enum Opcodes {
    NOP,
    LD_INT,
    LD_DOUBLE,
    LD_STRING,
    LD_UNDF,
    LD_NULL,
    LD_TRUE,
    LD_FALSE,
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
    OBJ_ALLOC,
    OBJ_STORE,
    OBJ_LOAD,
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

    raw(): Buffer;
}

export class ConstantIntegerOperand implements Operand {
    value: number;
    length: Size;

    public constructor(value: number) {
        this.value = value;
        this.length = Size.new(1, "int");
    }

    public raw(): Buffer {
        const buf = Buffer.alloc(4);
        buf.writeInt32LE(this.value);
        return buf;
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

    public raw(): Buffer {
        const buf = Buffer.alloc(this.length.inBytes());
        switch (this.unit) {
            case "byte":
            case "bytes":
                buf.writeUint8(this.value, 0);
                break;
            case "short":
            case "shorts":
                buf.writeUint16LE(this.value, 0);
                break;
            case "int":
            case "ints":
                buf.writeUint32LE(this.value, 0);
                break;
            case "long":
            case "longs":
                throw "Not implemented";
            // break;

        }
        return buf;
    }

}

export class ConstantDoubleOperand implements Operand {
    value: number;
    length: Size;

    public constructor(value: number) {
        this.value = value;
        this.length = Size.new(1, "int");
    }

    public raw(): Buffer {
        const buf = Buffer.alloc(8);
        buf.writeDoubleLE(this.value);
        return buf;
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