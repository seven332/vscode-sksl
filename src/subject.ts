export class Subject {
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

        let promiseResolve: (value: string) => void = () => {
            /**/
        }
        const promise = new Promise<string>((resolve) => {
            promiseResolve = resolve
        })
        this.resolves.push(promiseResolve)

        return promise
    }

    private resolves = new Array<(value: string) => void>()
    private strings = new Array<string>()
    private pending = ''
}
