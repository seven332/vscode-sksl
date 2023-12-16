<script setup lang="ts">
import * as Vue from 'vue'
import './vscode-webview'
import {
    type Message,
    MessageType,
    pipe,
    type SelectSkSLRequest,
    type SelectSkSLResponse,
    type SkSLUniform,
    type GetUniformsResponse,
} from '@workspace/runner-data'

interface Uniform {
    uniform: SkSLUniform
    value: Vue.Ref<string>
}

let path = Vue.ref('')
let uniforms = Vue.ref([] as Uniform[])

window.addEventListener('message', (event) => {
    const message = event.data as Message
    switch (message.type) {
        case MessageType.kSelectSkSL:
            path.value = (message as SelectSkSLResponse).path
            break
        case MessageType.kGetUniforms:
            uniforms.value = []
            for (const uniform of (message as GetUniformsResponse).uniforms) {
                uniforms.value.push({ uniform: uniform, value: '' })
            }
            console.log(uniforms)
            break
    }
})

const vscode = acquireVsCodeApi()

function selectSkSL() {
    vscode.postMessage(pipe<SelectSkSLRequest>({ type: MessageType.kSelectSkSL }))
}
</script>

<template>
    <div class="body">
        <h1>SkSL Runner</h1>
        <button @click="selectSkSL()">Select SkSL</button>
        <span>{{ path }}</span>
        <li v-for="uniform in uniforms">
            {{ uniform.uniform.name }} - {{ uniform.uniform.type }}
            <input type="text" v-model="uniform.value" />
        </li>
    </div>
</template>

<style scoped>
.body {
    margin: 0 auto;
    max-width: 768px;
}
</style>
