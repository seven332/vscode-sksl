import * as path from 'path'
import { ExtensionContext } from 'vscode'
import { LanguageClient, TransportKind } from 'vscode-languageclient/node'
import { SkSL } from './sksl-wasi'
import {
    Url,
    dummyCloseResult,
    dummyFormatResult,
    dummyGetSymbolResult,
    dummyGetTokenResult,
    dummyUpdateResult,
} from './sksl'
import { decode, encode } from './simple-codec'

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

    async function onRequest<Params, Result>(url: string, dummyResult: Result) {
        client!.onRequest(url, async (params: Params) => {
            const buffer = await sksl.request(url, encode(params))
            return decode(buffer, dummyResult)
        })
    }

    onRequest(Url.kUpdate, dummyUpdateResult)
    onRequest(Url.kClose, dummyCloseResult)
    onRequest(Url.kGetSymbol, dummyGetSymbolResult)
    onRequest(Url.kFormat, dummyFormatResult)
    onRequest(Url.kGetToken, dummyGetTokenResult)

    sksl.setOnError((error: string) => {
        client?.sendRequest(Url.kError, error)
    })

    await client.start()
}

export async function deactivate() {
    if (client) {
        await client.stop()
        client = undefined
    }
}
