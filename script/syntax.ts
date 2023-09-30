import * as path from 'path'
import * as fs from 'fs'

const constant = {
    language: ['true', 'false'],
}

const keyword = {
    control: [
        'if',
        'else',
        'for',
        'while',
        'do',
        'switch',
        'case',
        'default',
        'break',
        'continue',
        'discard',
        'return',
    ],
}

const storage = {
    type: [
        'struct',
        'float',
        'float2',
        'float3',
        'float4',
        'half',
        'half2',
        'half3',
        'half4',
        'int',
        'int2',
        'int3',
        'int4',
        'uint',
        'uint2',
        'uint3',
        'uint4',
        'short',
        'short2',
        'short3',
        'short4',
        'ushort',
        'ushort2',
        'ushort3',
        'ushort4',
        'bool',
        'bool2',
        'bool3',
        'bool4',
        'void',
        'float2x2',
        'float2x3',
        'float2x4',
        'float3x2',
        'float3x3',
        'float3x4',
        'float4x2',
        'float4x3',
        'float4x4',
        'half2x2',
        'half2x3',
        'half2x4',
        'half3x2',
        'half3x3',
        'half3x4',
        'half4x2',
        'half4x3',
        'half4x4',
        'vec2',
        'vec3',
        'vec4',
        'ivec2',
        'ivec3',
        'ivec4',
        'bvec2',
        'bvec3',
        'bvec4',
        'mat2',
        'mat3',
        'mat4',
        'mat2x2',
        'mat2x3',
        'mat2x4',
        'mat3x2',
        'mat3x3',
        'mat3x4',
        'mat4x2',
        'mat4x3',
        'mat4x4',
        'texture2D',
        'textureExternalOES',
        'texture2DRect',
        'readWriteTexture2D',
        'readonlyTexture2D',
        'writeonlyTexture2D',
        'sampler2D',
        'samplerExternalOES',
        'sampler2DRect',
        'sampler',
        'subpassInput',
        'subpassInputMS',
        'colorFilter',
        'shader',
        'blender',
        'atomicUint',
    ],
    dollarType: [
        'genTexture2D',
        'readableTexture2D',
        'writableTexture2D',
        'genType',
        'genHType',
        'genIType',
        'genUType',
        'genBType',
        'mat',
        'hmat',
        'squareMat',
        'squareHMat',
        'vec',
        'hvec',
        'ivec',
        'uvec',
        'svec',
        'usvec',
        'bvec',
    ],
    modifier: [
        'in',
        'out',
        'inout',
        'uniform',
        'const',
        'flat',
        'noperspective',
        'inline',
        'noinline',
        'readonly',
        'writeonly',
        'buffer',
        'layout',
        'highp',
        'mediump',
        'lowp',
        'workgroup',
    ],
    dollarModifier: ['pure', 'es3', 'export'],
}

const grammar = {
    name: 'SkSL',
    scopeName: 'source.sksl',
    fileTypes: ['sksl'],
    patterns: [
        {
            name: 'comment.line.double-slash.sksl',
            begin: '\\/\\/',
            end: '$',
        },
        {
            name: 'comment.block.sksl',
            begin: '\\s*+(\\/\\*)',
            end: '\\*\\/',
        },
        {
            name: 'constant.numeric.sksl',
            match: '[0-9]*\\.[0-9]+([eE][+-]?[0-9]+)?|[0-9]+\\.[0-9]*([eE][+-]?[0-9]+)?|[0-9]+([eE][+-]?[0-9]+)',
        },
        {
            name: 'constant.numeric.sksl',
            match: '([1-9][0-9]*|0[0-7]*|0[xX][0-9a-fA-F]+)[uU]?',
        },
        {
            name: 'constant.language.sksl',
            match: `\\b(${constant.language.join('|')})\\b`,
        },
        {
            name: 'keyword.control.sksl',
            match: `\\b(${keyword.control.join('|')})\\b`,
        },
        {
            name: 'storage.type.sksl',
            match: `\\b(${storage.type.join('|')})\\b`,
        },
        {
            name: 'storage.type.sksl',
            match: `\\$\\b(${storage.dollarType.join('|')})\\b`,
        },
        {
            name: 'storage.modifier.sksl',
            match: `\\b(${storage.modifier.join('|')})\\b`,
        },
        {
            name: 'storage.modifier.sksl',
            match: `\\$\\b(${storage.dollarModifier.join('|')})\\b`,
        },
    ],
}

const language = {
    comments: {
        lineComment: '//',
        blockComment: ['/*', '*/'],
    },
    brackets: [
        ['{', '}'],
        ['[', ']'],
        ['(', ')'],
    ],
    autoClosingPairs: [
        { open: '{', close: '}' },
        { open: '[', close: ']' },
        { open: '(', close: ')' },
        { open: "'", close: "'", notIn: ['string', 'comment'] },
        { open: '"', close: '"', notIn: ['string'] },
        { open: '`', close: '`', notIn: ['string', 'comment'] },
        { open: '/**', close: ' */', notIn: ['string'] },
    ],
    surroundingPairs: [
        ['{', '}'],
        ['[', ']'],
        ['(', ')'],
        ["'", "'"],
        ['"', '"'],
        ['`', '`'],
    ],
}

const output = process.argv[2]
fs.mkdirSync(output, { recursive: true })
fs.writeFileSync(path.join(output, 'grammar.json'), JSON.stringify(grammar))
fs.writeFileSync(path.join(output, 'language.json'), JSON.stringify(language))
