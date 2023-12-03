import * as ls from 'vscode-languageserver/node'
import { findLastLE } from '@workspace/util'

export class OffsetPositionConverter {
    public constructor(content: string) {
        if (content.length <= 0) {
            return
        }
        this.lineOffsets.push(0)

        let index = 0
        let prev = ' '
        for (let i = 0; i < content.length; ++i) {
            const c = content.charAt(i)
            switch (c) {
                case '\n':
                    if (prev == '\n') {
                        this.lineOffsets.push(index)
                    }
                    break
                default:
                    if (prev == '\r' || prev == '\n') {
                        this.lineOffsets.push(index)
                    }
                    break
            }
            prev = c
            index += 1
        }
    }

    public getLines(): number {
        return this.lineOffsets.length
    }

    public toPosition(offset: number): ls.Position {
        const index = findLastLE(this.lineOffsets, offset, (value, element) => value < element)
        if (index == -1) {
            return ls.Position.create(0, offset)
        }
        const position = this.lineOffsets[index]
        return ls.Position.create(index, offset - position)
    }

    public toOffset(position: ls.Position): number {
        const line = Math.min(position.line, this.lineOffsets.length - 1)
        return this.lineOffsets[line] + position.character
    }

    private lineOffsets = new Array<number>()
}
