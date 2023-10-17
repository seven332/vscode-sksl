import { BufferSubject } from './buffer-subject'

describe('BufferSubject', () => {
    describe('one by one', () => {
        it('fetch', async () => {
            const subject = new BufferSubject()
            subject.append(Uint8Array.from([1]))
            subject.append(Uint8Array.from([0]))
            subject.append(Uint8Array.from([0]))
            subject.append(Uint8Array.from([0]))
            subject.append(Uint8Array.from([8]))
            expect(await subject.next()).toStrictEqual(Uint8Array.from([8]))
            subject.append(Uint8Array.from([2]))
            subject.append(Uint8Array.from([0]))
            subject.append(Uint8Array.from([0]))
            subject.append(Uint8Array.from([0]))
            subject.append(Uint8Array.from([8]))
            subject.append(Uint8Array.from([9]))
            expect(await subject.next()).toStrictEqual(Uint8Array.from([8, 9]))
        })

        it('request', async () => {
            const subject = new BufferSubject()
            const p1 = subject.next()
            const p2 = subject.next()
            subject.append(Uint8Array.from([1]))
            subject.append(Uint8Array.from([0]))
            subject.append(Uint8Array.from([0]))
            subject.append(Uint8Array.from([0]))
            subject.append(Uint8Array.from([8]))
            subject.append(Uint8Array.from([2]))
            subject.append(Uint8Array.from([0]))
            subject.append(Uint8Array.from([0]))
            subject.append(Uint8Array.from([0]))
            subject.append(Uint8Array.from([8]))
            subject.append(Uint8Array.from([9]))
            expect(await p1).toStrictEqual(Uint8Array.from([8]))
            expect(await p2).toStrictEqual(Uint8Array.from([8, 9]))
        })
    })

    describe('two buffer', () => {
        it('fetch', async () => {
            const subject = new BufferSubject()
            subject.append(Uint8Array.from([1, 0, 0, 0, 8, 2, 0, 0, 0, 8, 9]))
            expect(await subject.next()).toStrictEqual(Uint8Array.from([8]))
            expect(await subject.next()).toStrictEqual(Uint8Array.from([8, 9]))
        })

        it('request', async () => {
            const subject = new BufferSubject()
            const p1 = subject.next()
            const p2 = subject.next()
            subject.append(Uint8Array.from([1, 0, 0, 0, 8, 2, 0, 0, 0, 8, 9]))
            expect(await p1).toStrictEqual(Uint8Array.from([8]))
            expect(await p2).toStrictEqual(Uint8Array.from([8, 9]))
        })

        it('half', async () => {
            const subject = new BufferSubject()
            const p1 = subject.next()
            const p2 = subject.next()
            subject.append(Uint8Array.from([1, 0, 0]))
            subject.append(Uint8Array.from([0, 8, 2, 0, 0, 0, 8, 9]))
            expect(await p1).toStrictEqual(Uint8Array.from([8]))
            expect(await p2).toStrictEqual(Uint8Array.from([8, 9]))
        })
    })
})
