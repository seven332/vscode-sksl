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
    RunSkSLRequest,
    DebugSkSLRequest,
    kRunUrl,
    RunResult,
    RunParams,
    QueryParams,
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
                    case MessageType.kSelectSkSL:
                        await this.onSelectSkSLRequest(panel)
                        break
                    case MessageType.kRunSkSL:
                        await this.onRunSkSLRequest(panel, message as RunSkSLRequest)
                        break
                    case MessageType.kDebugSkSL:
                        await this.onDebugSkSLRequest(panel, message as DebugSkSLRequest)
                        break
                }
            },
            undefined,
            this.context.subscriptions,
        )
    }

    private async onSelectSkSLRequest(panel: vscode.WebviewPanel) {
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
    }

    private async selectSkSL(panel: vscode.WebviewPanel, uri: vscode.Uri) {
        panel.webview.postMessage(
            pipe<SelectSkSLResponse>({
                type: MessageType.kSelectSkSL,
                path: uri.fsPath,
            }),
        )

        const buffer = fs.readFileSync(uri.fsPath)
        const result: QueryResult = await this.client.sendRequest(
            kQueryUrl,
            pipe<QueryParams>({
                source: buffer.toString(),
            }),
        )
        panel.webview.postMessage(
            pipe<GetUniformsResponse>({
                type: MessageType.kGetUniforms,
                uniforms: result.uniforms,
            }),
        )
    }

    private async onRunSkSLRequest(panel: vscode.WebviewPanel, request: RunSkSLRequest) {
        const buffer = fs.readFileSync(request.path)
        const result: RunResult = await this.client.sendRequest(
            kRunUrl,
            pipe<RunParams>({
                source: buffer.toString(),
                values: request.values,
            }),
        )
        // TODO:
        console.log(result.color)
    }

    private async onDebugSkSLRequest(panel: vscode.WebviewPanel, request: DebugSkSLRequest) {
        console.log(panel)
        console.log(request)
    }
}
