import * as parser from "../parser";

if (process.argv.length < 4) {
    throw "Missing arguments";
}

const input: string = process.argv[2];
const output: string = process.argv[3];

parser.parseFile(input, output);