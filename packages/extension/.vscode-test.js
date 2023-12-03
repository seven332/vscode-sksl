const { defineConfig } = require('@vscode/test-cli')

module.exports = defineConfig({
    files: '../integration-test/build/*.test.js',
    mocha: {
        timeout: 20000,
    },
})
