import * as vscode from 'vscode'
import { Document } from './document'
import assert from 'assert'
import { toPosition } from './helper'

suite('Completion', () => {
    test('kind equal completion', async () => {
        await testCompletion('kind_equal.sksl', toPosition(0, 8), [
            {
                label: 'blender',
                kind: vscode.CompletionItemKind.Text,
            },
            {
                label: 'colorfilter',
                kind: vscode.CompletionItemKind.Text,
            },
            {
                label: 'meshfrag',
                kind: vscode.CompletionItemKind.Text,
            },
            {
                label: 'meshvert',
                kind: vscode.CompletionItemKind.Text,
            },
            {
                label: 'shader',
                kind: vscode.CompletionItemKind.Text,
            },
        ])
    })
})

async function testCompletion(name: string, position: vscode.Position, expected: vscode.CompletionItem[]) {
    const doc = await Document.create(name)
    const actual = (
        (await vscode.commands.executeCommand(
            'vscode.executeCompletionItemProvider',
            doc.doc.uri,
            position,
        )) as vscode.CompletionList
    ).items
    assert.equal(actual.length, expected.length)
    expected.forEach((expected_i, i) => {
        const actual_i = actual[i]
        assert.equal(actual_i.label, expected_i.label)
        assert.deepEqual(actual_i.kind, expected_i.kind)
    })
}
