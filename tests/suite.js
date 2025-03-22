const fs = require("fs");
const path = require("path");
const child_process = require("child_process");
const os = require("node:os");

const NODE_SUIT = path.join(__dirname, "utils", "node-suit.js");
const COMPILER = path.join(__dirname, "..", "atomixc", "dist", "bin", "atomixc.js");
const VM_RUNNER = os.platform() == "win32"
    ? path.join(__dirname, "..", "atomix", "cmake-build-debug", "atomix.exe")
    : path.join(__dirname, "..", "atomix", "cmake-build-debug", "atomix");

function* pipeFiles(dir) {
    const entries = fs.readdirSync(dir, {withFileTypes: true});

    for (const entry of entries) {
        const fullPath = path.join(dir, entry.name);

        if (entry.isDirectory()) {
            yield* pipeFiles(fullPath);
        } else if (fullPath.endsWith(".js")) {
            yield fullPath;
        }
    }
}

function compileProgram(test) {
    process.stdout.write(` - Compile program: ${test}: `);
    const outputFile = test + ".bin";
    const buildResult = child_process.spawnSync("node", [COMPILER, test, outputFile]);

    if (buildResult.status != 0) {
        console.log("Fail");
        console.log(buildResult.stderr);
    } else {
        console.log("Done");
    }

    return outputFile;
}

function runProgram(file) {
    process.stdout.write(` - Run test: `);
    const testResult = child_process.spawnSync(VM_RUNNER, [file], {
        encoding: "utf-8"
    });
    if (testResult.status != 0) {
        console.log("Fail");
        console.log(testResult.stderr);
    } else {
        console.log("Done");
    }
    return testResult.stdout;
}

let executed = 0;
let failed = 0;

for (const test of pipeFiles(path.join(__dirname, "src"))) {
    console.log(`Test: ${test}`);
    const snapshotFile = test + ".snp";
    let assert;
    if (!fs.existsSync(snapshotFile)) {
        process.stdout.write(" - No snapshot found, run nodejs suit: ");
        const subProcess = child_process.spawnSync("node", [NODE_SUIT, test], {
            encoding: "utf-8"
        });
        fs.writeFileSync(snapshotFile, subProcess.stdout);
        assert = subProcess.stdout;
        if (subProcess.status != 0) {
            console.log("Fail");
            console.log(subProcess.stderr);
        } else {
            console.log("Done");
        }
    } else {
        assert = fs.readFileSync(snapshotFile, "utf-8");
    }

    const program = compileProgram(test);
    const testContent = runProgram(program);
    executed++;
    if (assert != testContent) {
        failed++;
        console.log(" - Fail");
    } else {
        console.log(" - OK");
    }
}

console.log();
console.log();
console.log(`Executed ${executed} test, ${failed} failed`);
if (failed > 0) {
    process.exit(1);
}

process.exit(0);