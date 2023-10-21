import { FilePosition } from './file-position'
import { URI } from 'vscode-uri'
import createSkSLWasm, { SkSLWasm } from './sksl-wasm'
import { SkSLProgramKind, getSkSLProgramKind } from './sksl'
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

        const kind = getSkSLProgramKind(content)

        if (kind) {
            // This file is recognized

            // Call wasm
            this.setParams<UpdateParams>({ file, content, kind })
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

        if (this.filePositions.has(file)) {
            // Can't recognize the file

            // Call wasm
            this.setParams<CloseParams>({ file })
            this.wasm._Close()

            // Delete old file position
            this.filePositions.delete(file)

            return []
        }

        return []
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
    kind: SkSLProgramKind
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

function toRange(filePosition: FilePosition, range: SkSLRange): ls.Range {
    return ls.Range.create(filePosition.getPosition(range.start), filePosition.getPosition(range.end))
}
