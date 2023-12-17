import { Float } from '@workspace/float'

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

export interface RunParams {
    source: string
    values: string[]
}

export interface SkSLColor {
    r: Float
    g: Float
    b: Float
    a: Float
}

export const dummySkSLColor: SkSLColor = {
    r: Float.kZero,
    g: Float.kZero,
    b: Float.kZero,
    a: Float.kZero,
}

export interface RunResult {
    succeed: boolean
    color: SkSLColor
}

export const dummyRunResult: RunResult = {
    succeed: false,
    color: dummySkSLColor,
}
