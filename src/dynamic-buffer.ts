export class DynamicBuffer {
    public constructor() {
        this.data_ = new Uint8Array(8)
        this.size_ = 0
    }

    public get capacity() {
        return this.data_.length
    }

    public get size() {
        return this.size_
    }

    public writeBoolean(value: boolean) {
        this.ensureAppend(1)
        new DataView(this.data_.buffer).setUint8(value ? 1 : 0, this.size_)
        this.size_ += 1
    }

    public writeInt32(value: number) {
        if (!(Number.isInteger(value) && value >= -2147483648 && value <= 2147483647)) {
            throw new Error()
        }
        this.ensureAppend(4)
        new DataView(this.data_.buffer).setInt32(value, this.size_, true)
        this.size_ += 4
    }

    public writeString(value: string) {
        const data = new TextEncoder().encode(value)

        this.writeInt32(data.length)

        this.ensureAppend(data.length)
        this.data_.set(data, this.size_)
        this.size_ += data.length
    }

    public detach(): Uint8Array {
        const result = this.data_.subarray(0, this.size_)
        this.data_ = new Uint8Array(8)
        this.size_ = 0
        return result
    }

    private data_: Uint8Array
    private size_: number

    private ensureAppend(length: number) {
        const atLeast = this.size_ + length
        if (this.capacity >= atLeast) {
            return
        }
        let newCapacity = this.capacity * 2
        while (newCapacity < atLeast) {
            newCapacity *= 2
        }
        const newData = new Uint8Array(newCapacity)
        newData.set(this.data_)
        this.data_ = newData
    }
}
