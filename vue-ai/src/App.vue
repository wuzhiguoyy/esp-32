<template>
  <router-view />
  <Watermark />
</template>

<script setup>
import { onMounted } from 'vue'
import Watermark from './components/Watermark.vue'

onMounted(() => {
  // 轻度保护：禁用右键菜单与拖拽，减少直接复制
  document.addEventListener('contextmenu', (e) => e.preventDefault())
  document.addEventListener('dragstart', (e) => e.preventDefault())
})
</script>

<style>
html,
body,
#app {
  height: 100%;
  margin: 0;
  font-family: -apple-system, BlinkMacSystemFont, 'PingFang SC', 'Microsoft YaHei', sans-serif;
  background: #f5f7fa;
}

body {
  -webkit-user-select: none;
  user-select: none;
}
</style>
