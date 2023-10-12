export class StringSubject {
    public append(data: Uint8Array) {
        for (const byte of data) {
            if (byte == 10) {
                // \n
                const resolve = this.resolves.shift()
                if (resolve) {
                    resolve(this.pending)
                } else {
                    this.strings.push(this.pending)
                }
                this.pending = ''
            } else {
                this.pending += String.fromCharCode(byte)
            }
        }
    }

    public next(): Promise<string> {
        const first = this.strings.shift()
        if (first) {
            return Promise.resolve(first)
        }

        const promise = new Promise<string>((resolve) => {
            this.resolves.push(resolve)
        })

        return promise
    }

    public fetch(): string | undefined {
        return this.strings.shift()
    }

    private resolves = new Array<(value: string) => void>()
    private strings = new Array<string>()
    private pending = ''
}
