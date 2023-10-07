export class BufferSubject {
    public append(data: Uint8Array | undefined) {
        while (data) {
            data = this.length.append(data)
            if (!data) {
                break
            }
            if (!this.length.isFull()) {
                break
            }
            if (!this.buffer) {
                const length = this.length.getDataView().getUint32(0, true)
                this.buffer = new Buffer(length)
            }
            data = this.buffer.append(data)
            if (this.buffer.isFull()) {
                const buffer = this.buffer.getData()
                const resolve = this.resolves.shift()
                if (resolve) {
                    resolve(buffer)
                } else {
                    this.buffers.push(buffer)
                }
                this.buffer = undefined
                this.length.reset()
            }
        }
    }

    public next(): Promise<Uint8Array> {
        const first = this.buffers.shift()
        if (first) {
            return Promise.resolve(first)
        }

        const promise = new Promise<Uint8Array>((resolve) => {
            this.resolves.push(resolve)
        })

        return promise
    }

    private length = new Buffer(4)
    private buffer: Buffer | undefined
    private resolves = new Array<(buffer: Uint8Array) => void>()
    private buffers = new Array<Uint8Array>()
}

class Buffer {
    constructor(capacity: number) {
        this.buffer = new Uint8Array(capacity)
        this.size = 0
    }

    public isFull(): boolean {
        return this.size >= this.capacity
    }

    public getData(): Uint8Array {
        return this.buffer
    }

    public getDataView(): DataView {
        return new DataView(this.buffer.buffer)
    }

    public reset() {
        this.size = 0;
    }

    public append(data: Uint8Array): Uint8Array | undefined {
        if (data.length <= 0) {
            return undefined
        }
        if (this.isFull()) {
            return data
        }

        const remain = this.capacity - this.size
        const read = Math.min(remain, data.length)
        this.buffer.set(data.subarray(0, read), this.size)
        this.size += read

        if (data.length == read) {
            return undefined
        } else {
            return data.subarray(read)
        }
    }

    private buffer: Uint8Array
    private size: number
    private get capacity() {
        return this.buffer.length
    }
}
