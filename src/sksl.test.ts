import { ProgramKind, getProgramKind } from './sksl'

describe('getProgramKind', () => {
    it('right', () => {
        expect(getProgramKind('// kind=frag')).toBe(ProgramKind.kFrag)
        expect(getProgramKind('// kind=vert')).toBe(ProgramKind.kVert)
        expect(getProgramKind('// kind=compute')).toBe(ProgramKind.kCompute)
        expect(getProgramKind('// kind=shader')).toBe(ProgramKind.kShader)
        expect(getProgramKind('// kind=colorfilter')).toBe(ProgramKind.kColorFilter)
        expect(getProgramKind('// kind=blender')).toBe(ProgramKind.kBlender)
        expect(getProgramKind('// kind=mesh-vert')).toBe(ProgramKind.kMeshVert)
        expect(getProgramKind('// kind=mesh-frag')).toBe(ProgramKind.kMeshFrag)
    })

    it('case sensitive', () => {
        expect(getProgramKind('// kind=Frag')).toBeUndefined()
    })

    it('single slash', () => {
        expect(getProgramKind('/ kind=frag')).toBeUndefined()
    })

    it('many slash', () => {
        expect(getProgramKind('//// kind=frag')).toBe(ProgramKind.kFrag)
    })

    it('colon', () => {
        expect(getProgramKind('// kind: frag')).toBe(ProgramKind.kFrag)
    })
})
