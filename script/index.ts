import * as child_process from 'child_process'
import * as os from 'os'
import * as path from 'path'

function canonicalize(file: string): string {
    return path.join(...file.split(path.posix.sep))
}

function npx(name: string): string {
    return `${name}${os.platform() == 'win32' ? '.cmd' : ''}`
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
        '--configPlugin=typescript={module:"esnext"}',
        `--environment=NODE_ENV:${environment}`,
    )
}

function run(target: string) {
    switch (target) {
        case 'build':
            bundle(false)
            chdir('script')
            exec(npx('ts-node'), 'syntax.ts', '../build')
            chdir('wasi')
            exec('sh', 'build.sh')
            break
        case 'format':
            exec(npx('prettier'), '--write', '.')
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
