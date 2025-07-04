#!/usr/bin/env node

const child_process = require("child_process");
const path = require("path");
const fs = require("fs");

const COMPILER = path.join(__dirname, "..", "atomixc", "dist", "bin", "atomixc.js");
const RUNNER = path.join(__dirname, "..", "atomix", ".atomix", "bin", "Debug", "linux-x86_64", "runner");

const tmpDir = path.join(__dirname, "tmp");

const inputFile = process.argv[2];

if (!inputFile) {
    console.error("No input file provided");
    process.exit(1);
}

if (!fs.existsSync(tmpDir)) {
    fs.mkdirSync(tmpDir);
}

const baseName = path.basename(inputFile, ".js");
const inputDir = path.dirname(inputFile);
const bytecodeFile = path.join(tmpDir, `${baseName}-${Date.now()}.bin`);

const compile = child_process.spawnSync("node", [COMPILER, "compiler", "compile", inputFile, "-o", bytecodeFile, "-r", inputDir], {
    encoding: "utf-8"
});

if (compile.status != 0) {
    console.error("Bytecode compilation failed");
    console.error(compile.stdout);
    console.error(compile.stderr);
    process.exit(1);    
}

const run = child_process.spawnSync(RUNNER, [bytecodeFile], {
    encoding: "utf-8"
});

//fs.unlinkSync(bytecodeFile);

process.stdout.write(run.stdout);
process.stderr.write(run.stderr);
process.exit(run.status);

