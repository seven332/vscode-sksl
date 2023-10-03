import * as fs from 'fs'
import { Wasm, WasmProcess } from '@vscode/wasm-wasi'
import { Subject } from './subject'

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

    public async request(method: string, params: string) {
        await this.wasmProcess.stdin?.write(method)
        await this.wasmProcess.stdin?.write('\n')
        await this.wasmProcess.stdin?.write(params)
        await this.wasmProcess.stdin?.write('\n')
        return await this.outSubject.next()
    }

    private constructor(private wasmProcess: WasmProcess) {}

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
    private outSubject = new Subject()
    private errSubject = new Subject()
}
