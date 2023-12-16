<script setup lang="ts">
import { ref } from 'vue'
import './vscode-webview'
import {
    type Message,
    MessageType,
    pipe,
    type SelectSkSLRequestMessage,
    type SelectSkSLResponseMessage,
} from '@workspace/runner-data'

let skslPath = ref('')

window.addEventListener('message', (event) => {
    const message = event.data as Message
    switch (message.type) {
        case MessageType.kSelectSkSL:
            skslPath.value = (message as SelectSkSLResponseMessage).path
            break
    }
})

const vscode = acquireVsCodeApi()

function selectSkSL() {
    vscode.postMessage(pipe<SelectSkSLRequestMessage>({ type: MessageType.kSelectSkSL }))
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
