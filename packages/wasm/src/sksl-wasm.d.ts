/// <reference types="emscripten" />

export interface SkSLWasm extends EmscriptenModule {
    _SetParamsSize(size: number): void
    _GetParamsPtr(): number
    _GetResultPtr(): number
    _GetResultSize(): number
    _ToUTFOffsets(): void
    _Update(): void
    _Close(): void
    _GetSymbol(): void
    _Format(): void
    _GetToken(): void
    _GetTokenRange(): void
    _Hover(): void
    _Definition(): void
    _Completion(): void
}

export default function createSkSLWasm(moduleOverrides?: Partial<EmscriptenModule>): Promise<SkSLWasm>
