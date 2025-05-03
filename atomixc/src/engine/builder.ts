import {EngineArchitecture, EnginePlatform, generateRID} from "./structure";
import * as path from "path";
import * as child_process from "child_process";
import * as fs from "fs";
import {createFolder} from "./helper";

const ENGINE_BASE: string = path.join(__dirname, "..", "..", "..", "atomix");

const CC: string[] = ["zig", "cc"];
const CC_BASE_FLAGS: string[] = ["-Wall", "-std=gnu99"];

const AR: string[] = ["zig", "ar"];

class Gateway {
    public readonly archiver: Archiver;
    public readonly compiler: Compiler;
    public readonly platform: EnginePlatform;
    public readonly architecture: EngineArchitecture;

    public get CC_BASE_FLAGS(): string[] {
        const ADDITIONAL_FLAGS: string[] = [];
        if (this.platform == EnginePlatform.LINUX) {
            ADDITIONAL_FLAGS.push("-Wl,-T=\"" + path.join(ENGINE_BASE, "atomix.ld") + "\"");
        }

        return CC_BASE_FLAGS.concat(ADDITIONAL_FLAGS);
    }

    constructor(platform: EnginePlatform, architecture: EngineArchitecture) {
        this.platform = platform;
        this.architecture = architecture;
        this.archiver = new Archiver(this);
        this.compiler = new Compiler(this);
    }

    public getZigTarget(): string {
        let platform: string;
        switch (this.platform) {
            case EnginePlatform.WINDOWS:
                platform = "windows-gnu";
                break;
            case EnginePlatform.LINUX:
                platform = "linux-gnu";
                break;
            default:
                throw "Unexpected platform";
        }

        let arch: string;
        switch (this.architecture) {
            case EngineArchitecture.X64:
                arch = "x86_64";
                break;
            case EngineArchitecture.ARM64:
                arch = "aarch64";
                break;
            default:
                throw "Unexpected architecture";
        }

        return `${arch}-${platform}`;
    }
}

class Archiver {
    private info: Gateway;

    constructor(info: Gateway) {
        this.info = info;
    }

    public archive(inputs: string[], output: string, args: string[]): void {
        const execString: string = [...AR, "rcs", ...args, output, ...inputs].map(x => `"${x}"`).join(" ");

        child_process.execSync(execString, {
            encoding: "utf-8"
        });
    }
}

class Compiler {
    private info: Gateway;

    constructor(info: Gateway) {
        this.info = info;
    }

    public compile(input: string, output: string, args: string[]): void {
        const execString: string = [...CC, ...this.info.CC_BASE_FLAGS, "-c", input, "-o", output, ...args, "-target", this.info.getZigTarget()].map(x => `"${x}"`).join(" ");

        child_process.execSync(execString, {
            encoding: "utf-8"
        });
    }

    public link(args: string[], output: string): void {
        const execString: string = [...CC, ...this.info.CC_BASE_FLAGS, ...args, "-o", output, "-target", this.info.getZigTarget()].map(x => `"${x}"`).join(" ");

        child_process.execSync(execString, {
            encoding: "utf-8"
        });
    }
}

export class EngineBuilder {
    private gateway: Gateway;
    private readonly modules: string[];
    private readonly debug: boolean;
    private readonly objFolder: string;
    private readonly binFolder: string;

    private constructor(dir: string, platform: EnginePlatform, architecture: EngineArchitecture, modules: string[], debug: boolean) {
        this.gateway = new Gateway(platform, architecture);
        this.modules = modules;
        this.debug = debug;
        this.objFolder = path.join(dir, ".atomix", "obj", debug ? "Debug" : "Release", generateRID(platform, architecture));
        this.binFolder = path.join(dir, ".atomix", "bin", debug ? "Debug" : "Release", generateRID(platform, architecture));
    }

    public create(): void {
        const includes: string[] = [
            this.compileCore(),
            this.compileLoader(),
            "-Wl,--whole-archive",
            ...this.compileModules(),
            "-Wl,--no-whole-archive"
        ];

        if (!this.debug) {
            includes.push(this.packBytecode());
        }

        const name: string = this.gateway.platform === EnginePlatform.WINDOWS ? "runner.exe" : "runner";
        const result: string = path.join(this.binFolder, name);
        this.gateway.compiler.link(includes, result)
    }

    private compileCore(): string {
        const base: string = path.join(this.objFolder, "core");
        createFolder(base);

        const inputFiles: string[] = this.readdirSync(path.join(ENGINE_BASE, "core")).filter(file => file.endsWith(".c"));
        const objectFiles: string[] = this.compileCFiles(inputFiles, base, []);

        const result: string = path.join(this.objFolder, "libcore.a");
        this.gateway.archiver.archive(objectFiles, result, []);
        return result;
    }

    private compileLoader(): string {
        const base: string = path.join(this.objFolder, "loader");
        createFolder(base);

        const inputFiles: string[] = this.readdirSync(path.join(ENGINE_BASE, this.debug ? "debug" : "release")).filter(file => file.endsWith(".c"));
        const objectFiles: string[] = this.compileCFiles(inputFiles, base, ["-I",  path.join(ENGINE_BASE, "core")]);

        const result: string = path.join(this.objFolder, "libloader.a");
        this.gateway.archiver.archive(objectFiles, result, []);
        return result;
    }

    private compileModules(): string[] {
        const results: string[] = [];

        for (const module of this.modules) {
            const dir: string = path.join(ENGINE_BASE, module);
            if (!fs.existsSync(dir)) {
                continue;
            }

            results.push(this.compileModule(module));
        }

        return results;
    }

    private compileModule(module: string): string {
        const base: string = path.join(this.objFolder, `mod_${module}`);
        createFolder(base);

        const inputFiles: string[] = this.readdirSync(path.join(ENGINE_BASE, "modules", module)).filter(file => file.endsWith(".c"));
        const objectFiles: string[] = this.compileCFiles(inputFiles, base, ["-I", path.join(ENGINE_BASE, "core")]);

        const result: string = path.join(this.objFolder, `libmod_${module}.a`);
        this.gateway.archiver.archive(objectFiles, result, []);
        return result;
    }

    private compileCFiles(files: string[], outputDir: string, args: string[]): string[] {
        const objFiles: string[] = [];
        for (const file of files) {
            const objFile = path.join(outputDir, path.basename(file) + ".o");
            this.gateway.compiler.compile(file, objFile, args);
            objFiles.push(objFile);
        }

        return objFiles;
    }

    private packBytecode(): string {
        throw "Not implemented";
    }

    private readdirSync(folder: string): string[] {
        return fs.readdirSync(folder).map(file => path.join(folder, file));
    }

    public static createEngine(dir: string, platform: EnginePlatform, architecture: EngineArchitecture, modules: string[], debug: boolean) {
        new EngineBuilder(dir, platform, architecture, modules, debug).create();
    }

    public static getAllModules(): string[] {
        const base: string = path.join(ENGINE_BASE, "modules");
        return fs.readdirSync(base).filter(dir => fs.statSync(path.join(base, dir)).isDirectory());
    }
}