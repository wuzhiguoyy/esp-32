<template>
  <el-card>
    <el-form :inline="true" class="filters">
      <el-form-item label="关键字">
        <el-input v-model="query.keyword" placeholder="对象ID / 操作人" clearable />
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
      <el-table-column prop="createTime" label="时间" min-width="170" />
      <el-table-column prop="module" label="模块" width="120" />
      <el-table-column prop="action" label="动作" width="140" />
      <el-table-column prop="targetId" label="对象ID" min-width="170" />
      <el-table-column prop="adminUsername" label="操作人" width="130" />
      <el-table-column prop="remark" label="备注" min-width="180" show-overflow-tooltip />
      <el-table-column prop="ip" label="IP" width="140" />
    </el-table>

    <el-pagination class="pager" background layout="total, sizes, prev, pager, next" :total="total"
                   v-model:current-page="query.pageNum" v-model:page-size="query.pageSize"
                   :page-sizes="[20, 50, 100]" @current-change="load" @size-change="reload" />
  </el-card>
</template>

<script setup>
import { onMounted } from 'vue'
import { auditApi } from '../../api/admin'
import { usePage } from '../../composables/usePage'

const { loading, rows, total, query, load, reload } = usePage(auditApi.page, {
  keyword: '',
  startDate: '',
  endDate: ''
})

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
