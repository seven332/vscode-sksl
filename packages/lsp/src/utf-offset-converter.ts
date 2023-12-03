import { findFirstGE } from '@workspace/util'

export interface UTFOffset {
    utf8Offset: number
    utf16Offset: number
}

export const dummyUTFOffset: UTFOffset = {
    utf8Offset: 0,
    utf16Offset: 0,
}

export class UTFOffsetConverter {
    public constructor(private offsets: UTFOffset[]) {}

    public toUTF16(utf8Offset: number): number {
        let index = findFirstGE(this.offsets, utf8Offset, (element, utf8Offset) => element.utf8Offset < utf8Offset)
        if (index == -1) {
            index = this.offsets.length - 1
        }
        return this.offsets[index].utf16Offset
    }

    public toUTF8(utf16Offset: number): number {
        let index = findFirstGE(this.offsets, utf16Offset, (element, utf16Offset) => element.utf16Offset < utf16Offset)
        if (index == -1) {
            index = this.offsets.length - 1
        }
        return this.offsets[index].utf8Offset
    }
}
