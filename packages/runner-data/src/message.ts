import { type SkSLUniform } from './wasm'

export enum MessageType {
    kSelectSkSL,
    kGetUniforms,
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

export function pipe<T>(t: T): T {
    return t
}
