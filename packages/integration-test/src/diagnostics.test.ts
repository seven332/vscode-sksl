import { toRange } from './helper'
import { Document } from './document'
import * as vscode from 'vscode'
import assert from 'assert'

suite('Diagnostics', () => {
    test('no kind diagnostics', async () => {
        await testDiagnostics('no_kind.sksl', [
            {
                message:
                    'Set the kind of SkSL source to enable Code IntelliSense. // kind=(shader|colorfilter|blender|meshfrag|meshvert)',
                range: toRange(0, 0, 0, 0),
                severity: vscode.DiagnosticSeverity.Information,
            },
        ])
    })

    test('error diagnostics', async () => {
        await testDiagnostics('error.sksl', [
            {
                message: "'main' must return: 'vec4', 'float4', or 'half4'",
                range: toRange(3, 0, 3, 11),
                severity: vscode.DiagnosticSeverity.Error,
            },
        ])
    })

    test('no diagnostics', async () => {
        await testDiagnostics('no_diagnostics.sksl', [])
    })
})

async function testDiagnostics(name: string, expected: vscode.Diagnostic[]) {
    const doc = await Document.create(name)
    const actual = vscode.languages.getDiagnostics(doc.doc.uri)
    assert.equal(actual.length, expected.length)
    expected.forEach((expected_i, i) => {
        const actual_i = actual[i]
        assert.equal(actual_i.message, expected_i.message)
        assert.deepEqual(actual_i.range, expected_i.range)
        assert.equal(actual_i.severity, expected_i.severity)
    })
}
