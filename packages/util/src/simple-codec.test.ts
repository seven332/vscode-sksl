import { Float } from './float'
import { encode, decode } from './simple-codec'

interface TestObject {
    a: boolean
    b: number
    c: string
    d: number[]
}

const testObjectDummy: TestObject = {
    a: false,
    b: 0,
    c: '',
    d: [0],
}

enum TestEnum {
    a = 'a',
    b = 'b',
}

describe('SimpleBuffer', () => {
    it('boolean', () => {
        expect(decode<boolean>(encode(false), false)).toBe(false)
        expect(decode<boolean>(encode(true), false)).toBe(true)
    })

    it('int32', () => {
        expect(decode<number>(encode(-2), 0)).toBe(-2)
        expect(decode<number>(encode(-1), 0)).toBe(-1)
        expect(decode<number>(encode(0), 0)).toBe(0)
        expect(decode<number>(encode(1), 0)).toBe(1)
        expect(decode<number>(encode(2), 0)).toBe(2)
        expect(decode<number>(encode(-2147483648), 0)).toBe(-2147483648)
        expect(decode<number>(encode(2147483647), 0)).toBe(2147483647)
    })

    it('float32', () => {
        expect(decode<Float>(encode(new Float(1.25)), Float.kZero)).toStrictEqual(new Float(1.25))
    })

    it('string', () => {
        expect(decode<string>(encode('hello🌎𠮷'), '')).toBe('hello🌎𠮷')
    })

    it('array', () => {
        expect(decode<boolean[]>(encode([false, true, false]), [false])).toStrictEqual([false, true, false])
        expect(decode<number[]>(encode([-1, 0, 1]), [0])).toStrictEqual([-1, 0, 1])
        expect(decode<string[]>(encode(['', '1', '123']), [''])).toStrictEqual(['', '1', '123'])
        expect(decode<string[][]>(encode([[''], ['1'], ['2', '123']]), [['']])).toStrictEqual([
            [''],
            ['1'],
            ['2', '123'],
        ])
    })

    it('object', () => {
        const value: TestObject = {
            a: true,
            b: 12345,
            c: '54321',
            d: [784935, 45236523],
        }
        expect(decode<TestObject>(encode(value), testObjectDummy)).toStrictEqual(value)
    })

    it('enum', () => {
        expect(decode<TestEnum>(encode(TestEnum.b), TestEnum.a)).toStrictEqual(TestEnum.b)
    })
})
