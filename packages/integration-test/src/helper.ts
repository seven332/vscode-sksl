import * as vscode from 'vscode'

export function toPosition(line: number, char: number) {
    return new vscode.Position(line, char)
}

export function toRange(sLine: number, sChar: number, eLine: number, eChar: number) {
    const start = new vscode.Position(sLine, sChar)
    const end = new vscode.Position(eLine, eChar)
    return new vscode.Range(start, end)
}
