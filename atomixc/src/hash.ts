import * as crypto from "crypto";
import * as path from "path";

export function hashString(s: string): [number, number] {
    const hash: Buffer = crypto.createHash("sha256").update(s).digest();

    return [
        hash.readUInt32LE(0) >>> 0,
        hash.readUInt32LE(4) >>> 0
    ];
}

export function hashFilePath(file: string, root: string, prefix: string): [number, number] {
    if (!path.isAbsolute(file)) {
        file = path.posix.join(process.cwd(), file).replace(/\\/g, "/");
    }
    if (!path.isAbsolute(root)) {
        root = path.posix.join(process.cwd(), root).replace(/\\/g, "/");
    }

    const signature: string = prefix + file.replace(root, "");
    return hashString(signature);
}
