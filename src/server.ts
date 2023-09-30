import * as ls from 'vscode-languageserver/node'
import * as lstd from 'vscode-languageserver-textdocument'

const connection = ls.createConnection(ls.ProposedFeatures.all)
const documents = new ls.TextDocuments(lstd.TextDocument)

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

connection.onRequest(ls.SemanticTokensRequest.method, (params: ls.SemanticTokensParams) => {
    // TODO:
})

documents.listen(connection)
connection.listen()
