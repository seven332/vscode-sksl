export enum SkSLProgramKind {
    kShader = 'shader',
    kColorFilter = 'colorfilter',
    kBlender = 'blender',
    kMeshFrag = 'meshfrag',
    kMeshVert = 'meshvert',
}

export const getSkSLProgramKind = (content: string): SkSLProgramKind | undefined => {
    const regex = new RegExp(
        `^[ \\t]*\\/\\/[ /\\t]*kind[ \\t]*[:=][ \\t]*(${Object.values(SkSLProgramKind).join('|')})`,
    )
    const match = content.match(regex)
    if (match) {
        return match[1] as SkSLProgramKind
    }
}
