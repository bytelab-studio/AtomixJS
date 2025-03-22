export interface Section {
    length: number;
}

export interface SectionBuilder<T extends Section> {
    build(): T;
}