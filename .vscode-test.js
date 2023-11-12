const { defineConfig } = require('@vscode/test-cli')

module.exports = defineConfig({
    files: 'build_integration_test/*.test.js',
    mocha: {
        timeout: 20000,
    },
})
