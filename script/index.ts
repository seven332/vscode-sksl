import * as child_process from 'child_process'
import * as os from 'os'
import * as path from 'path'
import * as fs from 'fs'

function canonicalize(file: string): string {
    return path.join(...file.split(path.posix.sep))
}

function npx(name: string): string {
    return `${name}${os.platform() == 'win32' ? '.cmd' : ''}`
}

function list(dir: string, regex: RegExp): string[] {
    dir = canonicalize(dir)
    const entries = fs.readdirSync(dir)
    return entries.filter((entry) => entry.match(regex)).map((entry) => path.join(dir, entry))
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

enum RollupKind {
    Production = 'production',
    Development = 'development',
    IntegrationTest = 'integration-test',
}

function rollup(kind: RollupKind) {
    chdir('.')
    exec(npx('rollup'), '--config', '--configPlugin=typescript', `--environment=NODE_ENV:${kind}`)
}

function run(target: string) {
    switch (target) {
        case 'prepare':
            chdir('wasm')
            exec('bash', 'prepare.sh')
            break
        case 'build':
            chdir('.')
            exec('rm', '-rf', 'build')
            chdir('wasm')
            exec('bash', 'build.sh')
            exec('cp', 'build/src/sksl-wasm.js', '../src')
            rollup(RollupKind.Production)
            chdir('script')
            exec(npx('ts-node'), 'syntax.ts', '../build')
            chdir('wasm')
            exec('cp', 'build/src/sksl-wasm.wasm', '../build')
            break
        case 'test':
            // js test
            chdir('.')
            exec(npx('jest'))
            // wasm test
            chdir('wasm')
            exec('bash', 'test.sh')
            // integration test
            chdir('.')
            exec('rm', '-rf', 'build_integration_test')
            rollup(RollupKind.IntegrationTest)
            chdir('.')
            exec(npx('vscode-test'))
            break
        case 'format':
            exec(npx('prettier'), '--write', '.')
            exec('clang-format', '-i', ...list('wasm/src', /\.(cpp|h)$/))
            exec('clang-format', '-i', ...list('wasm/src/action', /\.(cpp|h)$/))
            break
        case 'package':
            chdir('.')
            exec(npx('vsce'), 'package', '--out', 'sksl.vsix')
            break
        default:
            break
    }
}

run(process.argv[2])
