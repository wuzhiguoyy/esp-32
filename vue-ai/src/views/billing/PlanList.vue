<template>
  <el-card>
    <el-form :inline="true" class="filters">
      <el-form-item label="关键字">
        <el-input v-model="query.keyword" placeholder="编码 / 名称" clearable @keyup.enter="reload" />
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="reload">查询</el-button>
        <el-button type="success" @click="openCreate">新增套餐</el-button>
      </el-form-item>
    </el-form>

    <el-table :data="rows" v-loading="loading" border stripe>
      <el-table-column prop="planCode" label="编码" width="140" />
      <el-table-column prop="planName" label="名称" min-width="160" />
      <el-table-column label="计费" width="100">
        <template #default="{ row }">{{ billingText(row.billingType) }}</template>
      </el-table-column>
      <el-table-column prop="price" label="价格(元)" width="110" />
      <el-table-column prop="durationDays" label="天数" width="90" />
      <el-table-column prop="quotaAsr" label="ASR额度" width="110" />
      <el-table-column label="状态" width="90">
        <template #default="{ row }">
          <el-tag :type="row.status === 1 ? 'success' : 'info'">{{ row.status === 1 ? '上架' : '下架' }}</el-tag>
        </template>
      </el-table-column>
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

  <el-dialog v-model="visible" :title="form.id ? '编辑套餐' : '新增套餐'" width="560px">
    <el-form :model="form" label-width="110px">
      <el-form-item label="套餐编码"><el-input v-model="form.planCode" /></el-form-item>
      <el-form-item label="套餐名称"><el-input v-model="form.planName" /></el-form-item>
      <el-form-item label="计费方式">
        <el-select v-model="form.billingType" style="width: 100%">
          <el-option label="按月" :value="1" />
          <el-option label="按年" :value="2" />
          <el-option label="按次" :value="3" />
          <el-option label="永久" :value="4" />
        </el-select>
      </el-form-item>
      <el-form-item label="价格"><el-input-number v-model="form.price" :min="0" :precision="2" /></el-form-item>
      <el-form-item label="有效天数"><el-input-number v-model="form.durationDays" :min="1" /></el-form-item>
      <el-form-item label="ASR额度"><el-input v-model="form.quotaAsr" placeholder="秒，空=不限" /></el-form-item>
      <el-form-item label="TTS额度"><el-input v-model="form.quotaTts" placeholder="空=不限" /></el-form-item>
      <el-form-item label="描述"><el-input v-model="form.description" type="textarea" :rows="2" /></el-form-item>
      <el-form-item label="状态">
        <el-switch v-model="form.status" :active-value="1" :inactive-value="0" active-text="上架" />
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
import { planApi } from '../../api/admin'
import { usePage } from '../../composables/usePage'

const { loading, rows, total, query, load, reload } = usePage(planApi.page, { keyword: '' })

const billingText = (t) => ({ 1: '按月', 2: '按年', 3: '按次', 4: '永久' }[t] || '-')

const visible = ref(false)
const emptyForm = () => ({
  id: '',
  planCode: '',
  planName: '',
  billingType: 1,
  price: 0,
  durationDays: 30,
  quotaAsr: '',
  quotaTts: '',
  description: '',
  status: 1,
  sort: 0
})
const form = reactive(emptyForm())

function openCreate() {
  Object.assign(form, emptyForm())
  visible.value = true
}

function openEdit(row) {
  Object.assign(form, {
    id: row.id,
    planCode: row.planCode,
    planName: row.planName,
    billingType: row.billingType,
    price: Number(row.price || 0),
    durationDays: row.durationDays || 30,
    quotaAsr: row.quotaAsr || '',
    quotaTts: row.quotaTts || '',
    description: row.description || '',
    status: row.status,
    sort: row.sort || 0
  })
  visible.value = true
}

async function save() {
  if (!form.planCode || !form.planName) {
    ElMessage.warning('编码和名称必填')
    return
  }
  await planApi.save({ ...form })
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
