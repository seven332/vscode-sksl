import { SkSLProgramKind, getSkSLProgramKind } from './sksl'

describe('getSkSLProgramKind', () => {
    it('right', () => {
        expect(getSkSLProgramKind('// kind=frag')).toBe(SkSLProgramKind.kFrag)
        expect(getSkSLProgramKind('// kind=vert')).toBe(SkSLProgramKind.kVert)
        expect(getSkSLProgramKind('// kind=compute')).toBe(SkSLProgramKind.kCompute)
        expect(getSkSLProgramKind('// kind=shader')).toBe(SkSLProgramKind.kShader)
        expect(getSkSLProgramKind('// kind=colorfilter')).toBe(SkSLProgramKind.kColorFilter)
        expect(getSkSLProgramKind('// kind=blender')).toBe(SkSLProgramKind.kBlender)
        expect(getSkSLProgramKind('// kind=mesh-vert')).toBe(SkSLProgramKind.kMeshVert)
        expect(getSkSLProgramKind('// kind=mesh-frag')).toBe(SkSLProgramKind.kMeshFrag)
    })

    it('case sensitive', () => {
        expect(getSkSLProgramKind('// kind=Frag')).toBeUndefined()
    })

    it('single slash', () => {
        expect(getSkSLProgramKind('/ kind=frag')).toBeUndefined()
    })

    it('many slash', () => {
        expect(getSkSLProgramKind('//// kind=frag')).toBe(SkSLProgramKind.kFrag)
    })

    it('colon', () => {
        expect(getSkSLProgramKind('// kind: frag')).toBe(SkSLProgramKind.kFrag)
    })
})
