import { HtmlBuilder } from './html-builder'

describe('HtmlBuilder', () => {
    it('build', () => {
        const html = HtmlBuilder.build((b) => {
            b.html({
                lang: 'en',
            })
        })
        expect(html).toBe('<!DOCTYPE html><html lang="en"></html>')
    })
})
