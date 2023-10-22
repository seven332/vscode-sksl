import { findFirstGE, findFirstGT, findLastLE, findLastLT } from './binary-search'

describe('binary-search', () => {
    it('findFirstGE', () => {
        expect(findFirstGE([], 1, (element, value) => element < value)).toBe(-1)
        expect(findFirstGE([1], 2, (element, value) => element < value)).toBe(-1)
        expect(findFirstGE([1, 1], 2, (element, value) => element < value)).toBe(-1)
        expect(findFirstGE([1, 1, 1], 2, (element, value) => element < value)).toBe(-1)

        expect(findFirstGE([1, 2, 3], 2, (element, value) => element < value)).toBe(1)
        expect(findFirstGE([1, 2, 2, 3], 2, (element, value) => element < value)).toBe(1)
        expect(findFirstGE([1, 2, 2, 2, 3], 2, (element, value) => element < value)).toBe(1)

        expect(findFirstGE([1, 1, 2, 3], 2, (element, value) => element < value)).toBe(2)
        expect(findFirstGE([1, 1, 1, 2, 3], 2, (element, value) => element < value)).toBe(3)

        expect(findFirstGE([1, 1, 3], 2, (element, value) => element < value)).toBe(2)
        expect(findFirstGE([1, 1, 1, 3], 2, (element, value) => element < value)).toBe(3)
    })

    it('findFirstGT', () => {
        expect(findFirstGT([], 1, (value, element) => value < element)).toBe(-1)
        expect(findFirstGT([1], 2, (value, element) => value < element)).toBe(-1)
        expect(findFirstGT([1, 1], 2, (value, element) => value < element)).toBe(-1)
        expect(findFirstGT([1, 1, 1], 2, (value, element) => value < element)).toBe(-1)

        expect(findFirstGT([1, 2, 3], 2, (value, element) => value < element)).toBe(2)
        expect(findFirstGT([1, 2, 2, 3], 2, (value, element) => value < element)).toBe(3)
        expect(findFirstGT([1, 2, 2, 2, 3], 2, (value, element) => value < element)).toBe(4)

        expect(findFirstGT([1, 2, 3, 3], 2, (value, element) => value < element)).toBe(2)
        expect(findFirstGT([1, 2, 3, 3, 3], 2, (value, element) => value < element)).toBe(2)
    })

    it('findLastLE', () => {
        expect(findLastLE([], 1, (value, element) => value < element)).toBe(-1)
        expect(findLastLE([1], 2, (value, element) => value < element)).toBe(0)
        expect(findLastLE([1, 1], 2, (value, element) => value < element)).toBe(1)
        expect(findLastLE([1, 1, 1], 2, (value, element) => value < element)).toBe(2)

        expect(findLastLE([1, 2, 3], 2, (value, element) => value < element)).toBe(1)
        expect(findLastLE([1, 2, 2, 3], 2, (value, element) => value < element)).toBe(2)
        expect(findLastLE([1, 2, 2, 2, 3], 2, (value, element) => value < element)).toBe(3)

        expect(findLastLE([1, 2, 3, 3], 2, (value, element) => value < element)).toBe(1)
        expect(findLastLE([1, 2, 3, 3, 3], 2, (value, element) => value < element)).toBe(1)
    })

    it('findLastLT', () => {
        expect(findLastLT([], 1, (element, value) => element < value)).toBe(-1)
        expect(findLastLT([1], 2, (element, value) => element < value)).toBe(0)
        expect(findLastLT([1, 1], 2, (element, value) => element < value)).toBe(1)
        expect(findLastLT([1, 1, 1], 2, (element, value) => element < value)).toBe(2)

        expect(findLastLT([1, 2, 3], 2, (element, value) => element < value)).toBe(0)
        expect(findLastLT([1, 2, 2, 3], 2, (element, value) => element < value)).toBe(0)
        expect(findLastLT([1, 2, 2, 2, 3], 2, (element, value) => element < value)).toBe(0)

        expect(findLastLT([1, 1, 2, 3], 2, (element, value) => element < value)).toBe(1)
        expect(findLastLT([1, 1, 1, 2, 3], 2, (element, value) => element < value)).toBe(2)

        expect(findLastLT([1, 1, 3], 2, (element, value) => element < value)).toBe(1)
        expect(findLastLT([1, 1, 1, 3], 2, (element, value) => element < value)).toBe(2)
    })
})
