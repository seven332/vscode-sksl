import * as ls from 'vscode-languageserver/node'

export class FilePosition {
    public constructor(content: string) {
        if (content.length <= 0) {
            return
        }
        this.positions.push(0)

        let index = 0
        let prev = ' '
        for (const c of content) {
            switch (c) {
                case '\n':
                    if (prev == '\n') {
                        this.positions.push(index)
                    }
                    break
                default:
                    if (prev == '\r' || prev == '\n') {
                        this.positions.push(index)
                    }
                    break
            }
            prev = c
            index += 1
        }
    }

    public getPosition(offset: number): ls.Position {
        const index = this.findLastIndex((position) => position <= offset)
        if (index == -1) {
            return ls.Position.create(0, offset)
        }
        const position = this.positions[index]
        return ls.Position.create(index, offset - position)
    }

    private positions = new Array<number>()

    private findLastIndex(predicate: (position: number) => boolean): number {
        let i = this.positions.length - 1
        while (i >= 0) {
            if (predicate(this.positions[i])) {
                return i
            }
            i -= 1
        }
        return -1
    }
}
