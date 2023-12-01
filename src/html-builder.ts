type Attributes = { [key: string]: string }
type Block = (builder: HtmlBuilder) => void
const emptyBlock: Block = () => {}

export class HtmlBuilder {
    public static build(block: (builder: HtmlBuilder) => void): string {
        const builder = new HtmlBuilder()
        builder.result += '<!DOCTYPE html>'
        block(builder)
        return builder.result
    }

    public element(tag: string, attributes: Attributes, block: Block = emptyBlock) {
        this.result += '<'
        this.result += tag

        for (const [key, value] of Object.entries(attributes)) {
            this.result += ' '
            this.result += key
            if (value.length > 0) {
                this.result += '='
                this.result += '"'
                this.result += value
                this.result += '"'
            }
        }

        this.result += '>'

        if (block) {
            block(this)
        }

        this.result += '</'
        this.result += tag
        this.result += '>'
    }

    public body(attributes: Attributes, block: Block = emptyBlock) {
        this.element('body', attributes, block)
    }

    public html(attributes: Attributes, block: Block = emptyBlock) {
        this.element('html', attributes, block)
    }

    public head(attributes: Attributes, block: Block = emptyBlock) {
        this.element('head', attributes, block)
    }

    public meta(attributes: Attributes, block: Block = emptyBlock) {
        this.element('meta', attributes, block)
    }

    public title(attributes: Attributes, block: Block = emptyBlock) {
        this.element('title', attributes, block)
    }

    public img(attributes: Attributes, block: Block = emptyBlock) {
        this.element('img', attributes, block)
    }

    public input(attributes: Attributes, block: Block = emptyBlock) {
        this.element('input', attributes, block)
    }

    public h1(attributes: Attributes, block: Block = emptyBlock) {
        this.element('h1', attributes, block)
    }

    public text(value: string) {
        this.result += value
    }

    private result: string = ''

    private constructor() {}
}
