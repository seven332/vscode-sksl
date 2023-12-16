export enum MessageType {
    kSelectSkSL,
    kGetUniforms,
}

export interface Message {
    type: MessageType
}

export interface SelectSkSLRequestMessage extends Message {
    type: MessageType.kSelectSkSL
}

export interface SelectSkSLResponseMessage extends Message {
    type: MessageType.kSelectSkSL
    path: string
}

export function pipe<T>(t: T): T {
    return t
}
