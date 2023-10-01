import * as path from 'path'
import { ExtensionContext } from 'vscode'
import { LanguageClient, TransportKind } from 'vscode-languageclient/node'
import { SkSL } from './sksl-wasi'
import { CloseParams, Method, UpdateParams } from './sksl'

let client: LanguageClient | undefined

export async function activate(context: ExtensionContext) {
    const wasmPath = context.asAbsolutePath(path.join('build', 'sksl-wasi'))
    const sksl = await SkSL.create(wasmPath)
    sksl.run()

    const module = context.asAbsolutePath(path.join('build', 'server.js'))
    const transport = TransportKind.ipc
    client = new LanguageClient(
        'SkSL Language Server',
        {
            run: { module, transport },
            debug: {
                module,
                transport,
                runtime: 'node',
                options: { execArgv: ['--nolazy', '--inspect=6009'] },
            },
        },
        {
            documentSelector: [{ language: 'sksl' }],
        },
    )

    client.onRequest(Method.kUpdate, async (params: UpdateParams) => {
        return await sksl.request(Method.kUpdate, params)
    })

    client.onRequest(Method.kClose, async (params: CloseParams) => {
        return await sksl.request(Method.kClose, params)
    })

    await client.start()
}

export async function deactivate() {
    if (client) {
        await client.stop()
        client = undefined
    }
}
