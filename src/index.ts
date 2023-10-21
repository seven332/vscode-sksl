import * as path from 'path'
import { ExtensionContext } from 'vscode'
import { LanguageClient, TransportKind } from 'vscode-languageclient/node'

let client: LanguageClient | undefined

export async function activate(context: ExtensionContext) {
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
