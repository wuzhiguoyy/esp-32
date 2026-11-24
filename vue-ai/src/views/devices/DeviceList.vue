<template>
  <el-card>
    <el-form :inline="true" class="filters">
      <el-form-item label="关键字">
        <el-input v-model="query.keyword" placeholder="设备ID / ClientId / MAC" clearable @keyup.enter="reload" />
      </el-form-item>
      <el-form-item label="状态">
        <el-select v-model="query.status" placeholder="全部" clearable style="width: 140px">
          <el-option label="已激活" :value="1" />
          <el-option label="未激活" :value="3" />
          <el-option label="禁用" :value="0" />
          <el-option label="停用" :value="2" />
        </el-select>
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="reload">查询</el-button>
        <el-button @click="reset">重置</el-button>
      </el-form-item>
    </el-form>

    <el-table :data="rows" v-loading="loading" border stripe>
      <el-table-column prop="deviceId" label="设备ID" min-width="170" />
      <el-table-column prop="chipModel" label="芯片" width="110" />
      <el-table-column prop="boardType" label="板型" width="150" />
      <el-table-column label="状态" width="90">
        <template #default="{ row }">
          <el-tag :type="statusType(row.status)">{{ statusText(row.status) }}</el-tag>
        </template>
      </el-table-column>
      <el-table-column label="在线" width="80">
        <template #default="{ row }">
          <el-tag :type="row.onlineStatus === 1 ? 'success' : 'info'">
            {{ row.onlineStatus === 1 ? '在线' : '离线' }}
          </el-tag>
        </template>
      </el-table-column>
      <el-table-column label="绑定" width="90">
        <template #default="{ row }">{{ row.bindStatus === 1 ? '已绑定' : '未绑定' }}</template>
      </el-table-column>
      <el-table-column prop="activationCode" label="激活码" width="110" />
      <el-table-column prop="lastActiveAt" label="最近上线" min-width="170" />
      <el-table-column label="操作" width="220" fixed="right">
        <template #default="{ row }">
          <el-button link type="primary" @click="openBind(row)">绑定</el-button>
          <el-button link type="warning" @click="unbind(row)">解绑</el-button>
          <el-button link :type="row.status === 0 ? 'success' : 'danger'" @click="toggle(row)">
            {{ row.status === 0 ? '启用' : '禁用' }}
          </el-button>
        </template>
      </el-table-column>
    </el-table>

    <el-pagination class="pager" background layout="total, sizes, prev, pager, next" :total="total"
                   v-model:current-page="query.pageNum" v-model:page-size="query.pageSize"
                   :page-sizes="[10, 20, 50, 100]" @current-change="load" @size-change="reload" />
  </el-card>

  <el-dialog v-model="bindVisible" title="绑定用户" width="420px">
    <el-form label-width="90px">
      <el-form-item label="设备ID">
        <el-input v-model="bindForm.deviceId" disabled />
      </el-form-item>
      <el-form-item label="用户ID">
        <el-input v-model="bindForm.userId" placeholder="数字用户ID" />
      </el-form-item>
    </el-form>
    <template #footer>
      <el-button @click="bindVisible = false">取消</el-button>
      <el-button type="primary" @click="submitBind">确定</el-button>
    </template>
  </el-dialog>
</template>

<script setup>
import { onMounted, reactive, ref } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { deviceApi } from '../../api/admin'
import { usePage } from '../../composables/usePage'

const { loading, rows, total, query, load, reload } = usePage(deviceApi.page, {
  keyword: '',
  status: null
})

const bindVisible = ref(false)
const bindForm = reactive({ deviceId: '', userId: '' })

const statusText = (s) => ({ 0: '禁用', 1: '已激活', 2: '停用', 3: '未激活' }[s] || '-')
const statusType = (s) => ({ 0: 'danger', 1: 'success', 2: 'warning', 3: 'info' }[s] || 'info')

function reset() {
  query.keyword = ''
  query.status = null
  reload()
}

function openBind(row) {
  bindForm.deviceId = row.deviceId
  bindForm.userId = ''
  bindVisible.value = true
}

async function submitBind() {
  if (!bindForm.userId) {
    ElMessage.warning('请填写用户ID')
    return
  }
  await deviceApi.bind({ deviceId: bindForm.deviceId, userId: String(bindForm.userId) })
  ElMessage.success('绑定成功')
  bindVisible.value = false
  load()
}

async function unbind(row) {
  const { value } = await ElMessageBox.prompt('请输入要解绑的用户ID', '解绑设备', {
    inputPlaceholder: '用户ID',
    inputValidator: (v) => (!!v && /^\d+$/.test(v)) || '请输入数字用户ID'
  })
  await deviceApi.unbind({ deviceId: row.deviceId, userId: value })
  ElMessage.success('已解绑')
  load()
}

async function toggle(row) {
  const next = row.status === 0 ? 1 : 0
  await deviceApi.status({ deviceId: row.deviceId, status: next, remark: '后台操作' })
  ElMessage.success('状态已更新')
  load()
}

onMounted(load)
</script>

<style scoped>
.filters {
  margin-bottom: 8px;
}
.pager {
  margin-top: 16px;
  justify-content: flex-end;
}
</style>
