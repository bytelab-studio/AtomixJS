import {Section, SectionBuilder} from "./section";
import {Size} from "../size";

export interface StringTable extends Section {
    count: number;
    offsets: number[];
    strings: string[];
}

export class StringTableBuilder implements SectionBuilder<StringTable> {
    private length: Size;
    private count: number;
    private offsets: Size[];
    private strings: string[];

    public constructor() {
        this.length = Size.new().add(2, "ints");
        this.count = 0;
        this.offsets = [];
        this.strings = [];
    }

    public registerString(x: string): number {
        let idx: number = this.strings.indexOf(x);
        if (idx != -1) {
            return idx;
        }

        let offset : Size = this.count == 0
            ? Size.new()
            : this.offsets[this.count - 1]
                .copy()
                .add(this.strings[this.count - 1].length, "bytes");

        idx = this.count++;
        this.strings.push(x);
        this.offsets.push(offset);
        this.length.add(1, "int").add(x.length, "bytes");
        return idx;
    }

    public build(): StringTable {
        return {
            length: this.length.inBytes(),
            count: this.count,
            offsets: this.offsets.map(x => x.inBytes()),
            strings: this.strings
        }
    }
}