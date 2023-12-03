import * as vscode from 'vscode'
import * as path from 'path'

export class Document {
    public static async create(name: string): Promise<Document> {
        const uri = vscode.Uri.file(path.resolve(__dirname, '../src/fixtures', name))
        const ext = vscode.extensions.getExtension('seven332.vscode-sksl')!
        await ext.activate()
        const doc = await vscode.workspace.openTextDocument(uri)
        const editor = await vscode.window.showTextDocument(doc)
        // Wait for server activation
        await this.sleep(2000)
        return new Document(doc, editor)
    }

    private constructor(
        readonly doc: vscode.TextDocument,
        readonly editor: vscode.TextEditor,
    ) {}

    private static async sleep(ms: number) {
        return new Promise((resolve) => setTimeout(resolve, ms))
    }
}
