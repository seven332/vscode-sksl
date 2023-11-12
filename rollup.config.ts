import type { RollupOptions } from 'rollup'
import resolve from '@rollup/plugin-node-resolve'
import commonjs from '@rollup/plugin-commonjs'
import typescript from '@rollup/plugin-typescript'
import terser from '@rollup/plugin-terser'
import * as fs from 'fs'

const production = process.env.NODE_ENV == 'production'
const integrationTest = process.env.NODE_ENV == 'integration-test'

let input: string[]
if (integrationTest) {
    const entries = fs.readdirSync('integration-test')
    input = entries.filter((entry) => entry.endsWith('.test.ts')).map((entry) => `integration-test/${entry}`)
} else {
    input = ['src/index.ts', 'src/server.ts']
}

export default <RollupOptions>{
    input,
    output: {
        dir: integrationTest ? 'build_integration_test' : 'build',
        format: 'cjs',
        sourcemap: true,
    },
    plugins: [
        resolve({ preferBuiltins: true }),
        commonjs(),
        typescript({ tsconfig: './tsconfig.json' }),
        production && terser(),
    ].filter(Boolean),
    external: ['vscode'],
}
