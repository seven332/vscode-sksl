import * as fs from 'fs'
import { Wasm, WasmProcess } from '@vscode/wasm-wasi'

export class SkSL {
    public static async create(wasmPath: string): Promise<SkSL> {
        const bits = fs.readFileSync(wasmPath)
        const wasm = await Wasm.load()
        const wasmModule = await WebAssembly.compile(bits)
        const wasmProcess = await wasm.createProcess('sksl-wasi', wasmModule)
        return new SkSL(wasmProcess)
    }

    public run() {
        this.wasmProcess.run()
    }

    private constructor(private wasmProcess: WasmProcess) {}
}
