<script setup lang="ts">
import { ref } from 'vue'
import './vscode-webview'

let skslPath = ref('')

// TODO: message type

window.addEventListener('message', (event) => {
    const message = event.data
    skslPath.value = message
})

const vscode = acquireVsCodeApi()

function selectSkSL() {
    vscode.postMessage('Select SkSL')
}
</script>

<template>
    <div class="body">
        <h1>SkSL Runner</h1>
        <button @click="selectSkSL()">Select SkSL</button>
        <span>{{ skslPath }}</span>
    </div>
</template>

<style scoped>
.body {
    margin: 0 auto;
    max-width: 768px;
}
</style>
