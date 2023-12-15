import { URI } from 'vscode-uri'
import createSkSLWasm, { SkSLWasm } from '@workspace/wasm'
import * as ls from 'vscode-languageserver/node'
import { FilePosition } from './file-position'
import { UTFOffset, dummyUTFOffset } from './utf-offset-converter'
import { decode, encode } from '@workspace/util'
import { QueryParams, QueryResult, dummyQueryResult } from './runner-data'

export const kTokenTypes = [
    'class',
    'interface',
    'struct',
    'parameter',
    'variable',
    'property',
    'decorator',
    'function',
    'number',
]

export const kTokenModifiers = ['readonly', 'defaultLibrary']

export const kTriggerCharacters = [':', '=']

export class SkSLServer {
    public static async create(path: string): Promise<SkSLServer> {
        return new SkSLServer(await createSkSLWasm({ locateFile: () => path }))
    }

    public update(uri: string, content: string): ls.Diagnostic[] {
        const file = URI.parse(uri).fsPath
        const uris = this.files.get(file) || new Set()
        uris.add(uri)
        this.files.set(file, uris)

        // Call wasm
        this.setParams<UpdateParams>({ file, content })
        this.wasm._Update()
        const result = this.getResult<UpdateResult>(dummyUpdateResult)

        const filePosition = this.toFilePosition(content)
        this.documents.set(file, { recognized: result.succeed, filePosition })

        // Return diagnostics
        return result.diagnostics.map((diagnostic) =>
            ls.Diagnostic.create(toRange(filePosition, diagnostic.range), diagnostic.message, diagnostic.severity),
        )
    }

    public close(uri: string): ls.Diagnostic[] | undefined {
        const file = URI.parse(uri).fsPath
        const uris = this.files.get(file)
        if (uris) {
            uris.delete(uri)
            if (!uris.size) {
                this.files.delete(file)
            }
        }

        if (this.files.has(file)) {
            // This file is still open
            return
        }

        const document = this.documents.get(file)
        if (document) {
            // Call wasm
            this.setParams<CloseParams>({ file })
            this.wasm._Close()

            // Delete document
            this.documents.delete(file)
        }
        return []
    }

    public getSymbol(uri: string): ls.DocumentSymbol[] {
        const file = URI.parse(uri).fsPath
        const document = this.documents.get(file)
        if (!document || !document.recognized) {
            return []
        }

        // Call wasm
        this.setParams<GetSymbolParams>({ file })
        this.wasm._GetSymbol()
        const result = this.getResult<GetSymbolResult>(dummyGetSymbolResult)

        const toKind = (kind: SkSLSymbolKind): ls.SymbolKind => {
            switch (kind) {
                default:
                case SkSLSymbolKind.kVariable:
                    return ls.SymbolKind.Variable
                case SkSLSymbolKind.kFunction:
                    return ls.SymbolKind.Function
                case SkSLSymbolKind.kField:
                    return ls.SymbolKind.Field
                case SkSLSymbolKind.kStruct:
                    return ls.SymbolKind.Struct
                case SkSLSymbolKind.kInterface:
                    return ls.SymbolKind.Interface
            }
        }

        const toSymbol = (symbol: SkSLSymbol): ls.DocumentSymbol =>
            ls.DocumentSymbol.create(
                symbol.name,
                symbol.detail,
                toKind(symbol.kind),
                toRange(document.filePosition, symbol.range),
                toRange(document.filePosition, symbol.selectionRange),
                symbol.children.map((child) => toSymbol(child)),
            )

        return result.symbols.map((symbol) => toSymbol(symbol))
    }

    public format(uri: string): ls.TextEdit[] {
        const file = URI.parse(uri).fsPath
        const document = this.documents.get(file)
        if (!document || !document.recognized) {
            return []
        }

        // Call wasm
        this.setParams<FormatParams>({ file })
        this.wasm._Format()
        const result = this.getResult<FormatResult>(dummyFormatResult)

        if (result.newContent.length == 0) {
            return []
        }

        return [
            ls.TextEdit.replace(
                ls.Range.create(ls.Position.create(0, 0), ls.Position.create(document.filePosition.getLines(), 0)),
                result.newContent,
            ),
        ]
    }

