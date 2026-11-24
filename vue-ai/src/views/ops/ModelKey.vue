<template>
  <el-card>
    <el-alert type="info" :closable="false" show-icon
              title="优先级：设备配置 > 客户配置 > 平台配置。Key 加密存储，查询只显示脱敏。" />

    <el-form :inline="true" class="filters">
      <el-form-item label="配置范围">
        <el-select v-model="query.scope" style="width: 140px">
          <el-option label="平台默认" :value="1" />
          <el-option label="客户" :value="2" />
          <el-option label="设备" :value="3" />
        </el-select>
      </el-form-item>
      <el-form-item v-if="query.scope === 2" label="客户ID">
        <el-input v-model="query.userId" placeholder="用户ID" style="width: 160px" />
      </el-form-item>
      <el-form-item v-if="query.scope === 3" label="设备ID">
        <el-input v-model="query.deviceId" placeholder="deviceId" style="width: 220px" />
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="loadConfig">查询</el-button>
      </el-form-item>
    </el-form>

    <el-form :model="form" label-width="110px" style="max-width: 620px">
      <el-form-item label="服务商">
        <el-select v-model="form.provider" style="width: 100%">
          <el-option label="DeepSeek" value="deepseek" />
          <el-option label="OpenAI" value="openai" />
          <el-option label="豆包（火山方舟）" value="doubao" />
        </el-select>
      </el-form-item>
      <el-form-item label="API Key">
        <el-input v-model="form.apiKey" type="password" show-password
                  :placeholder="form.apiKeyMasked || 'sk-...'" />
        <div class="hint" v-if="form.apiKeyMasked">当前：{{ form.apiKeyMasked }}（留空不修改）</div>
      </el-form-item>
      <el-form-item label="接口地址">
        <el-input v-model="form.baseUrl" placeholder="空=使用默认地址" />
      </el-form-item>
      <el-form-item label="模型名">
        <el-input v-model="form.model" placeholder="deepseek-chat / gpt-4o-mini / doubao-..." />
      </el-form-item>
      <el-form-item label="状态">
        <el-switch v-model="form.status" :active-value="1" :inactive-value="0" active-text="启用" />
      </el-form-item>
      <el-form-item>
        <el-button type="primary" :loading="saving" @click="save">保存配置</el-button>
      </el-form-item>
    </el-form>
  </el-card>
</template>

<script setup>
import { reactive, ref } from 'vue'
import { ElMessage } from 'element-plus'
import { modelConfigApi } from '../../api/admin'

const query = reactive({ scope: 1, userId: '', deviceId: '' })
const form = reactive({
  provider: 'deepseek',
  apiKey: '',
  apiKeyMasked: '',
  baseUrl: '',
  model: '',
  status: 1
})
const saving = ref(false)

async function loadConfig() {
  const data = await modelConfigApi.get({ ...query })
  if (!data) {
    Object.assign(form, { provider: 'deepseek', apiKey: '', apiKeyMasked: '', baseUrl: '', model: '', status: 1 })
    ElMessage.info('暂无配置')
    return
  }
  Object.assign(form, {
    provider: data.provider || 'deepseek',
    apiKey: '',
    apiKeyMasked: data.apiKeyMasked || '',
    baseUrl: data.baseUrl || '',
    model: data.model || '',
    status: data.status ?? 1
  })
}

async function save() {
  if (!form.apiKey && !form.apiKeyMasked) {
    ElMessage.warning('请填写 API Key')
    return
  }
  saving.value = true
  try {
    const body = {
      scope: query.scope,
      userId: query.scope === 2 ? query.userId : null,
      deviceId: query.scope === 3 ? query.deviceId : null,
      provider: form.provider,
      apiKey: form.apiKey || form.apiKeyMasked,
      baseUrl: form.baseUrl,
      model: form.model,
      status: form.status
    }
    const data = await modelConfigApi.save(body)
    form.apiKey = ''
    form.apiKeyMasked = data.apiKeyMasked
    ElMessage.success('已保存')
  } catch (e) {
    ElMessage.error(e.message || '保存失败')
  } finally {
    saving.value = false
  }
}
</script>

<style scoped>
.filters {
  margin: 16px 0;
}
.hint {
  color: #909399;
  font-size: 12px;
  margin-top: 4px;
}
</style>
