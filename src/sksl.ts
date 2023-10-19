import { uinteger } from 'vscode-languageclient'

export enum SkSLProgramKind {
    kShader = 'shader',
    kColorFilter = 'colorfilter',
    kBlender = 'blender',
    kMeshFrag = 'meshfrag',
    kMeshVert = 'meshvert',
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
    kGetToken = 'sksl/get-token',
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

export interface SkSLToken {
    range: SkSLRange
    tokenType: uinteger
    tokenModifiers: uinteger
}

export const dummySkSLToken: SkSLToken = {
    range: dummySkSLRange,
    tokenType: 0,
    tokenModifiers: 0,
}

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

export interface GetTokenParams {
    file: string
}

export interface GetTokenResult {
    tokens: SkSLToken[]
}

export const dummyGetTokenResult: GetTokenResult = {
    tokens: [dummySkSLToken],
}
