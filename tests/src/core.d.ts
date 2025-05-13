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
        getExportObj(): any;
    }

    declare const module: Module;

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