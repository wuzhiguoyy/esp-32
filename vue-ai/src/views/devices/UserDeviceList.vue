<template>
  <el-card>
    <el-form :inline="true" class="filters">
      <el-form-item label="关键字">
        <el-input v-model="query.keyword" placeholder="设备ID / 用户ID" clearable @keyup.enter="reload" />
      </el-form-item>
      <el-form-item label="状态">
        <el-select v-model="query.status" placeholder="全部" clearable style="width: 140px">
          <el-option label="有效" :value="1" />
          <el-option label="已解绑" :value="0" />
        </el-select>
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="reload">查询</el-button>
      </el-form-item>
    </el-form>

    <el-table :data="rows" v-loading="loading" border stripe>
      <el-table-column prop="deviceId" label="设备ID" min-width="180" />
      <el-table-column prop="userId" label="用户ID" width="140" />
      <el-table-column label="归属" width="110">
        <template #default="{ row }">{{ row.isOwner === 1 ? '主绑定' : '被分享' }}</template>
      </el-table-column>
      <el-table-column label="状态" width="100">
        <template #default="{ row }">
          <el-tag :type="row.status === 1 ? 'success' : 'info'">
            {{ row.status === 1 ? '有效' : '已解绑' }}
          </el-tag>
        </template>
      </el-table-column>
      <el-table-column prop="bindTime" label="绑定时间" min-width="170" />
      <el-table-column prop="unbindTime" label="解绑时间" min-width="170" />
      <el-table-column label="操作" width="110" fixed="right">
        <template #default="{ row }">
          <el-button v-if="row.status === 1" link type="danger" @click="unbind(row)">解绑</el-button>
        </template>
      </el-table-column>
    </el-table>

    <el-pagination class="pager" background layout="total, sizes, prev, pager, next" :total="total"
                   v-model:current-page="query.pageNum" v-model:page-size="query.pageSize"
                   :page-sizes="[10, 20, 50]" @current-change="load" @size-change="reload" />
  </el-card>
</template>

<script setup>
import { onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { userDeviceApi } from '../../api/admin'
import { usePage } from '../../composables/usePage'

const { loading, rows, total, query, load, reload } = usePage(userDeviceApi.page, {
  keyword: '',
  status: null
})

async function unbind(row) {
  await ElMessageBox.confirm(`确认解绑设备 ${row.deviceId} 与用户 ${row.userId}？`, '提示', { type: 'warning' })
  await userDeviceApi.unbind({ deviceId: row.deviceId, userId: row.userId })
  ElMessage.success('已解绑')
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
