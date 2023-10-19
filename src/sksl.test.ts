import { getSkSLProgramKind, SkSLProgramKind } from './sksl'

describe('getSkSLProgramKind', () => {
    it('right', () => {
        expect(getSkSLProgramKind('// kind=shader')).toBe(SkSLProgramKind.kShader)
        expect(getSkSLProgramKind('// kind=colorfilter')).toBe(SkSLProgramKind.kColorFilter)
        expect(getSkSLProgramKind('// kind=blender')).toBe(SkSLProgramKind.kBlender)
        expect(getSkSLProgramKind('// kind=meshfrag')).toBe(SkSLProgramKind.kMeshFrag)
        expect(getSkSLProgramKind('// kind=meshvert')).toBe(SkSLProgramKind.kMeshVert)
    })

    it('two line', () => {
        expect(getSkSLProgramKind('// kind=shader\nabc')).toBe(SkSLProgramKind.kShader)
    })

    it('second line', () => {
        expect(getSkSLProgramKind('abc\n// kind=shader')).toBeUndefined()
    })

    it('case sensitive', () => {
        expect(getSkSLProgramKind('// kind=Shader')).toBeUndefined()
    })

    it('single slash', () => {
        expect(getSkSLProgramKind('/ kind=shader')).toBeUndefined()
    })

    it('many slash', () => {
        expect(getSkSLProgramKind('//// kind=shader')).toBe(SkSLProgramKind.kShader)
    })

    it('colon', () => {
        expect(getSkSLProgramKind('// kind: shader')).toBe(SkSLProgramKind.kShader)
    })
})
