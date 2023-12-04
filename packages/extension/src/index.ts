import * as path from 'path'
import * as vscode from 'vscode'
import { LanguageClient, TransportKind } from 'vscode-languageclient/node'
import * as fs from 'fs'

let client: LanguageClient | undefined

export async function activate(context: vscode.ExtensionContext) {
    context.subscriptions.push(vscode.commands.registerCommand('sksl.showRunner', () => showRunner(context)))

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

function showRunner(context: vscode.ExtensionContext) {
    const panel = vscode.window.createWebviewPanel('sksl.runner', 'SkSL Runner', vscode.ViewColumn.Beside, {
        enableScripts: true,
    })
    const htmlPath = context.asAbsolutePath(path.join('build', 'runner', 'index.html'))
    panel.webview.html = fs.readFileSync(htmlPath).toString()
}