    public getToken(uri: string): ls.SemanticTokens {
        const builder = new ls.SemanticTokensBuilder()

        const file = URI.parse(uri).fsPath
        const document = this.documents.get(file)
        if (!document || !document.recognized) {
            return builder.build()
        }

        // Call wasm
        this.setParams<GetTokenParams>({ file })
        this.wasm._GetToken()
        const result = this.getResult<GetTokenResult>(dummyGetTokenResult)

        for (const token of result.tokens) {
            const range = toRange(document.filePosition, token.range)
            if (range.start.line == range.end.line) {
                builder.push(
                    range.start.line,
                    range.start.character,
                    range.end.character - range.start.character,
                    token.tokenType,
                    token.tokenModifiers,
                )
            }
        }

        return builder.build()
    }

    public getTokenRange(uri: string, range: ls.Range): ls.SemanticTokens {
        const builder = new ls.SemanticTokensBuilder()

        const file = URI.parse(uri).fsPath
        const document = this.documents.get(file)
        if (!document || !document.recognized) {
            return builder.build()
        }

        // Call wasm
        this.setParams<GetTokenRangeParams>({ file, range: toSkSLRange(document.filePosition, range) })
        this.wasm._GetTokenRange()
        const result = this.getResult<GetTokenRangeResult>(dummyGetTokenRangeResult)

        for (const token of result.tokens) {
            const range = toRange(document.filePosition, token.range)
            if (range.start.line == range.end.line) {
                builder.push(
                    range.start.line,
                    range.start.character,
                    range.end.character - range.start.character,
                    token.tokenType,
                    token.tokenModifiers,
                )
            }
        }

        return builder.build()
    }

    public hover(uri: string, position: ls.Position): ls.Hover | undefined {
        const file = URI.parse(uri).fsPath
        const document = this.documents.get(file)
        if (!document || !document.recognized) {
            return
        }

        // Call wasm
        this.setParams<HoverParams>({ file, position: document.filePosition.toOffset(position) })
        this.wasm._Hover()
        const result = this.getResult<HoverResult>(dummyHoverResult)

        if (!result.found) {
            return
        }

        return {
            contents: {
                kind: result.markdown ? ls.MarkupKind.Markdown : ls.MarkupKind.PlainText,
                value: result.content,
            },
            range: toRange(document.filePosition, result.range),
        }
    }

    public definition(uri: string, position: ls.Position): ls.Location | undefined {
        const file = URI.parse(uri).fsPath
        const document = this.documents.get(file)
        if (!document || !document.recognized) {
            return
        }

        // Call wasm
        this.setParams<DefinitionParams>({ file, position: document.filePosition.toOffset(position) })
        this.wasm._Definition()
        const result = this.getResult<DefinitionResult>(dummyDefinitionResult)

        if (!result.found) {
            return
        }

        return ls.Location.create(uri, toRange(document.filePosition, result.range))
    }

    public completion(uri: string, position: ls.Position): ls.CompletionItem[] {
        const file = URI.parse(uri).fsPath
        const document = this.documents.get(file)
        if (!document) {
            return []
        }

        // Call wasm
        this.setParams<CompletionParams>({ file, position: document.filePosition.toOffset(position) })
        this.wasm._Completion()
        const result = this.getResult<CompletionResult>(dummyCompletionResult)

        return result.items
    }

    // Runner

    public query(params: QueryParams): QueryResult {
        this.setParams<QueryParams>(params)
        this.wasm._Query()
        return this.getResult<QueryResult>(dummyQueryResult)
    }

    private files = new Map<string, Set<string>>()
    private documents = new Map<string, Document>()

    private constructor(private wasm: SkSLWasm) {}

    private setParams<T>(t: T) {
        const bytes = encode(t)
        this.wasm._SetParamsSize(bytes.byteLength)
        this.wasm.HEAPU8.set(bytes, this.wasm._GetParamsPtr())
    }

