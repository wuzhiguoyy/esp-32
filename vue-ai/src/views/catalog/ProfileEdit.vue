<template>
  <el-card>
    <el-alert type="info" :closable="false" show-icon
              title="设备级配置优先于产品级配置。mode=translate 走翻译，mode=chat 走对话。" />

    <el-form :inline="true" class="filters">
      <el-form-item label="设备ID">
        <el-input v-model="query.deviceId" placeholder="设备 deviceId" clearable style="width: 220px" />
      </el-form-item>
      <el-form-item label="产品ID">
        <el-input v-model="query.productId" placeholder="产品主键" clearable style="width: 160px" />
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="loadProfile">查询</el-button>
      </el-form-item>
    </el-form>

    <el-form :model="form" label-width="120px" style="max-width: 640px">
      <el-form-item label="运行模式">
        <el-select v-model="form.mode" style="width: 100%">
          <el-option label="对话 chat" value="chat" />
          <el-option label="翻译 translate" value="translate" />
          <el-option label="故事 story" value="story" />
        </el-select>
      </el-form-item>
      <el-form-item label="音色"><el-input v-model="form.voice" placeholder="zh-CN-XiaoxiaoNeural" /></el-form-item>
      <el-form-item label="语言"><el-input v-model="form.language" placeholder="zh-CN / en-US" /></el-form-item>
      <el-form-item label="音量">
        <el-slider v-model="form.volume" :min="0" :max="100" />
      </el-form-item>
      <el-form-item label="语速">
        <el-slider v-model="form.speed" :min="0" :max="100" />
      </el-form-item>
      <el-form-item label="唤醒词"><el-input v-model="form.wakeWord" /></el-form-item>
      <el-form-item label="扩展配置">
        <el-input v-model="form.configJson" type="textarea" :rows="4" placeholder='{"key":"value"}' />
      </el-form-item>
      <el-form-item label="状态">
        <el-switch v-model="form.status" :active-value="1" :inactive-value="0" active-text="启用" />
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="save">保存配置</el-button>
      </el-form-item>
    </el-form>
  </el-card>
</template>

<script setup>
import { reactive, ref } from 'vue'
import { ElMessage } from 'element-plus'
import { profileApi } from '../../api/admin'

const query = reactive({ deviceId: '', productId: '' })
const form = reactive({
  id: '',
  deviceId: '',
  productId: '',
  mode: 'chat',
  voice: '',
  language: 'zh-CN',
  volume: 60,
  speed: 50,
  wakeWord: '',
  configJson: '',
  status: 1
})

async function loadProfile() {
  if (!query.deviceId && !query.productId) {
    ElMessage.warning('设备ID 或 产品ID 至少填一个')
    return
  }
  const data = await profileApi.get({ deviceId: query.deviceId, productId: query.productId })
  if (!data) {
    Object.assign(form, {
      id: '',
      deviceId: query.deviceId,
      productId: query.productId,
      mode: 'chat',
      voice: '',
      language: 'zh-CN',
      volume: 60,
      speed: 50,
      wakeWord: '',
      configJson: '',
      status: 1
    })
    ElMessage.info('暂无配置，可直接新增')
    return
  }
  Object.assign(form, {
    id: data.id,
    deviceId: data.deviceId || '',
    productId: data.productId || '',
    mode: data.mode || 'chat',
    voice: data.voice || '',
    language: data.language || 'zh-CN',
    volume: data.volume ?? 60,
    speed: data.speed ?? 50,
    wakeWord: data.wakeWord || '',
    configJson: data.configJson || '',
    status: data.status ?? 1
  })
}

async function save() {
  if (!form.deviceId && !form.productId) {
    ElMessage.warning('设备ID 或 产品ID 至少填一个')
    return
  }
  await profileApi.save({ ...form })
  ElMessage.success('配置已保存')
}

defineExpose({ loadProfile })
</script>

<style scoped>
.filters {
  margin: 16px 0;
}
</style>
