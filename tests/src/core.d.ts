declare global {
    /**
     * Prints it parameters to the screen
     * @param x
     */
    function print(...x: any[]): void;

    /**
     * Holds information about the current module
     */
    interface Module {
        /**
         * Returns the internal VM export object of the current module
         */
        getExportObj(): object;

        /**
         * Imports a module by its 64Bit hash. Requires to be integers
         * @param hash1 - first 32Bit of the hash
         * @param hash2 - second 32Bit of the hash
         */
        importModule<T extends object = object>(hash1: number, hash2: number): T;
    }

    declare const Module: Module;

    interface ObjectPrototype {
        readonly prototype: ObjectPrototype;
    }

    interface Object {
        /**
         * Instantiate a new object of the given constructor.
         * @param constructor - The constructor of the class to instantiate.
         * @param args - The parameters passed to the constructor function.
         */
        instantiate<T extends object>(constructor: Function, ...args: any[]): T;

        /**
         * Creates a new object with the Object prototype as parent.
         */
        create(): object;

        /**
         * Creates a new object with the given prototype as parent.
         * @param prototype - The prototype of the new object.
         */
        create(prototype: object): object;
        
        readonly prototype: ObjectPrototype;
    }

    declare const Object: Object;

    interface Array<T> {
        new <T>(length: number): Array<T>;
        new <T>(...items: T[]): Array<T>;
        <T>(length: number): Array<T>;
        <T>(...items: T[]): Array<T>;
        readonly length: number;
        [n: number]: T;
        isArray(x: any): boolean;
    }

    declare const Array: Array;

    interface FunctionPrototype {
        call(thisValue: any, ...args: any[]);

        readonly prototype: ObjectPrototype;
    }

    interface Function {
        /**
         * Function constructor cannot be implemented
         * because native JavaScript parsing cannot be fulfilled.
         */
        (): never;
        /**
         * Function constructor cannot be implemented
         * because native JavaScript parsing cannot be fulfilled.
         */
        new (): never;

        readonly prototype: FunctionPrototype;
    }

    declare const Function: Function;
}

export {}