export enum ProgramKind {
    kFrag = 'frag',
    kVert = 'vert',
    kCompute = 'compute',
    kShader = 'shader',
    kColorFilter = 'colorfilter',
    kBlender = 'blender',
    kMeshVert = 'mesh-vert',
    kMeshFrag = 'mesh-frag',
}

export const getProgramKind = (content: string): ProgramKind | undefined => {
    const regex = new RegExp(`[ \\t]*\\/\\/[ /\\t]*kind[ \\t]*[:=][ \\t]*(${Object.values(ProgramKind).join('|')})`)
    const match = content.match(regex)
    if (match) {
        return match[1] as ProgramKind
    }
}
