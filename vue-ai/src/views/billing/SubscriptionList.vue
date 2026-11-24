<template>
  <el-card>
    <el-form :inline="true" class="filters">
      <el-form-item label="关键字">
        <el-input v-model="query.keyword" placeholder="设备ID / 用户ID" clearable />
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="reload">查询</el-button>
      </el-form-item>
    </el-form>

    <el-table :data="rows" v-loading="loading" border stripe>
      <el-table-column prop="deviceId" label="设备ID" min-width="180" />
      <el-table-column prop="userId" label="用户ID" width="120" />
      <el-table-column prop="planId" label="套餐ID" width="110" />
      <el-table-column prop="startAt" label="生效时间" min-width="170" />
      <el-table-column prop="expireAt" label="到期时间" min-width="170" />
      <el-table-column label="状态" width="100">
        <template #default="{ row }">
          <el-tag :type="statusType(row.status)">{{ statusText(row.status) }}</el-tag>
        </template>
      </el-table-column>
      <el-table-column label="自动续费" width="100">
        <template #default="{ row }">{{ row.autoRenew === 1 ? '是' : '否' }}</template>
      </el-table-column>
    </el-table>

    <el-pagination class="pager" background layout="total, sizes, prev, pager, next" :total="total"
                   v-model:current-page="query.pageNum" v-model:page-size="query.pageSize"
                   :page-sizes="[10, 20, 50]" @current-change="load" @size-change="reload" />
  </el-card>
</template>

<script setup>
import { onMounted } from 'vue'
import { subscriptionApi } from '../../api/admin'
import { usePage } from '../../composables/usePage'

const { loading, rows, total, query, load, reload } = usePage(subscriptionApi.page, { keyword: '' })

const statusText = (s) => ({ 0: '已过期', 1: '生效中', 2: '已取消', 3: '未生效' }[s] || '-')
const statusType = (s) => ({ 0: 'info', 1: 'success', 2: 'warning', 3: 'info' }[s] || 'info')

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
