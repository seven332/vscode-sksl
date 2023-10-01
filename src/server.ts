import * as ls from 'vscode-languageserver/node'
import * as lstd from 'vscode-languageserver-textdocument'
import { URI } from 'vscode-uri'
import { Method, ProgramKind, getProgramKind } from './sksl'

const connection = ls.createConnection(ls.ProposedFeatures.all)
const documents = new ls.TextDocuments(lstd.TextDocument)
const files = new Map<string, Set<string>>()
const sources = new Set<string>()

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
        await update(file, content, kind)
        // connection.sendDiagnostics({
        //   uri: uri,
        //   diagnostics: getDiagnostics(file),
        // })
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

documents.listen(connection)
connection.listen()

async function update(file: string, content: string, kind: ProgramKind) {
    const result: string = await connection.sendRequest(Method.kUpdate, { file, content, kind })
    console.log(result)
    sources.add(file)
}

async function close(file: string) {
    if (sources.has(file)) {
        const result: string = await connection.sendRequest(Method.kClose, { file })
        console.log(result)
        sources.delete(file)
    }
}
