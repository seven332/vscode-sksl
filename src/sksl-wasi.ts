import * as fs from 'fs'
import { Wasm, WasmProcess } from '@vscode/wasm-wasi'
import { StringSubject } from './string-subject'
import { BufferSubject } from './buffer-subject'
import { encode } from './simple-codec'

export class SkSL {
    public static async create(wasmPath: string): Promise<SkSL> {
        const bits = fs.readFileSync(wasmPath)
        const wasm = await Wasm.load()
        const wasmModule = await WebAssembly.compile(bits)
        const wasmProcess = await wasm.createProcess('sksl-wasi', wasmModule, {
            stdio: { in: { kind: 'pipeIn' }, out: { kind: 'pipeOut' }, err: { kind: 'pipeOut' } },
        })
        return new SkSL(wasmProcess)
    }

    public setOnError(onError: (error: string) => void) {
        this.onError = onError
        this.flushError()
    }

    public run() {
        this.wasmProcess.run()
        this.wasmProcess.stdout?.onData((data) => {
            this.outSubject.append(data)
        })
        this.wasmProcess.stderr?.onData((data) => {
            this.errSubject.append(data)
            this.flushError()
        })
    }

    public async request(url: string, params: Uint8Array): Promise<Uint8Array> {
        this.write(encode(url))
        this.write(params)
        return await this.outSubject.next()
    }

    private constructor(private wasmProcess: WasmProcess) {}

    private async write(bytes: Uint8Array) {
        const buffer = new Uint8Array(4 + bytes.byteLength)
        new DataView(buffer.buffer).setUint32(0, bytes.byteLength, true)
        buffer.set(bytes, 4)
        await this.wasmProcess.stdin?.write(buffer)
    }

    private flushError() {
        if (this.onError) {
            let error = this.errSubject.fetch()
            while (error) {
                this.onError(error)
                error = this.errSubject.fetch()
            }
        }
    }

    private onError: ((error: string) => void) | undefined
    private outSubject = new BufferSubject()
    private errSubject = new StringSubject()
}
