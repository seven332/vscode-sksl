import { uinteger } from 'vscode-languageclient'

export enum SkSLProgramKind {
    kFrag = 'frag',
    kVert = 'vert',
    kCompute = 'compute',
    kShader = 'shader',
    kColorFilter = 'colorfilter',
    kBlender = 'blender',
    kMeshVert = 'mesh-vert',
    kMeshFrag = 'mesh-frag',
}

export const getSkSLProgramKind = (content: string): SkSLProgramKind | undefined => {
    const regex = new RegExp(
        `^[ \\t]*\\/\\/[ /\\t]*kind[ \\t]*[:=][ \\t]*(${Object.values(SkSLProgramKind).join('|')})`,
    )
    const match = content.match(regex)
    if (match) {
        return match[1] as SkSLProgramKind
    }
}

export enum Url {
    kError = 'sksl/error',
    kUpdate = 'sksl/update',
    kClose = 'sksl/close',
    kGetSymbol = 'sksl/get-symbol',
    kFormat = 'sksl/format',
}

export interface SkSLRange {
    start: uinteger
    end: uinteger
}

export const dummySkSLRange: SkSLRange = {
    start: 0,
    end: 0,
}

export interface SkSLError {
    message: string
    range: SkSLRange
}

export const dummySkSLError: SkSLError = {
    message: '',
    range: dummySkSLRange,
}

export enum SkSLSymbolKind {
    kVariable = 'variable',
    kFunction = 'function',
    kField = 'field',
    kStruct = 'struct',
    kInterface = 'interface',
}

export interface SkSLSymbol {
    name: string
    detail: string
    kind: SkSLSymbolKind
    range: SkSLRange
    selectionRange: SkSLRange
    children: SkSLSymbol[]
}

export const dummySkSLSymbol: SkSLSymbol = (() => {
    const dummy: SkSLSymbol = {
        name: '',
        detail: '',
        kind: SkSLSymbolKind.kVariable,
        range: dummySkSLRange,
        selectionRange: dummySkSLRange,
        children: [],
    }
    dummy.children = [dummy]
    return dummy
})()

export interface UpdateParams {
    file: string
    content: string
    kind: SkSLProgramKind
}

export interface UpdateResult {
    succeed: boolean
    errors: SkSLError[]
}

export const dummyUpdateResult: UpdateResult = {
    succeed: false,
    errors: [dummySkSLError],
}

export interface CloseParams {
    file: string
}

export interface CloseResult {
    succeed: boolean
}

export const dummyCloseResult: CloseResult = {
    succeed: false,
}

export interface GetSymbolParams {
    file: string
}

export interface GetSymbolResult {
    symbols: SkSLSymbol[]
}

export const dummyGetSymbolResult: GetSymbolResult = {
    symbols: [dummySkSLSymbol],
}

export interface FormatParams {
    file: string
}

export interface FormatResult {
    newContent: string
}

export const dummyFormatResult: FormatResult = {
    newContent: '',
}
