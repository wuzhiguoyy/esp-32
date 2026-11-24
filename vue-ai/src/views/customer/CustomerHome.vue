<template>
  <div class="page">
    <el-card v-if="!token">
      <template #header>我的设备</template>
      <p class="tip">请在主站登录后，把登录 Token 粘贴到这里（仅本机保存）。</p>
      <el-input v-model="tokenInput" type="textarea" :rows="4" placeholder="eyJhbGciOi..." />
      <el-button type="primary" class="btn" @click="saveToken">进入</el-button>
    </el-card>

    <template v-else>
      <div class="bar">
        <h2>我的设备</h2>
        <el-button link type="primary" @click="logout">退出</el-button>
      </div>

      <el-tabs v-model="tab">
        <el-tab-pane label="绑定设备" name="bind">
          <el-card>
            <p class="tip">设备开机后屏幕会显示 6 位激活码，输入即可绑定。</p>
            <el-input v-model="code" placeholder="请输入设备屏幕上的激活码" style="max-width: 320px" />
            <el-button type="primary" class="btn" :loading="binding" @click="bind">绑定</el-button>
          </el-card>
        </el-tab-pane>

        <el-tab-pane label="我的设备" name="list">
          <el-card>
            <el-table :data="devices" v-loading="loading" border stripe>
              <el-table-column prop="deviceId" label="设备ID" min-width="180" />
              <el-table-column prop="chipModel" label="芯片" width="110" />
              <el-table-column label="状态" width="100">
                <template #default="{ row }">
                  <el-tag :type="row.status === 1 ? 'success' : 'info'">
                    {{ row.status === 1 ? '已激活' : '未激活' }}
                  </el-tag>
                </template>
              </el-table-column>
              <el-table-column label="在线" width="90">
                <template #default="{ row }">
                  <el-tag :type="row.onlineStatus === 1 ? 'success' : 'info'">
                    {{ row.onlineStatus === 1 ? '在线' : '离线' }}
                  </el-tag>
                </template>
              </el-table-column>
              <el-table-column prop="firmwareVer" label="固件" width="100" />
              <el-table-column prop="lastActiveAt" label="最近上线" min-width="170" />
              <el-table-column label="操作" width="100" fixed="right">
                <template #default="{ row }">
                  <el-button link type="danger" @click="unbind(row)">解绑</el-button>
                </template>
              </el-table-column>
            </el-table>
          </el-card>
        </el-tab-pane>

        <el-tab-pane label="我的模型Key" name="model">
          <el-card>
            <el-alert type="info" :closable="false" show-icon
                      title="不填则使用平台默认模型；填了就用你自己的 Key。" />
            <el-form :model="modelForm" label-width="110px" style="max-width: 560px; margin-top: 16px">
              <el-form-item label="服务商">
                <el-select v-model="modelForm.provider" style="width: 100%">
                  <el-option label="DeepSeek" value="deepseek" />
                  <el-option label="OpenAI" value="openai" />
                  <el-option label="豆包（火山方舟）" value="doubao" />
                </el-select>
              </el-form-item>
              <el-form-item label="API Key">
                <el-input v-model="modelForm.apiKey" type="password" show-password
                          :placeholder="modelForm.apiKeyMasked || 'sk-...'" />
                <div class="hint" v-if="modelForm.apiKeyMasked">当前：{{ modelForm.apiKeyMasked }}</div>
              </el-form-item>
              <el-form-item label="接口地址">
                <el-input v-model="modelForm.baseUrl" placeholder="空=默认" />
              </el-form-item>
              <el-form-item label="模型名">
                <el-input v-model="modelForm.model" placeholder="deepseek-chat" />
              </el-form-item>
              <el-form-item>
                <el-button type="primary" :loading="saving" @click="saveModel">保存</el-button>
              </el-form-item>
            </el-form>
          </el-card>
        </el-tab-pane>
      </el-tabs>
    </template>
  </div>
</template>

<script setup>
import { onMounted, reactive, ref } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { customerApi, getCustomerToken, setCustomerToken } from '../api/customer'

const token = ref(getCustomerToken())
const tokenInput = ref('')
const tab = ref('bind')
const code = ref('')
const binding = ref(false)
const loading = ref(false)
const devices = ref([])
const saving = ref(false)
const modelForm = reactive({ provider: 'deepseek', apiKey: '', apiKeyMasked: '', baseUrl: '', model: '' })

function saveToken() {
  if (!tokenInput.value.trim()) {
    ElMessage.warning('请粘贴 Token')
    return
  }
  setCustomerToken(tokenInput.value.trim())
  token.value = tokenInput.value.trim()
  loadDevices()
  loadModel()
}

function logout() {
  setCustomerToken('')
  token.value = ''
}

async function bind() {
  if (!code.value.trim()) {
    ElMessage.warning('请输入激活码')
    return
  }
  binding.value = true
  try {
    await customerApi.bind({ activationCode: code.value.trim() })
    ElMessage.success('绑定成功，设备将自动激活')
    code.value = ''
    tab.value = 'list'
    loadDevices()
  } catch (e) {
    ElMessage.error(e.message || '绑定失败')
  } finally {
    binding.value = false
  }
}

async function loadDevices() {
  loading.value = true
  try {
    const data = await customerApi.devices()
    devices.value = data?.list || []
  } catch (e) {
    ElMessage.error(e.message || '加载失败')
  } finally {
    loading.value = false
  }
}

async function unbind(row) {
  await ElMessageBox.confirm(`确认解绑设备 ${row.deviceId}？`, '提示', { type: 'warning' })
  await customerApi.unbind({ deviceId: row.deviceId })
  ElMessage.success('已解绑')
  loadDevices()
}

async function loadModel() {
  try {
    const data = await customerApi.getModelConfig()
    if (data) {
      Object.assign(modelForm, {
        provider: data.provider || 'deepseek',
        apiKey: '',
        apiKeyMasked: data.apiKeyMasked || '',
        baseUrl: data.baseUrl || '',
        model: data.model || ''
      })
    }
  } catch (e) {
    // 未配置忽略
  }
}

async function saveModel() {
  if (!modelForm.apiKey && !modelForm.apiKeyMasked) {
    ElMessage.warning('请填写 API Key')
    return
  }
  saving.value = true
  try {
    const data = await customerApi.saveModelConfig({
      provider: modelForm.provider,
      apiKey: modelForm.apiKey || modelForm.apiKeyMasked,
      baseUrl: modelForm.baseUrl,
      model: modelForm.model,
      status: 1
    })
    modelForm.apiKey = ''
    modelForm.apiKeyMasked = data?.apiKeyMasked || modelForm.apiKeyMasked
    ElMessage.success('已保存')
  } catch (e) {
    ElMessage.error(e.message || '保存失败')
  } finally {
    saving.value = false
  }
}

onMounted(() => {
  if (token.value) {
    loadDevices()
    loadModel()
  }
})
</script>

<style scoped>
.page {
  max-width: 1000px;
  margin: 24px auto;
  padding: 0 16px;
}
.bar {
  display: flex;
  justify-content: space-between;
  align-items: center;
}
.tip {
  color: #909399;
  font-size: 13px;
}
.btn {
  margin-top: 12px;
}
.hint {
  color: #909399;
  font-size: 12px;
}
</style>
