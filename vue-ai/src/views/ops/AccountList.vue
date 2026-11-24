<template>
  <el-card>
    <el-form :inline="true" class="filters">
      <el-form-item label="关键字">
        <el-input v-model="query.keyword" placeholder="用户名 / 昵称" clearable />
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="reload">查询</el-button>
        <el-button type="success" @click="openCreate">新增账号</el-button>
      </el-form-item>
    </el-form>

    <el-table :data="rows" v-loading="loading" border stripe>
      <el-table-column prop="username" label="用户名" width="140" />
      <el-table-column prop="nickname" label="昵称" min-width="120" />
      <el-table-column prop="role" label="角色" width="130" />
      <el-table-column label="状态" width="90">
        <template #default="{ row }">
          <el-tag :type="row.status === 1 ? 'success' : 'info'">{{ row.status === 1 ? '启用' : '禁用' }}</el-tag>
        </template>
      </el-table-column>
      <el-table-column prop="lastLoginAt" label="最后登录" min-width="170" />
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

  <el-dialog v-model="visible" :title="form.id ? '编辑账号' : '新增账号'" width="480px">
    <el-form :model="form" label-width="90px">
      <el-form-item label="用户名"><el-input v-model="form.username" /></el-form-item>
      <el-form-item label="密码">
        <el-input v-model="form.password" type="password" show-password
                  :placeholder="form.id ? '留空则不修改' : '必填'" />
      </el-form-item>
      <el-form-item label="昵称"><el-input v-model="form.nickname" /></el-form-item>
      <el-form-item label="手机号"><el-input v-model="form.phone" /></el-form-item>
      <el-form-item label="角色">
        <el-select v-model="form.role" style="width: 100%">
          <el-option label="超级管理员" value="SUPER_ADMIN" />
          <el-option label="管理员" value="ADMIN" />
          <el-option label="运营" value="OPERATOR" />
          <el-option label="只读" value="VIEWER" />
        </el-select>
      </el-form-item>
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
import { accountApi } from '../../api/admin'
import { usePage } from '../../composables/usePage'

const { loading, rows, total, query, load, reload } = usePage(accountApi.page, { keyword: '' })

const visible = ref(false)
const emptyForm = () => ({
  id: '',
  username: '',
  password: '',
  nickname: '',
  phone: '',
  role: 'ADMIN',
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
    username: row.username,
    password: '',
    nickname: row.nickname || '',
    phone: row.phone || '',
    role: row.role || 'ADMIN',
    status: row.status
  })
  visible.value = true
}

async function save() {
  if (!form.username) {
    ElMessage.warning('用户名必填')
    return
  }
  await accountApi.save({ ...form })
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
