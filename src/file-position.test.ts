import { FilePosition } from './file-position'
import * as ls from 'vscode-languageserver/node'

describe('FilePosition', () => {
    it('normal', () => {
        const fp = new FilePosition('abc\nbcd')
        expect(fp.getPosition(0)).toStrictEqual(ls.Position.create(0, 0))
        expect(fp.getPosition(1)).toStrictEqual(ls.Position.create(0, 1))
        expect(fp.getPosition(2)).toStrictEqual(ls.Position.create(0, 2))
        expect(fp.getPosition(3)).toStrictEqual(ls.Position.create(0, 3))
        expect(fp.getPosition(4)).toStrictEqual(ls.Position.create(1, 0))
        expect(fp.getPosition(5)).toStrictEqual(ls.Position.create(1, 1))
        expect(fp.getPosition(6)).toStrictEqual(ls.Position.create(1, 2))
        expect(fp.getPosition(7)).toStrictEqual(ls.Position.create(1, 3))
        expect(fp.getPosition(8)).toStrictEqual(ls.Position.create(1, 4))
    })

    it('first break line', () => {
        const fp = new FilePosition('\nabc')
        expect(fp.getPosition(0)).toStrictEqual(ls.Position.create(0, 0))
        expect(fp.getPosition(1)).toStrictEqual(ls.Position.create(1, 0))
        expect(fp.getPosition(2)).toStrictEqual(ls.Position.create(1, 1))
        expect(fp.getPosition(3)).toStrictEqual(ls.Position.create(1, 2))
        expect(fp.getPosition(4)).toStrictEqual(ls.Position.create(1, 3))
    })

    it('break', () => {
        const fp = new FilePosition('\r\r\r\n\n\n\r\n\r\n\n\r\n\r')
        expect(fp.getPosition(0)).toStrictEqual(ls.Position.create(0, 0))
        expect(fp.getPosition(1)).toStrictEqual(ls.Position.create(1, 0))
        expect(fp.getPosition(2)).toStrictEqual(ls.Position.create(2, 0))
        expect(fp.getPosition(3)).toStrictEqual(ls.Position.create(2, 1))
        expect(fp.getPosition(4)).toStrictEqual(ls.Position.create(3, 0))
        expect(fp.getPosition(5)).toStrictEqual(ls.Position.create(4, 0))
        expect(fp.getPosition(6)).toStrictEqual(ls.Position.create(5, 0))
        expect(fp.getPosition(7)).toStrictEqual(ls.Position.create(5, 1))
        expect(fp.getPosition(8)).toStrictEqual(ls.Position.create(6, 0))
    })
})