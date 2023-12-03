import type { RollupOptions } from 'rollup'
import resolve from '@rollup/plugin-node-resolve'
import commonjs from '@rollup/plugin-commonjs'
import typescript from '@rollup/plugin-typescript'
import * as fs from 'fs'

export default <RollupOptions>{
    input: fs
        .readdirSync('src')
        .filter((entry) => entry.endsWith('.test.ts'))
        .map((entry) => `src/${entry}`),
    output: {
        dir: 'build',
        format: 'cjs',
        sourcemap: true,
    },
    plugins: [
        resolve({ preferBuiltins: true }),
        commonjs(),
        typescript({
            tsconfig: './tsconfig.json',
            include: ['./src/**/*.ts'],
        }),
    ],
    external: ['vscode'],
}
