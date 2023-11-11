import { UTFOffsetConverter } from './utf-offset-converter'

describe('UTFOffset', () => {
    const converter = new UTFOffsetConverter([
        { utf8Offset: 0, utf16Offset: 0 },
        { utf8Offset: 1, utf16Offset: 1 },
        { utf8Offset: 2, utf16Offset: 2 },
        { utf8Offset: 3, utf16Offset: 3 },
        { utf8Offset: 6, utf16Offset: 4 },
        { utf8Offset: 9, utf16Offset: 5 },
    ])

    it('toUTF8', () => {
        expect(converter.toUTF8(0)).toBe(0)
        expect(converter.toUTF8(1)).toBe(1)
        expect(converter.toUTF8(2)).toBe(2)
        expect(converter.toUTF8(3)).toBe(3)
        expect(converter.toUTF8(4)).toBe(6)
        expect(converter.toUTF8(5)).toBe(9)
        expect(converter.toUTF8(6)).toBe(9)
    })

    it('toUTF16', () => {
        expect(converter.toUTF16(0)).toBe(0)
        expect(converter.toUTF16(1)).toBe(1)
        expect(converter.toUTF16(2)).toBe(2)
        expect(converter.toUTF16(3)).toBe(3)
        expect(converter.toUTF16(4)).toBe(4)
        expect(converter.toUTF16(5)).toBe(4)
        expect(converter.toUTF16(6)).toBe(4)
        expect(converter.toUTF16(7)).toBe(5)
        expect(converter.toUTF16(8)).toBe(5)
        expect(converter.toUTF16(9)).toBe(5)
        expect(converter.toUTF16(10)).toBe(5)
    })
})
