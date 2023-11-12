import * as ls from 'vscode-languageserver/node'
import * as lstd from 'vscode-languageserver-textdocument'
import { SkSLServer } from './sksl-server'

const connection = ls.createConnection(ls.ProposedFeatures.all)
const documents = new ls.TextDocuments(lstd.TextDocument)
let server: SkSLServer | undefined = undefined

connection.onInitialize(async (params) => {
    server = await SkSLServer.create(params.initializationOptions.skslWasmPath)
    return {
        capabilities: {
            textDocumentSync: ls.TextDocumentSyncKind.Incremental,
            documentSymbolProvider: true,
            documentFormattingProvider: true,
            hoverProvider: true,
            definitionProvider: true,
            semanticTokensProvider: {
                legend: {
                    tokenTypes: [
                        'class',
                        'interface',
                        'struct',
                        'parameter',
                        'variable',
                        'property',
                        'decorator',
                        'function',
                        'number',
                    ],
                    tokenModifiers: ['readonly', 'defaultLibrary'],
                },
                range: true,
                full: true,
            },
            completionProvider: {
                triggerCharacters: [':', '='],
            },
        },
    }
})

documents.onDidChangeContent((event) => {
    const diagnostics = server?.update(event.document.uri, event.document.getText())
    if (diagnostics) {
        connection.sendDiagnostics({
            uri: event.document.uri,
            diagnostics,
        })
    }
})

documents.onDidClose((event) => {
    server?.close(event.document.uri)
})

connection.onDocumentSymbol((params) => {
    return server?.getSymbol(params.textDocument.uri)
})

connection.onDocumentFormatting((params) => {
    return server?.format(params.textDocument.uri)
})

connection.onRequest(ls.SemanticTokensRequest.method, (params: ls.SemanticTokensParams) => {
    return server?.getToken(params.textDocument.uri) ?? { data: [] }
})

connection.onRequest(ls.SemanticTokensRangeRequest.method, (params: ls.SemanticTokensRangeParams) => {
    return server?.getTokenRange(params.textDocument.uri, params.range) ?? { data: [] }
})

connection.onHover((params) => {
    return server?.hover(params.textDocument.uri, params.position)
})

connection.onDefinition((params) => {
    return server?.definition(params.textDocument.uri, params.position)
})

connection.onCompletion((params) => {
    return server?.completion(params.textDocument.uri, params.position)
})

documents.listen(connection)
connection.listen()
