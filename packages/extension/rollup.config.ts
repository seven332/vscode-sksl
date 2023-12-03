import type { RollupOptions } from 'rollup'
import resolve from '@rollup/plugin-node-resolve'
import commonjs from '@rollup/plugin-commonjs'
import typescript from '@rollup/plugin-typescript'
import terser from '@rollup/plugin-terser'

export default <RollupOptions>{
    input: ['src/index.ts', 'src/server.ts'],
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
            include: ['./src/**/*.ts', '../*/src/**/*.ts'],
        }),
        terser(),
    ],
    external: ['vscode'],
}
