import { ModuleFormat } from "./format/module";
import { Opcodes } from "./opcodes";

export class Dumper {
    private writeIntend(str: string | number): void {
        console.log("    " + str);
        
    }

    public dumpModule(module: ModuleFormat): void {
        console.log("$HEADER:");
        this.writeIntend(`MAGIC: ${module.header.magic[0]} ${module.header.magic[1]} ${module.header.magic[2]} ${module.header.magic[3]}`);
        this.writeIntend(`VERSION: ${module.header.version}`);
        this.writeIntend(`HASH: 0x${module.header.hash[0].toString(16)}${module.header.hash[1].toString(16)}`);
        this.writeIntend(`$STABLE: 0x${module.header.stableSectionStart.toString(16)}`);
        this.writeIntend(`$DATA: 0x${module.header.dataSectionStart.toString(16)}`);
        console.log();           
    
        console.log("$STABLE:");
        for (const [offset, string, i] of module.stableSection) {
            this.writeIntend(`${i.toString().padStart(3, "0")}: ${(`"${string}"`).padEnd(15, " ")} at 0x${offset.toString(16)}`);
        }
        console.log();

        console.log("$DATA:");
        for (const [instruction, i] of module.dataSection) {
            this.writeIntend(`${i.toString().padStart(3, "0")}: ${Opcodes[instruction.opcode].padEnd(15, " ")} ${instruction.operands.map(operand => operand.value).join(", ")}`);
        }
    }
}