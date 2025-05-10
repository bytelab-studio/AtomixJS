import * as fs from "fs";

export function createFolder(folder: string): void {
    if (fs.existsSync(folder) && fs.statSync(folder).isDirectory()) {
        return;
    }

    fs.mkdirSync(folder);
}