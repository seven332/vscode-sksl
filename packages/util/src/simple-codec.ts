import { DynamicBuffer } from './dynamic-buffer'
import { Float } from '@workspace/float'

export function encode(value: unknown): Uint8Array {
    const buffer = new DynamicBuffer()
    encodeToBuffer(buffer, value)
    return buffer.detach()
}

function encodeToBuffer(buffer: DynamicBuffer, value: unknown) {
    if (typeof value == 'boolean') {
        buffer.writeBoolean(value)
    } else if (typeof value == 'number') {
        buffer.writeInt32(value)
    } else if (typeof value == 'string') {
        buffer.writeString(value)
    } else if (Array.isArray(value)) {
        buffer.writeInt32(value.length)
        for (const element of value) {
            encodeToBuffer(buffer, element)
        }
    } else if (value instanceof Float) {
        buffer.writeFloat32(value.value)
    } else if (typeof value == 'object' && value != null) {
        for (const element of Object.values(value)) {
            encodeToBuffer(buffer, element)
        }
    } else {
        throw new Error(`Unsupported type: ${typeof value}`)
    }
}

export function decode<T>(buffer: Uint8Array, dummy: T): T {
    const [result, read] = decodeFromBuffer<T>(
        new DataView(buffer.buffer, buffer.byteOffset, buffer.byteLength),
        0,
        dummy,
    )
    if (read != buffer.length) {
        throw new Error(`${read} != ${buffer.length}`)
    }
    return result
}

function decodeFromBuffer<T>(buffer: DataView, offset: number, dummy: T): [T, number] {
    if (typeof dummy == 'boolean') {
        return [(buffer.getUint8(offset) != 0) as T, 1]
    } else if (typeof dummy == 'number') {
        return [buffer.getInt32(offset, true) as T, 4]
    } else if (typeof dummy == 'string') {
        let read = 0

        const length = buffer.getInt32(offset, true)
        read += 4

        const bytes = new Uint8Array(buffer.buffer, buffer.byteOffset + offset + read, length)
        read += length

        return [new TextDecoder().decode(bytes) as T, read]
    } else if (Array.isArray(dummy)) {
        const result = []
        let read = 0
        const elementDummy = dummy[0]

        const length = buffer.getInt32(offset, true)
        read += 4

        for (let i = 0; i < length; i += 1) {
            const [element, elementRead] = decodeFromBuffer(buffer, offset + read, elementDummy)
            result.push(element as never)
            read += elementRead
        }

        return [result as T, read]
    } else if (dummy instanceof Float) {
        return [new Float(buffer.getFloat32(offset, true)) as T, 4]
    } else if (typeof dummy == 'object' && dummy != null) {
        const result = {}
        let read = 0

        for (const [elementKey, elementDummy] of Object.entries(dummy)) {
            const [element, elementRead] = decodeFromBuffer(buffer, offset + read, elementDummy)
            result[elementKey] = element
            read += elementRead
        }

        return [result as T, read]
    } else {
        throw new Error(`Unsupported type: ${typeof dummy}`)
    }
}