    private getResult<T>(dummy: T): T {
        const ptr = this.wasm._GetResultPtr()
        const size = this.wasm._GetResultSize()
        const bytes = this.wasm.HEAPU8.subarray(ptr, ptr + size)
        return decode(bytes, dummy)
    }

    private toFilePosition(content: string): FilePosition {
        this.setParams<string>(content)
        this.wasm._ToUTFOffsets()
        const offsets = this.getResult<UTFOffset[]>([dummyUTFOffset])
        return new FilePosition(content, offsets)
    }
}

interface Document {
    recognized: boolean
    filePosition: FilePosition
}

interface SkSLRange {
    start: ls.uinteger
    end: ls.uinteger
}

const dummySkSLRange: SkSLRange = {
    start: 0,
    end: 0,
}

interface SkSLDiagnostic {
    message: string
    range: SkSLRange
    severity: ls.DiagnosticSeverity
}

const dummySkSLDiagnostic: SkSLDiagnostic = {
    message: '',
    range: dummySkSLRange,
    severity: 1,
}

enum SkSLSymbolKind {
    kVariable = 0,
    kFunction = 1,
    kField = 2,
    kStruct = 3,
    kInterface = 4,
}

interface SkSLSymbol {
    name: string
    detail: string
    kind: SkSLSymbolKind
    range: SkSLRange
    selectionRange: SkSLRange
    children: SkSLSymbol[]
}

const dummySkSLSymbol: SkSLSymbol = (() => {
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

interface SkSLToken {
    range: SkSLRange
    tokenType: ls.uinteger
    tokenModifiers: ls.uinteger
}

const dummySkSLToken: SkSLToken = {
    range: dummySkSLRange,
    tokenType: 0,
    tokenModifiers: 0,
}

interface SkSLCompletion {
    label: string
    kind: ls.CompletionItemKind
}

const dummySkSLCompletion: SkSLCompletion = {
    label: '',
    kind: 1,
}

interface UpdateParams {
    file: string
    content: string
}

interface UpdateResult {
    succeed: boolean
    diagnostics: SkSLDiagnostic[]
}

const dummyUpdateResult: UpdateResult = {
    succeed: false,
    diagnostics: [dummySkSLDiagnostic],
}

interface CloseParams {
    file: string
}

interface GetSymbolParams {
    file: string
}

interface GetSymbolResult {
    symbols: SkSLSymbol[]
}

const dummyGetSymbolResult: GetSymbolResult = {
    symbols: [dummySkSLSymbol],
}

interface FormatParams {
    file: string
}

interface FormatResult {
    newContent: string
}

const dummyFormatResult: FormatResult = {
    newContent: '',
}

interface GetTokenParams {
    file: string
}

interface GetTokenResult {
    tokens: SkSLToken[]
}

const dummyGetTokenResult: GetTokenResult = {
    tokens: [dummySkSLToken],
}

interface GetTokenRangeParams {
    file: string
    range: SkSLRange
}

type GetTokenRangeResult = GetTokenResult

const dummyGetTokenRangeResult = dummyGetTokenResult

interface HoverParams {
    file: string
    position: number
}

interface HoverResult {
    found: boolean
    markdown: boolean
    content: string
    range: SkSLRange
}

const dummyHoverResult: HoverResult = {
    found: false,
    markdown: false,
    content: '',
    range: dummySkSLRange,
}

interface DefinitionParams {
    file: string
    position: number
}

interface DefinitionResult {
    found: boolean
    range: SkSLRange
}

const dummyDefinitionResult: DefinitionResult = {
    found: false,
    range: dummySkSLRange,
}

interface CompletionParams {
    file: string
    position: number
}

interface CompletionResult {
    items: SkSLCompletion[]
}

const dummyCompletionResult: CompletionResult = {
    items: [dummySkSLCompletion],
}

function toRange(filePosition: FilePosition, range: SkSLRange): ls.Range {
    return ls.Range.create(filePosition.toPosition(range.start), filePosition.toPosition(range.end))
}

function toSkSLRange(filePosition: FilePosition, range: ls.Range): SkSLRange {
    return { start: filePosition.toOffset(range.start), end: filePosition.toOffset(range.end) }
}
