import {EngineArchitecture, EnginePlatform, generateRID} from "./structure";
import * as path from "path";
import * as child_process from "child_process";
import * as fs from "fs";
import {createFolder} from "./helper";

const ENGINE_BASE: string = path.join(__dirname, "..", "..", "..", "atomix");

const CC: string[] = ["zig", "cc"];
const CC_BASE_FLAGS: string[] = ["-Wall", "-std=gnu99"];

const AR: string[] = ["zig", "ar"];

interface ModInfo {
    loader: string[];
}

interface CDFItem {
    directory: string;
    arguments: string[];
    file: string;
}

class Gateway {
    public readonly archiver: Archiver;
    public readonly compiler: Compiler;
    public readonly platform: EnginePlatform;
    public readonly architecture: EngineArchitecture;

    public get CC_BASE_FLAGS(): string[] {
        const ADDITIONAL_FLAGS: string[] = [];
        if (this.platform == EnginePlatform.LINUX) {
            // ADDITIONAL_FLAGS.push("-Wl,-T=\"" + path.join(ENGINE_BASE, "atomix.ld") + "\"");
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
                platform = "linux-musl";
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

    public buildCDFArray(output: string, args: string[]): string[] {
        return [
            ...CC,
            ...this.info.CC_BASE_FLAGS,
            "-c",
            "-o",
            output,
            ...args,
            "-target",
            this.info.getZigTarget()
        ];
    }

    public compile(input: string, output: string, args: string[]): void {
        const execString: string = [...CC, ...this.info.CC_BASE_FLAGS, "-c", input, "-o", output, ...args, "-target", this.info.getZigTarget()].map(x => `"${x}"`).join(" ");

        child_process.execSync(execString, {
            encoding: "utf-8"
        });
    }

    public compileFromInput(input: string, output: string, args: string[]): void {
        const execString: string = [...CC, ...this.info.CC_BASE_FLAGS, "-x", "c", "-", "-c", "-o", output, ...args, "-target", this.info.getZigTarget()].map(x => `"${x}"`).join(" ");

        child_process.execSync(execString, {
            encoding: "utf-8",
            input: input,
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
            this.compileModLoader(),
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

    public cdf(): CDFItem[] {
        return [
            ...this.getCoreCDF(),
            ...this.getLoaderCDF(),
            ...this.modules.map(module => this.getModuleCDF(module)).flat()
        ];
    }

    private getCoreCDF(): CDFItem[] {
        const files: string[] = this.readdirSync(path.join(ENGINE_BASE, "core")).filter(file => file.endsWith(".c"));
        return files.map(file => ({
            directory: ENGINE_BASE,
            arguments: this.gateway.compiler.buildCDFArray(path.join(this.objFolder, "core", path.basename(file) + ".o"), []),
            file: file,
        }));
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

    private getLoaderCDF(): CDFItem[] {
        const files: string[] = this.readdirSync(path.join(ENGINE_BASE, this.debug ? "debug" : "release")).filter(file => file.endsWith(".c"));
        return files.map(file => ({
            directory: ENGINE_BASE,
            arguments: this.gateway.compiler.buildCDFArray(path.join(this.objFolder, "loader", path.basename(file) + ".o"), ["-I", path.join(ENGINE_BASE, "core")]),
            file: file,
        }));
    }

    private compileLoader(): string {
        const base: string = path.join(this.objFolder, "loader");
        createFolder(base);

        const inputFiles: string[] = this.readdirSync(path.join(ENGINE_BASE, this.debug ? "debug" : "release")).filter(file => file.endsWith(".c"));
        const objectFiles: string[] = this.compileCFiles(inputFiles, base, ["-I", path.join(ENGINE_BASE, "core")]);

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

    private compileModLoader(): string {
        const output: string = path.join(this.objFolder, "mod_loader.o");
        const loaders: string[] = this.modules.map(module => EngineBuilder.getModuleInfo(module).loader).flat();
        this.gateway.compiler.compileFromInput(`
        #include <stddef.h>

        ${loaders.map(loader => `extern void ${loader}(void*);`).join('\n')}

        const void* __MOD_LOADER__[] = {
            ${loaders.map(loader => `(const void*)${loader}`).join(",\n    ")}
        };
        const size_t __MOD_LOADER_SIZE__ = ${loaders.length};
        `, output, []);
        return output;
    }

    private getModuleCDF(module: string): CDFItem[] {
        const files: string[] = this.readdirSync(path.join(ENGINE_BASE, "modules", module)).filter(file => file.endsWith(".c"));
        return files.map(file => ({
            directory: ENGINE_BASE,
            arguments: this.gateway.compiler.buildCDFArray(path.join(this.objFolder, `mod_${module}`, path.basename(file) + ".o"), ["-I", path.join(ENGINE_BASE, "core")]),
            file: file
        }));
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

    public static createCDF(output: string, platform: EnginePlatform, architecture: EngineArchitecture, modules: string[], debug: boolean) {
        if (modules.length == 0) {
            modules = this.getAllModules();
        } else {
            const allModules: string[] = this.getAllModules();
            for (const module of modules) {
                if (!allModules.includes(module)) {
                    console.log(`Module '${module}' not found. Available modules: ${allModules.join(", ")}`);
                    process.exit(1);
                }
            }
        }

        const cdf: CDFItem[] = new EngineBuilder(process.cwd(), platform, architecture, modules, debug).cdf();
        fs.writeFileSync(output, JSON.stringify(cdf, null, 4));
    }

    public static getAllModules(): string[] {
        const base: string = path.join(ENGINE_BASE, "modules");
        return fs.readdirSync(base).filter(dir => fs.statSync(path.join(base, dir)).isDirectory());
    }

    public static getModuleInfo(module: string): ModInfo {
        const file: string = path.join(ENGINE_BASE, "modules", module, "mod.json");
        return JSON.parse(fs.readFileSync(file, "utf-8"));
    }
}