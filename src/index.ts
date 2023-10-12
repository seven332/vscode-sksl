import * as path from 'path'
import { ExtensionContext } from 'vscode'
import { LanguageClient, TransportKind } from 'vscode-languageclient/node'
import { SkSL } from './sksl-wasi'
import {
    CloseParams,
    FormatParams,
    GetSymbolParams,
    UpdateParams,
    Url,
    dummyCloseResult,
    dummyFormatResult,
    dummyGetSymbolResult,
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

    client.onRequest(Url.kUpdate, async (params: UpdateParams) => {
        const buffer = await sksl.request(Url.kUpdate, encode(params))
        return decode(buffer, dummyUpdateResult)
    })

    client.onRequest(Url.kClose, async (params: CloseParams) => {
        const buffer = await sksl.request(Url.kClose, encode(params))
        return decode(buffer, dummyCloseResult)
    })

    client.onRequest(Url.kGetSymbol, async (params: GetSymbolParams) => {
        const buffer = await sksl.request(Url.kGetSymbol, encode(params))
        return decode(buffer, dummyGetSymbolResult)
    })

    client.onRequest(Url.kFormat, async (params: FormatParams) => {
        const buffer = await sksl.request(Url.kFormat, encode(params))
        return decode(buffer, dummyFormatResult)
    })

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
