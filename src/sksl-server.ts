import { FilePosition } from './file-position'
import { URI } from 'vscode-uri'
import createSkSLWasm, { SkSLWasm } from './sksl-wasm'
import * as ls from 'vscode-languageserver/node'
import { decode, encode } from './simple-codec'
import { uinteger } from 'vscode-languageclient'

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

        const filePosition = new FilePosition(content)

        if (result.succeed) {
            // Set new file position
            this.filePositions.set(file, filePosition)
        } else {
            // Delete old file position
            this.filePositions.delete(file)
        }

        // Return error
        return result.errors.map((error) =>
            ls.Diagnostic.create(toRange(filePosition, error.range), error.message, ls.DiagnosticSeverity.Error),
        )
    }

    public close(uri: string) {
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

        if (!this.filePositions.has(file)) {
            // This file is not recognized
            return
        }

        // Call wasm
        this.setParams<CloseParams>({ file })
        this.wasm._Close()

        // Delete old file position
        this.filePositions.delete(file)
    }

    public getSymbol(uri: string): ls.DocumentSymbol[] {
        const file = URI.parse(uri).fsPath
        const filePosition = this.filePositions.get(file)
        if (!filePosition) {
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
                toRange(filePosition, symbol.range),
                toRange(filePosition, symbol.selectionRange),
                symbol.children.map((child) => toSymbol(child)),
            )

        return result.symbols.map((symbol) => toSymbol(symbol))
    }

    public format(uri: string): ls.TextEdit[] {
        const file = URI.parse(uri).fsPath
        const filePosition = this.filePositions.get(file)
        if (!filePosition) {
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
                ls.Range.create(ls.Position.create(0, 0), ls.Position.create(filePosition.getLines(), 0)),
                result.newContent,
            ),
        ]
    }

    public getToken(uri: string): ls.SemanticTokens {
        const builder = new ls.SemanticTokensBuilder()

        const file = URI.parse(uri).fsPath
        const filePosition = this.filePositions.get(file)
        if (!filePosition) {
            return builder.build()
        }

        // Call wasm
        this.setParams<GetTokenParams>({ file })
        this.wasm._GetToken()
        const result = this.getResult<GetTokenResult>(dummyGetTokenResult)

        for (const token of result.tokens) {
            const range = toRange(filePosition, token.range)
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
        const filePosition = this.filePositions.get(file)
        if (!filePosition) {
            return builder.build()
        }

        // Call wasm
        this.setParams<GetTokenRangeParams>({ file, range: toSkSLRange(filePosition, range) })
        this.wasm._GetTokenRange()
        const result = this.getResult<GetTokenRangeResult>(dummyGetTokenRangeResult)

        for (const token of result.tokens) {
            const range = toRange(filePosition, token.range)
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
        const filePosition = this.filePositions.get(file)
        if (!filePosition) {
            return
        }

        // Call wasm
        this.setParams<HoverParams>({ file, position: filePosition.getOffset(position) })
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
            range: toRange(filePosition, result.range),
        }
    }

    public definition(uri: string, position: ls.Position): ls.Location | undefined {
        const file = URI.parse(uri).fsPath
        const filePosition = this.filePositions.get(file)
        if (!filePosition) {
            return
        }

        // Call wasm
        this.setParams<DefinitionParams>({ file, position: filePosition.getOffset(position) })
        this.wasm._Definition()
        const result = this.getResult<DefinitionResult>(dummyDefinitionResult)

        if (!result.found) {
            return
        }

        return ls.Location.create(uri, toRange(filePosition, result.range))
    }

    private files = new Map<string, Set<string>>()
    private filePositions = new Map<string, FilePosition>()

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
}

interface SkSLRange {
    start: uinteger
    end: uinteger
}

const dummySkSLRange: SkSLRange = {
    start: 0,
    end: 0,
}

interface SkSLError {
    message: string
    range: SkSLRange
}

const dummySkSLError: SkSLError = {
    message: '',
    range: dummySkSLRange,
}

enum SkSLSymbolKind {
    kVariable = 'variable',
    kFunction = 'function',
    kField = 'field',
    kStruct = 'struct',
    kInterface = 'interface',
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
    tokenType: uinteger
    tokenModifiers: uinteger
}

const dummySkSLToken: SkSLToken = {
    range: dummySkSLRange,
    tokenType: 0,
    tokenModifiers: 0,
}

interface UpdateParams {
    file: string
    content: string
}

interface UpdateResult {
    succeed: boolean
    errors: SkSLError[]
}

const dummyUpdateResult: UpdateResult = {
    succeed: false,
    errors: [dummySkSLError],
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

function toRange(filePosition: FilePosition, range: SkSLRange): ls.Range {
    return ls.Range.create(filePosition.getPosition(range.start), filePosition.getPosition(range.end))
}

function toSkSLRange(filePosition: FilePosition, range: ls.Range): SkSLRange {
    return { start: filePosition.getOffset(range.start), end: filePosition.getOffset(range.end) }
}
