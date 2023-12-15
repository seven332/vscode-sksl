import * as path from 'path'
import * as vscode from 'vscode'
import { LanguageClient, TransportKind } from 'vscode-languageclient/node'
import * as fs from 'fs'
import { kQueryUrl } from './runner-data'
import { QueryResult } from '@workspace/lsp'

let client: LanguageClient | undefined

export async function activate(context: vscode.ExtensionContext) {
    context.subscriptions.push(vscode.commands.registerCommand('sksl.showRunner', (uri) => showRunner(context, uri)))

    const module = context.asAbsolutePath(path.join('build', 'server.js'))
    const skslWasmPath = context.asAbsolutePath(path.join('build', 'sksl-wasm.wasm'))
    const transport = TransportKind.ipc
    client = new LanguageClient(
        'SkSL Language Server',
        {
            run: { module, transport },
            debug: {
                module,
                transport,
                options: { execArgv: ['--nolazy', '--inspect=6009'] },
            },
        },
        {
            documentSelector: [{ language: 'sksl' }],
            initializationOptions: { skslWasmPath },
        },
    )
    await client.start()
}

export async function deactivate() {
    if (client) {
        await client.stop()
        client = undefined
    }
}

function showRunner(context: vscode.ExtensionContext, uri: vscode.Uri | undefined) {
    const panel = vscode.window.createWebviewPanel('sksl.runner', 'SkSL Runner', vscode.ViewColumn.Beside, {
        enableScripts: true,
    })

    const htmlPath = context.asAbsolutePath(path.join('build', 'runner', 'index.html'))
    panel.webview.html = fs.readFileSync(htmlPath).toString()

    if (uri) {
        selectSkSL(panel, uri)
    }

    panel.webview.onDidReceiveMessage(
        async () => {
            const uris = await vscode.window.showOpenDialog({
                canSelectFiles: true,
                canSelectFolders: false,
                canSelectMany: false,
                filters: {
                    SkSL: ['sksl'],
                },
            })
            if (uris && uris.length >= 1) {
                selectSkSL(panel, uris[0])
            }
        },
        undefined,
        context.subscriptions,
    )
}

async function selectSkSL(panel: vscode.WebviewPanel, uri: vscode.Uri) {
    panel.webview.postMessage(uri.toString())
    const buffer = fs.readFileSync(uri.fsPath)
    const result: QueryResult | undefined = await client?.sendRequest(kQueryUrl, {
        source: buffer.toString(),
    })
    console.log(result)
}
