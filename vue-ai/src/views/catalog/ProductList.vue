<template>
  <el-card>
    <el-form :inline="true" class="filters">
      <el-form-item label="关键字">
        <el-input v-model="query.keyword" placeholder="产品编码 / 名称" clearable @keyup.enter="reload" />
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="reload">查询</el-button>
        <el-button type="success" @click="openCreate">新增产品</el-button>
      </el-form-item>
    </el-form>

    <el-table :data="rows" v-loading="loading" border stripe>
      <el-table-column prop="productCode" label="产品编码" width="160" />
      <el-table-column prop="productName" label="产品名称" min-width="160" />
      <el-table-column prop="chipModel" label="芯片" width="120" />
      <el-table-column prop="boardType" label="板型" min-width="150" />
      <el-table-column label="状态" width="90">
        <template #default="{ row }">
          <el-tag :type="row.status === 1 ? 'success' : 'info'">{{ row.status === 1 ? '启用' : '停用' }}</el-tag>
        </template>
      </el-table-column>
      <el-table-column prop="createTime" label="创建时间" min-width="170" />
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

  <el-dialog v-model="visible" :title="form.id ? '编辑产品' : '新增产品'" width="520px">
    <el-form :model="form" label-width="110px">
      <el-form-item label="产品编码"><el-input v-model="form.productCode" /></el-form-item>
      <el-form-item label="产品名称"><el-input v-model="form.productName" /></el-form-item>
      <el-form-item label="芯片型号"><el-input v-model="form.chipModel" placeholder="ESP32-S3" /></el-form-item>
      <el-form-item label="板型"><el-input v-model="form.boardType" placeholder="bread-compact-wifi" /></el-form-item>
      <el-form-item label="描述"><el-input v-model="form.description" type="textarea" :rows="2" /></el-form-item>
      <el-form-item label="状态">
        <el-switch v-model="form.status" :active-value="1" :inactive-value="0" active-text="启用" />
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
import { productApi } from '../../api/admin'
import { usePage } from '../../composables/usePage'

const { loading, rows, total, query, load, reload } = usePage(productApi.page, { keyword: '' })

const visible = ref(false)
const emptyForm = () => ({
  id: '',
  productCode: '',
  productName: '',
  chipModel: '',
  boardType: '',
  description: '',
  status: 1
})
const form = reactive(emptyForm())

function openCreate() {
  Object.assign(form, emptyForm())
  visible.value = true
}

function openEdit(row) {
  Object.assign(form, {
    id: row.id,
    productCode: row.productCode,
    productName: row.productName,
    chipModel: row.chipModel,
    boardType: row.boardType || '',
    description: row.description || '',
    status: row.status
  })
  visible.value = true
}

async function save() {
  if (!form.productCode || !form.productName || !form.chipModel) {
    ElMessage.warning('编码、名称、芯片型号必填')
    return
  }
  await productApi.save({ ...form })
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
