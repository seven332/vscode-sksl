import * as vscode from 'vscode'
import { HtmlBuilder } from './html-builder'

export function showRunner() {
    const panel = vscode.window.createWebviewPanel('sksl.runner', 'SkSL Runner', vscode.ViewColumn.Beside, {})
    panel.webview.html = getWebviewContent()
}

function getWebviewContent() {
    return HtmlBuilder.build((_) => {
        _.html({ lang: 'en' }, () => {
            _.head({}, () => {
                _.meta({ charset: 'UTF-8' })
                _.meta({ name: 'viewport', content: 'width=device-width, initial-scale=1.0' })
                _.title({}, () => {
                    _.text('SkSL Runner')
                })
            })
            _.body({}, () => {
                _.h1({}, () => {
                    _.text('SkSL Runner')
                })
            })
        })
    })
}
