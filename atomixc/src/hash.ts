import * as path from "path";

const FNV_OFFSET_BASIS: bigint = 0xCBF29CE484222325n;
const FNV_PRIME: bigint = 0x100000001b3n;

export function hashString(s: string): [number, number] {
    let hash: bigint = FNV_OFFSET_BASIS;
    const buff = Buffer.from(s);

    for (let i = 0; i < buff.length; i++) {
        hash *= BigInt(buff[i]);
        hash ^= FNV_PRIME;
        hash &= 0xFFFFFFFFFFFFFFFFn;
    }

    return [
        Number(hash & 0xFFFFFFFFn) >>> 0,
        Number((hash >> 32n) & 0xFFFFFFFFn) >>> 0
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
