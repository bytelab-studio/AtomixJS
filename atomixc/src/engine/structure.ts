import * as path from "path";
import {createFolder} from "./helper";
import {EngineBuilder} from "./builder";

export enum EnginePlatform {
    WINDOWS,
    LINUX
}

export enum EngineArchitecture {
    X64,
    ARM64
}

export function generateRID(platform: EnginePlatform, architecture: EngineArchitecture): string {
    let plat: string;

    switch (platform) {
        case EnginePlatform.WINDOWS:
            plat = "windows";
            break;
        case EnginePlatform.LINUX:
            plat = "linux";
            break;
        default:
            throw "Unexpected platform";
    }

    let arch: string;
    switch (architecture) {
        case EngineArchitecture.X64:
            arch = "x86_64";
            break;
        case EngineArchitecture.ARM64:
            arch = "arm64";
            break;
    }

    return `${plat}-${arch}`;
}


export function initStructure(base: string): void {
    const FOLDERS: string[][] = [
        [".atomix"],
        [".atomix", "obj"],
        [".atomix", "bin"],
        [".atomix", "bc"],
        [".atomix", "cache"],
        [".atomix", "logs"],
    ];

    for (const folder of FOLDERS) {
        createFolder(path.join(base, ...folder));
    }
}

export function initEngineBuild(base: string, platform: EnginePlatform, architecture: EngineArchitecture): void {
    const dir: string = path.join(base, ".atomix");
    const FOLDERS: string[][] = [
        ["obj", "Debug"],
        ["obj", "Debug", generateRID(platform, architecture)],
        ["obj", "Release"],
        ["obj", "Release", generateRID(platform, architecture)],
        ["bin", "Debug"],
        ["bin", "Debug", generateRID(platform, architecture)],
        ["bin", "Release"],
        ["bin", "Release", generateRID(platform, architecture)],
    ];
    for (const folder of FOLDERS) {
        createFolder(path.join(dir, ...folder));
    }

    EngineBuilder.createEngine(base, platform, architecture, EngineBuilder.getAllModules(), true);
}