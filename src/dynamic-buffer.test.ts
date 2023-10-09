import { DynamicBuffer } from './dynamic-buffer'

describe('DynamicBuffer', () => {
    it('writeBoolean', () => {
        const buffer = new DynamicBuffer()
        buffer.writeBoolean(true)
        expect(buffer.size).toBe(1)
        expect(buffer.capacity).toBe(8)
        buffer.writeBoolean(true)
        expect(buffer.size).toBe(2)
        expect(buffer.capacity).toBe(8)
        buffer.writeBoolean(true)
        expect(buffer.size).toBe(3)
        expect(buffer.capacity).toBe(8)
        buffer.writeBoolean(true)
        expect(buffer.size).toBe(4)
        expect(buffer.capacity).toBe(8)
        buffer.writeBoolean(true)
        expect(buffer.size).toBe(5)
        expect(buffer.capacity).toBe(8)
        buffer.writeBoolean(true)
        expect(buffer.size).toBe(6)
        expect(buffer.capacity).toBe(8)
        buffer.writeBoolean(true)
        expect(buffer.size).toBe(7)
        expect(buffer.capacity).toBe(8)
        buffer.writeBoolean(true)
        expect(buffer.size).toBe(8)
        expect(buffer.capacity).toBe(8)
        buffer.writeBoolean(true)
        expect(buffer.size).toBe(9)
        expect(buffer.capacity).toBe(16)
    })

    it('writeInt32', () => {
        const buffer = new DynamicBuffer()
        buffer.writeInt32(3)
        expect(buffer.size).toBe(4)
        expect(buffer.capacity).toBe(8)
        buffer.writeInt32(3)
        expect(buffer.size).toBe(8)
        expect(buffer.capacity).toBe(8)
        buffer.writeInt32(3)
        expect(buffer.size).toBe(12)
        expect(buffer.capacity).toBe(16)
        buffer.writeInt32(3)
        expect(buffer.size).toBe(16)
        expect(buffer.capacity).toBe(16)
        buffer.writeInt32(3)
        expect(buffer.size).toBe(20)
        expect(buffer.capacity).toBe(32)
    })

    it('writeString', () => {
        const buffer = new DynamicBuffer()
        buffer.writeString('abc')
        expect(buffer.size).toBe(7)
        expect(buffer.capacity).toBe(8)
        buffer.writeString('abcde')
        expect(buffer.size).toBe(16)
        expect(buffer.capacity).toBe(16)
    })

    it('detach', () => {
        const buffer = new DynamicBuffer()
        buffer.writeString('abc')
        expect(buffer.detach().length).toBe(7)
        expect(buffer.size).toBe(0)
    })
})
