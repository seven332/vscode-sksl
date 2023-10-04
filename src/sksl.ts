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
    const regex = new RegExp(`[ \\t]*\\/\\/[ /\\t]*kind[ \\t]*[:=][ \\t]*(${Object.values(SkSLProgramKind).join('|')})`)
    const match = content.match(regex)
    if (match) {
        return match[1] as SkSLProgramKind
    }
}

export enum Method {
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

export interface SkSLError {
    message: string
    range: SkSLRange
}

export enum SkSLSymbolKind {
    kExternal = 'external',
    kField = 'field',
    kFunction = 'function',
    kStruct = 'struct',
    kVariable = 'variable',
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

export interface UpdateParams {
    file: string
    content: string
    kind: SkSLProgramKind
}

export interface UpdateResult {
    succeed: boolean
    errors: SkSLError[]
}

export interface CloseParams {
    file: string
}

export interface CloseResult {
    succeed: boolean
}

export interface GetSymbolParams {
    file: string
}

export interface GetSymbolResult {
    symbols: SkSLSymbol[]
}

export interface FormatParams {
    file: string
}

export interface FormatResult {
    newContent: string
}
