const fs = require("fs/promises");
const fsSync = require("fs");
const path = require("path");
const child_process = require("child_process");
const os = require("os");

const NODE_SUIT = path.join(__dirname, "utils", "node-suit.js");
const COMPILER = path.join(__dirname, "..", "atomixc", "dist", "bin", "atomixc.js");
const VM_RUNNER = os.platform() === "win32"
    ? path.join(__dirname, "..", "atomix", "cmake-build-debug", "debug", "atomix.exe")
    : path.join(__dirname, "..", "atomix", "cmake-build-debug", "debug", "atomix");
const WORKERS = 4;

class AsyncLock {
    constructor() {
        this.lock = Promise.resolve();
    }

    acquire(fn) {
        const next = this.lock.then(() => fn());
        this.lock = next.catch(() => {}); // Prevent lock from halting on errors
        return next;
    }
}

function* pipeFiles(dir) {
    const entries = fsSync.readdirSync(dir, {withFileTypes: true});

    for (const entry of entries) {
        const fullPath = path.join(dir, entry.name);

        if (entry.isDirectory()) {
            yield* pipeFiles(fullPath);
        } else if (fullPath.endsWith(".js")) {
            yield fullPath;
        }
    }
}

function runSubprocess(command, args, identifier) {
    return new Promise((resolve, reject) => {
        const child = child_process.spawn(command, args, {encoding: "utf-8"});

        let stdout = "";
        let stderr = "";

        child.stdout.on("data", data => stdout += data);
        child.stderr.on("data", data => stderr += data);

        child.on("error", reject);

        child.on("close", code => {
            if (code !== 0) {
                return reject(new Error(`Process exited with code ${code}: ${identifier}`));
            }
            resolve(stdout);
        });
    });
}

async function compileProgram(test) {
    const outputFile = test + ".bin";
    await runSubprocess("node", [COMPILER, test, outputFile], test);
    return outputFile;
}

async function runProgram(file) {
    const testResult = await runSubprocess(VM_RUNNER, [file], file);
    return testResult.replace(/\r\n/g, "\n");
}

async function getSnapshot(test) {
    const file = test + ".snp";
    try {
        const content = await fs.readFile(file, "utf-8");
        return content.replace(/\r\n/g, "\n");
    } catch (err) {
        if (err.code !== "ENOENT") throw err;
    }

    const output = await runSubprocess("node", [NODE_SUIT, test], test);

    await fs.writeFile(file, output, "utf-8");

    return output;
}


const tests = Array.from(pipeFiles(path.join(__dirname, "src")));
let testCount = tests.length;
const failedFiles = [];
const mutex = new AsyncLock();

Promise.all(Array.from({length: WORKERS}, async () => {
    while (tests.length > 0) {
        let test;
        await mutex.acquire(() => {
            test = tests.pop();
        });
        const [assert, program] = await Promise.all([
            getSnapshot(test),
            compileProgram(test)
        ]);
        const result = await runProgram(program);
        if (assert !== result) {
            process.stdout.write("E");
            failedFiles.push(test);
        } else {
            process.stdout.write(".");
        }
    }
})).then(() => {
    if (failedFiles.length > 0) {
        console.log();
        console.log();
        console.log(failedFiles.map(f => "- " + f).join("\n"));
    }

    console.log();
    console.log();
    console.log(`Executed ${testCount} test, ${failedFiles.length} failed`);
    process.exit(failedFiles.length > 0 ? 1 : 0);
});

