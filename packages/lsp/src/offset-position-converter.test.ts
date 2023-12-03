import { OffsetPositionConverter } from './offset-position-converter'
import * as ls from 'vscode-languageserver/node'

describe('OffsetPosition', () => {
    describe('toPosition', () => {
        it('normal', () => {
            const fp = new OffsetPositionConverter('abc\nbcd')
            expect(fp.toPosition(0)).toStrictEqual(ls.Position.create(0, 0))
            expect(fp.toPosition(1)).toStrictEqual(ls.Position.create(0, 1))
            expect(fp.toPosition(2)).toStrictEqual(ls.Position.create(0, 2))
            expect(fp.toPosition(3)).toStrictEqual(ls.Position.create(0, 3))
            expect(fp.toPosition(4)).toStrictEqual(ls.Position.create(1, 0))
            expect(fp.toPosition(5)).toStrictEqual(ls.Position.create(1, 1))
            expect(fp.toPosition(6)).toStrictEqual(ls.Position.create(1, 2))
            expect(fp.toPosition(7)).toStrictEqual(ls.Position.create(1, 3))
            expect(fp.toPosition(8)).toStrictEqual(ls.Position.create(1, 4))
        })

        it('first break line', () => {
            const fp = new OffsetPositionConverter('\nabc')
            expect(fp.toPosition(0)).toStrictEqual(ls.Position.create(0, 0))
            expect(fp.toPosition(1)).toStrictEqual(ls.Position.create(1, 0))
            expect(fp.toPosition(2)).toStrictEqual(ls.Position.create(1, 1))
            expect(fp.toPosition(3)).toStrictEqual(ls.Position.create(1, 2))
            expect(fp.toPosition(4)).toStrictEqual(ls.Position.create(1, 3))
        })

        it('break', () => {
            const fp = new OffsetPositionConverter('\r\r\r\n\n\n\r\n\r\n\n\r\n\r')
            expect(fp.toPosition(0)).toStrictEqual(ls.Position.create(0, 0))
            expect(fp.toPosition(1)).toStrictEqual(ls.Position.create(1, 0))
            expect(fp.toPosition(2)).toStrictEqual(ls.Position.create(2, 0))
            expect(fp.toPosition(3)).toStrictEqual(ls.Position.create(2, 1))
            expect(fp.toPosition(4)).toStrictEqual(ls.Position.create(3, 0))
            expect(fp.toPosition(5)).toStrictEqual(ls.Position.create(4, 0))
            expect(fp.toPosition(6)).toStrictEqual(ls.Position.create(5, 0))
            expect(fp.toPosition(7)).toStrictEqual(ls.Position.create(5, 1))
            expect(fp.toPosition(8)).toStrictEqual(ls.Position.create(6, 0))
        })
    })

    describe('toOffset', () => {
        it('normal', () => {
            const fp = new OffsetPositionConverter('abc\nbcd')
            expect(fp.toOffset(ls.Position.create(0, 0))).toBe(0)
            expect(fp.toOffset(ls.Position.create(0, 1))).toBe(1)
            expect(fp.toOffset(ls.Position.create(0, 2))).toBe(2)
            expect(fp.toOffset(ls.Position.create(0, 3))).toBe(3)
            expect(fp.toOffset(ls.Position.create(0, 4))).toBe(4)
            expect(fp.toOffset(ls.Position.create(1, 0))).toBe(4)
            expect(fp.toOffset(ls.Position.create(1, 1))).toBe(5)
            expect(fp.toOffset(ls.Position.create(1, 2))).toBe(6)
            expect(fp.toOffset(ls.Position.create(1, 3))).toBe(7)
            expect(fp.toOffset(ls.Position.create(1, 4))).toBe(8)
            expect(fp.toOffset(ls.Position.create(2, 0))).toBe(4)
            expect(fp.toOffset(ls.Position.create(2, 1))).toBe(5)
        })
    })
})
