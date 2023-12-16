export interface QueryParams {
    source: string
}

export interface SkSLUniform {
    type: string
    name: string
}

export const dummySkSLUniform: SkSLUniform = {
    type: '',
    name: '',
}

export interface QueryResult {
    succeed: boolean
    kind: string
    uniforms: SkSLUniform[]
}

export const dummyQueryResult: QueryResult = {
    succeed: false,
    kind: '',
    uniforms: [dummySkSLUniform],
}
