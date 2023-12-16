import * as vscode from 'vscode'
import { LanguageClient } from 'vscode-languageclient/node'
import * as path from 'path'
import * as fs from 'fs'
import {
    Message,
    QueryResult,
    kQueryUrl,
    MessageType,
    SelectSkSLResponse,
    pipe,
    GetUniformsResponse,
} from '@workspace/runner-data'

export class Runner {
    public static kCommand = 'sksl.showRunner'

    public constructor(
        private context: vscode.ExtensionContext,
        private client: LanguageClient,
    ) {}

    public run(uri: vscode.Uri | undefined) {
        const panel = vscode.window.createWebviewPanel('sksl.runner', 'SkSL Runner', vscode.ViewColumn.Beside, {
            enableScripts: true,
        })

        const htmlPath = this.context.asAbsolutePath(path.join('build', 'runner-ui', 'index.html'))
        panel.webview.html = fs.readFileSync(htmlPath).toString()

        if (uri) {
            this.selectSkSL(panel, uri)
        }

        panel.webview.onDidReceiveMessage(
            async (message: Message) => {
                switch (message.type) {
                    case MessageType.kSelectSkSL: {
                        const uris = await vscode.window.showOpenDialog({
                            canSelectFiles: true,
                            canSelectFolders: false,
                            canSelectMany: false,
                            filters: {
                                SkSL: ['sksl'],
                            },
                        })
                        if (uris && uris.length >= 1) {
                            this.selectSkSL(panel, uris[0])
                        }
                        break
                    }
                }
            },
            undefined,
            this.context.subscriptions,
        )
    }

    private async selectSkSL(panel: vscode.WebviewPanel, uri: vscode.Uri) {
        panel.webview.postMessage(
            pipe<SelectSkSLResponse>({
                type: MessageType.kSelectSkSL,
                path: uri.toString(),
            }),
        )

        const buffer = fs.readFileSync(uri.fsPath)
        const result: QueryResult = await this.client.sendRequest(kQueryUrl, {
            source: buffer.toString(),
        })
        panel.webview.postMessage(
            pipe<GetUniformsResponse>({
                type: MessageType.kGetUniforms,
                uniforms: result.uniforms,
            }),
        )
    }
}
