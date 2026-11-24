<template>
  <el-card>
    <el-form :inline="true" class="filters">
      <el-form-item label="关键字">
        <el-input v-model="query.keyword" placeholder="订单号 / 设备ID" clearable />
      </el-form-item>
      <el-form-item label="开始">
        <el-date-picker v-model="query.startDate" type="date" value-format="YYYY-MM-DD" placeholder="开始日期" />
      </el-form-item>
      <el-form-item label="结束">
        <el-date-picker v-model="query.endDate" type="date" value-format="YYYY-MM-DD" placeholder="结束日期" />
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="reload">查询</el-button>
      </el-form-item>
    </el-form>

    <el-table :data="rows" v-loading="loading" border stripe>
      <el-table-column prop="orderNo" label="订单号" min-width="180" />
      <el-table-column prop="userId" label="用户ID" width="120" />
      <el-table-column prop="deviceId" label="设备ID" min-width="170" />
      <el-table-column prop="planName" label="套餐" width="130" />
      <el-table-column prop="payAmount" label="实付" width="100" />
      <el-table-column label="支付状态" width="110">
        <template #default="{ row }">
          <el-tag :type="payType(row.payStatus)">{{ payText(row.payStatus) }}</el-tag>
        </template>
      </el-table-column>
      <el-table-column prop="payTime" label="支付时间" min-width="170" />
      <el-table-column prop="createTime" label="创建时间" min-width="170" />
    </el-table>

    <el-pagination class="pager" background layout="total, sizes, prev, pager, next" :total="total"
                   v-model:current-page="query.pageNum" v-model:page-size="query.pageSize"
                   :page-sizes="[10, 20, 50]" @current-change="load" @size-change="reload" />
  </el-card>
</template>

<script setup>
import { onMounted } from 'vue'
import { orderApi } from '../../api/admin'
import { usePage } from '../../composables/usePage'

const { loading, rows, total, query, load, reload } = usePage(orderApi.page, {
  keyword: '',
  startDate: '',
  endDate: ''
})

const payText = (s) => ({ 0: '待支付', 1: '已支付', 2: '已退款', 3: '已关闭' }[s] || '-')
const payType = (s) => ({ 0: 'info', 1: 'success', 2: 'warning', 3: 'info' }[s] || 'info')

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
