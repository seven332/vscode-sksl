import { OffsetPositionConverter } from './offset-position-converter'
import { UTFOffset, UTFOffsetConverter } from './utf-offset-converter'
import * as ls from 'vscode-languageserver/node'

export class FilePosition {
    public constructor(content: string, offsets: UTFOffset[]) {
        this.offsetPositionConverter = new OffsetPositionConverter(content)
        this.utfOffsetConverter = new UTFOffsetConverter(offsets)
    }

    public getLines(): number {
        return this.offsetPositionConverter.getLines()
    }

    public toPosition(offset: number): ls.Position {
        return this.offsetPositionConverter.toPosition(this.utfOffsetConverter.toUTF16(offset))
    }

    public toOffset(position: ls.Position): number {
        return this.utfOffsetConverter.toUTF8(this.offsetPositionConverter.toOffset(position))
    }

    private offsetPositionConverter: OffsetPositionConverter
    private utfOffsetConverter: UTFOffsetConverter
}
