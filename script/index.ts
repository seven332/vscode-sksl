import * as child_process from 'child_process'
import * as path from 'path'
import * as fs from 'fs'

function canonicalize(file: string): string {
    return path.join(...file.split(path.posix.sep))
}

function chdir(dir: string) {
    dir = canonicalize(dir)
    process.chdir(path.join(__dirname, '..', dir))
}

function exec(cmd: string, ...args: string[]) {
    cmd = canonicalize(cmd)
    const result = child_process.spawnSync(cmd, args, { stdio: 'inherit' })
    if (result.status !== 0) {
        console.error('Failed to execute:', cmd, ...args, `(${result.error?.message})`)
        process.exit(-1)
    }
}

function pnpm_exec(cmd: string, ...args: string[]) {
    exec('pnpm', 'exec', cmd, ...args)
}

function list(dir: string, regex: RegExp): string[] {
    dir = canonicalize(dir)
    const entries = fs.readdirSync(dir)
    return entries.filter((entry) => entry.match(regex)).map((entry) => path.join(dir, entry))
}

function run(target: string) {
    switch (target) {
        case 'prepare':
            chdir('packages/wasm/c++')
            exec('bash', 'prepare.sh')
            break
        case 'build':
            // wasm
            chdir('packages/wasm/c++')
            exec('bash', 'build.sh')
            exec('cp', 'build/src/sksl-wasm.js', '../src')
            // runner-ui
            chdir('packages/runner-ui')
            exec('rm', '-rf', 'dist')
            exec('pnpm', 'build')
            // extension
            chdir('packages/extension')
            exec('rm', '-rf', 'build')
            pnpm_exec('rollup', '--config', '--configPlugin=typescript')
            exec('cp', '../wasm/c++/build/src/sksl-wasm.wasm', 'build')
            exec('cp', '-R', '../runner-ui/dist', 'build/runner-ui')
            chdir('.')
            exec('ts-node', 'script/syntax.ts', 'packages/extension/build')
            break
        case 'test':
            // js test
            chdir('.')
            exec('jest')
            // wasm test
            chdir('packages/wasm/c++')
            exec('bash', 'test.sh')
            // integration test
            chdir('packages/integration-test')
            exec('rm', '-rf', 'build')
            pnpm_exec('rollup', '--config', '--configPlugin=typescript')
            chdir('packages/extension')
            pnpm_exec('vscode-test')
            break
        case 'format':
            exec('prettier', '--write', '.')
            exec('clang-format', '-i', ...list('packages/wasm/c++/src', /\.(cpp|h)$/))
            exec('clang-format', '-i', ...list('packages/wasm/c++/src/action', /\.(cpp|h)$/))
            break
        case 'package':
            chdir('packages/extension')
            exec('cp', '-f', '../../LICENSE', '.')
            exec('cp', '-f', '../../README.md', '.')
            pnpm_exec('vsce', 'package', '--no-dependencies', '--out', 'sksl.vsix')
            break
    }
}

run(process.argv[2])
