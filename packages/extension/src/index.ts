import * as path from 'path'
import * as vscode from 'vscode'
import { LanguageClient, TransportKind } from 'vscode-languageclient/node'
import { Runner } from './runner'

let client: LanguageClient | undefined

export async function activate(context: vscode.ExtensionContext) {
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

    const runner = new Runner(context, client)
    context.subscriptions.push(
        vscode.commands.registerCommand(Runner.kCommand, (url) => {
            runner.run(url)
        }),
    )
}

export async function deactivate() {
    if (client) {
        await client.stop()
        client = undefined
    }
}
