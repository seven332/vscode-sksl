import * as ls from 'vscode-languageserver/node'
import * as lstd from 'vscode-languageserver-textdocument'
import { URI } from 'vscode-uri'
import { CloseParams, Method, ProgramKind, UpdateParams, UpdateResult, getProgramKind } from './sksl'
import { FilePosition } from './file-position'

const connection = ls.createConnection(ls.ProposedFeatures.all)
const documents = new ls.TextDocuments(lstd.TextDocument)
const files = new Map<string, Set<string>>()
const filePositions = new Map<string, FilePosition>()

connection.onInitialize(() => {
    return {
        capabilities: {
            textDocumentSync: ls.TextDocumentSyncKind.Incremental,
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
    const kind = getProgramKind(content)
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

connection.onRequest(ls.SemanticTokensRequest.method, (params: ls.SemanticTokensParams) => {
    params.textDocument.uri
    // TODO:
})

connection.onRequest(Method.kError, (error: string) => {
    console.log(`sksl-wasi-error: ${error}`)
})

documents.listen(connection)
connection.listen()

async function update(file: string, content: string, kind: ProgramKind): Promise<ls.Diagnostic[]> {
    const params: UpdateParams = { file, content, kind }
    const body: string = await connection.sendRequest(Method.kUpdate, JSON.stringify(params))
    const result: UpdateResult = JSON.parse(body)
    console.log(Method.kUpdate, body)

    const filePosition = new FilePosition(content)

    const diagnostics = result.errors.map((error) =>
        ls.Diagnostic.create(
            ls.Range.create(filePosition.getPosition(error.start), filePosition.getPosition(error.end)),
            error.msg,
            ls.DiagnosticSeverity.Error,
        ),
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
    console.log(Method.kClose, body)

    filePositions.delete(file)
}
