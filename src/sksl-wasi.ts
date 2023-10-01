import * as fs from 'fs'
import { Wasm, WasmProcess } from '@vscode/wasm-wasi'
import { Method } from './sksl'
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

    public run() {
        this.wasmProcess.run()
        this.wasmProcess.stdout?.onData((data) => {
            this.subject.append(data)
        })
    }

    public async request(method: Method, params: unknown) {
        await this.wasmProcess.stdin?.write(method)
        await this.wasmProcess.stdin?.write('\n')
        await this.wasmProcess.stdin?.write(JSON.stringify(params))
        await this.wasmProcess.stdin?.write('\n')
        return await this.subject.next()
    }

    private constructor(private wasmProcess: WasmProcess) {}

    private subject = new Subject()
}
