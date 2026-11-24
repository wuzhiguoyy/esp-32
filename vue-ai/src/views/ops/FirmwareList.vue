<template>
  <el-card>
    <el-form :inline="true" class="filters">
      <el-form-item label="版本">
        <el-input v-model="query.keyword" placeholder="版本号" clearable />
      </el-form-item>
      <el-form-item label="芯片">
        <el-input v-model="query.chipModel" placeholder="ESP32-S3" clearable style="width: 140px" />
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="reload">查询</el-button>
        <el-button type="success" @click="openCreate">新增固件</el-button>
      </el-form-item>
    </el-form>

    <el-table :data="rows" v-loading="loading" border stripe>
      <el-table-column prop="version" label="版本" width="110" />
      <el-table-column prop="chipModel" label="芯片" width="120" />
      <el-table-column prop="boardType" label="板型" min-width="150" />
      <el-table-column prop="fileUrl" label="下载地址" min-width="240" show-overflow-tooltip />
      <el-table-column label="状态" width="100">
        <template #default="{ row }">
          <el-tag :type="statusType(row.status)">{{ statusText(row.status) }}</el-tag>
        </template>
      </el-table-column>
      <el-table-column prop="publishTime" label="发布时间" min-width="170" />
      <el-table-column label="操作" width="100" fixed="right">
        <template #default="{ row }">
          <el-button link type="primary" @click="openEdit(row)">编辑</el-button>
        </template>
      </el-table-column>
    </el-table>

    <el-pagination class="pager" background layout="total, sizes, prev, pager, next" :total="total"
                   v-model:current-page="query.pageNum" v-model:page-size="query.pageSize"
                   :page-sizes="[10, 20, 50]" @current-change="load" @size-change="reload" />
  </el-card>

  <el-dialog v-model="visible" :title="form.id ? '编辑固件' : '新增固件'" width="560px">
    <el-form :model="form" label-width="110px">
      <el-form-item label="版本号"><el-input v-model="form.version" placeholder="1.0.3" /></el-form-item>
      <el-form-item label="芯片型号"><el-input v-model="form.chipModel" placeholder="ESP32-S3" /></el-form-item>
      <el-form-item label="板型"><el-input v-model="form.boardType" placeholder="空=全部" /></el-form-item>
      <el-form-item label="下载地址"><el-input v-model="form.fileUrl" /></el-form-item>
      <el-form-item label="文件大小"><el-input v-model="form.fileSize" placeholder="byte" /></el-form-item>
      <el-form-item label="更新说明"><el-input v-model="form.releaseNote" type="textarea" :rows="3" /></el-form-item>
      <el-form-item label="状态">
        <el-select v-model="form.status" style="width: 100%">
          <el-option label="草稿" :value="0" />
          <el-option label="灰度中" :value="1" />
          <el-option label="全量发布" :value="2" />
          <el-option label="已下线" :value="3" />
        </el-select>
      </el-form-item>
    </el-form>
    <template #footer>
      <el-button @click="visible = false">取消</el-button>
      <el-button type="primary" @click="save">保存</el-button>
    </template>
  </el-dialog>
</template>

<script setup>
import { onMounted, reactive, ref } from 'vue'
import { ElMessage } from 'element-plus'
import { firmwareApi } from '../../api/admin'
import { usePage } from '../../composables/usePage'

const { loading, rows, total, query, load, reload } = usePage(firmwareApi.page, {
  keyword: '',
  chipModel: ''
})

const statusText = (s) => ({ 0: '草稿', 1: '灰度中', 2: '全量', 3: '下线' }[s] || '-')
const statusType = (s) => ({ 0: 'info', 1: 'warning', 2: 'success', 3: 'info' }[s] || 'info')

const visible = ref(false)
const emptyForm = () => ({
  id: '',
  version: '',
  chipModel: '',
  boardType: '',
  fileUrl: '',
  fileSize: '',
  releaseNote: '',
  status: 0
})
const form = reactive(emptyForm())

function openCreate() {
  Object.assign(form, emptyForm())
  visible.value = true
}

function openEdit(row) {
  Object.assign(form, {
    id: row.id,
    version: row.version,
    chipModel: row.chipModel,
    boardType: row.boardType || '',
    fileUrl: row.fileUrl,
    fileSize: row.fileSize || '',
    releaseNote: row.releaseNote || '',
    status: row.status
  })
  visible.value = true
}

async function save() {
  if (!form.version || !form.chipModel || !form.fileUrl) {
    ElMessage.warning('版本、芯片、地址必填')
    return
  }
  await firmwareApi.save({ ...form })
  ElMessage.success('已保存')
  visible.value = false
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
