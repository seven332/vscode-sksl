import * as child_process from 'child_process'
import * as os from 'os'
import * as path from 'path'

function canonicalize(file: string): string {
    return path.join(...file.split(path.posix.sep))
}

function npx(name: string): string {
    return `${name}${os.platform() == 'win32' ? '.cmd' : ''}`
}

function exec(cmd: string, ...args: string[]) {
    cmd = canonicalize(cmd)
    const result = child_process.spawnSync(cmd, args, { stdio: 'inherit' })
    if (result.status !== 0) {
        console.error('Failed to execute:', cmd, ...args, `(${result.error?.message})`)
        process.exit(-1)
    }
}

function run(target: string) {
    switch (target) {
        case 'format':
            exec(npx('prettier'), '--write', '.')
            break
        default:
            break
    }
}

run(process.argv[2])
