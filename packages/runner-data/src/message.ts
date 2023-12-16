import { type SkSLUniform } from './wasm'

export enum MessageType {
    kSelectSkSL,
    kGetUniforms,
    kRunSkSL,
    kDebugSkSL,
}

export interface Message {
    type: MessageType
}

export interface SelectSkSLRequest extends Message {
    type: MessageType.kSelectSkSL
}

export interface SelectSkSLResponse extends Message {
    type: MessageType.kSelectSkSL
    path: string
}

export interface GetUniformsResponse extends Message {
    type: MessageType.kGetUniforms
    uniforms: SkSLUniform[]
}

export interface RunSkSLRequest extends Message {
    type: MessageType.kRunSkSL
    path: string
    values: string[]
}

export interface DebugSkSLRequest extends Message {
    type: MessageType.kDebugSkSL
    path: string
    values: string[]
}

export function pipe<T>(t: T): T {
    return t
}
