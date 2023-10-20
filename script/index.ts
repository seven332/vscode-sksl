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

function bundle(debug: boolean) {
    const environment = debug ? 'development' : 'production'
    chdir('.')
    exec(
        npx('rollup'),
        '--config',
        '--configPlugin=typescript={module:"esnext",exclude:"wasm/**/*.ts"}',
        `--environment=NODE_ENV:${environment}`,
    )
}

function run(target: string) {
    switch (target) {
        case 'build':
            chdir('.')
            exec('rm', '-rf', 'build')
            bundle(false)
            chdir('script')
            exec(npx('ts-node'), 'syntax.ts', '../build')
            chdir('wasm')
            exec('sh', 'build.sh')
            break
        case 'test':
            chdir('.')
            exec(npx('jest'))
            chdir('wasm')
            exec('sh', 'test.sh')
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
