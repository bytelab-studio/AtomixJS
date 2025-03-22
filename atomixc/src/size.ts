export type Unit = "byte" | "bytes" | "short" | "shorts" | "int" | "ints" | "long" | "longs";

export class Size {
    private bytes: number;

    public constructor();
    public constructor(x: number, unit: Unit);

    public constructor(x?: number, unit?: Unit) {
        this.bytes = 0;

        if (x && unit) {
            this.add(x, unit);
        }
    }

    public add(x: number, unit: Unit): this {
        switch (unit) {
            case "byte":
            case "bytes":
                this.bytes += x;
                break;
            case "short":
            case "shorts":
                this.bytes += x * 2;
                break;
            case "int":
            case "ints":
                this.bytes += x * 4;
                break;
            case "long":
            case "longs":
                this.bytes += x * 8;
                break;
        }

        return this;
    }

    public subtract(x: number, unit: Unit): this {
        return this.add(-x, unit);
    }

    public inBytes(): number {
        return this.bytes;
    }

    public copy(): Size {
        return new Size(this.bytes, "bytes");
    }

    public static new(): Size;
    public static new(x: number, unit: Unit): Size;

    public static new(x?: number, unit?: Unit): Size {
        return x && unit ? new Size(x, unit) : new Size();
    }
}