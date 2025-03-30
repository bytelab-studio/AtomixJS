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
}

export {}