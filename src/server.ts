import * as ls from 'vscode-languageserver/node'
import * as lstd from 'vscode-languageserver-textdocument'
import { URI } from 'vscode-uri'
import {
    CloseParams,
    GetSymbolParams,
    GetSymbolResult,
    Method,
    SkSLProgramKind,
    SkSLRange,
    SkSLSymbolKind,
    UpdateParams,
    UpdateResult,
    getSkSLProgramKind,
} from './sksl'
import { FilePosition } from './file-position'

const connection = ls.createConnection(ls.ProposedFeatures.all)
const documents = new ls.TextDocuments(lstd.TextDocument)
const files = new Map<string, Set<string>>()
const filePositions = new Map<string, FilePosition>()

connection.onInitialize(() => {
    return {
        capabilities: {
            textDocumentSync: ls.TextDocumentSyncKind.Incremental,
            documentSymbolProvider: true,
            semanticTokensProvider: {
                legend: {
                    tokenTypes: [],
                    tokenModifiers: [],
                },
                range: false,
                full: true,
            },
        },
    }
})

documents.onDidChangeContent(async (event) => {
    const uri = event.document.uri
    const file = URI.parse(uri).fsPath
    const uris = files.get(file) || new Set()
    uris.add(uri)
    files.set(file, uris)
    const content = event.document.getText()
    const kind = getSkSLProgramKind(content)
    if (kind) {
        const diagnostics = await update(file, content, kind)
        connection.sendDiagnostics({
            uri: uri,
            diagnostics: diagnostics,
        })
    } else {
        await close(file)
        connection.sendDiagnostics({
            uri: uri,
            diagnostics: [],
        })
    }
})

documents.onDidClose(async (event) => {
    const uri = event.document.uri
    const file = URI.parse(uri).fsPath
    const uris = files.get(file)
    if (uris) {
        uris.delete(uri)
        if (!uris.size) {
            files.delete(file)
        }
    }
    if (!files.has(file)) {
        await close(file)
        connection.sendDiagnostics({
            uri: uri,
            diagnostics: [],
        })
    }
})

connection.onDocumentSymbol((params) => {
    const uri = params.textDocument.uri
    const file = URI.parse(uri).fsPath
    return getDocumentSymbol(file)
})

connection.onRequest(ls.SemanticTokensRequest.method, (params: ls.SemanticTokensParams) => {
    params.textDocument.uri
    // TODO:
})

connection.onRequest(Method.kError, (error: string) => {
    console.log(`sksl-wasi-error: ${error}`)
})

documents.listen(connection)
connection.listen()

function toRange(filePosition: FilePosition, range: SkSLRange): ls.Range {
    return ls.Range.create(filePosition.getPosition(range.start), filePosition.getPosition(range.end))
}

async function update(file: string, content: string, kind: SkSLProgramKind): Promise<ls.Diagnostic[]> {
    const params: UpdateParams = { file, content, kind }
    const body: string = await connection.sendRequest(Method.kUpdate, JSON.stringify(params))
    const result: UpdateResult = JSON.parse(body)
    console.log(Method.kUpdate, file, body)

    const filePosition = new FilePosition(content)

    const diagnostics = result.errors.map((error) =>
        ls.Diagnostic.create(toRange(filePosition, error.range), error.message, ls.DiagnosticSeverity.Error),
    )

    if (result.succeed) {
        filePositions.set(file, filePosition)
    }

    return diagnostics
}

async function close(file: string) {
    if (!filePositions.has(file)) {
        return
    }

    const params: CloseParams = { file }
    const body: string = await connection.sendRequest(Method.kClose, JSON.stringify(params))
    console.log(Method.kClose, file, body)

    filePositions.delete(file)
}

async function getDocumentSymbol(file: string): Promise<ls.DocumentSymbol[]> {
    const filePosition = filePositions.get(file)
    if (!filePosition) {
        return []
    }

    const params: GetSymbolParams = { file }
    const body: string = await connection.sendRequest(Method.kGetSymbol, JSON.stringify(params))
    const result: GetSymbolResult = JSON.parse(body)
    console.log(Method.kGetSymbol, body)

    const toKind = (kind: SkSLSymbolKind): ls.SymbolKind => {
        switch (kind) {
            default:
            case SkSLSymbolKind.kExternal:
                return ls.SymbolKind.Module
            case SkSLSymbolKind.kField:
                return ls.SymbolKind.Field
            case SkSLSymbolKind.kFunction:
                return ls.SymbolKind.Function
            case SkSLSymbolKind.kStruct:
                return ls.SymbolKind.Struct
            case SkSLSymbolKind.kVariable:
                return ls.SymbolKind.Variable
            case SkSLSymbolKind.kInterface:
                return ls.SymbolKind.Interface
        }
    }

    return result.symbols.map((symbol) =>
        ls.DocumentSymbol.create(
            symbol.name,
            undefined,
            toKind(symbol.kind),
            toRange(filePosition, symbol.range),
            toRange(filePosition, symbol.selectionRange),
        ),
    )
}
