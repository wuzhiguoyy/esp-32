<template>
  <div>
    <el-row :gutter="16">
      <el-col :span="24 / cards.length" v-for="item in cards" :key="item.label">
        <el-card shadow="hover" class="card">
          <div class="label">{{ item.label }}</div>
          <div class="value">{{ item.value }}</div>
        </el-card>
      </el-col>
    </el-row>

    <el-card class="tips">
      <template #header>使用说明</template>
      <ul>
        <li>设备列表可绑定/解绑用户、禁用/启用设备</li>
        <li>产品与设备配置决定设备运行模式（chat / translate）和音色</li>
        <li v-if="billingEnabled">套餐、订单、订阅为收费模块，用量按设备按天统计</li>
        <li>所有写操作都会记录到操作审计</li>
      </ul>
    </el-card>
  </div>
</template>

<script setup>
import { onMounted, ref } from 'vue'
import { deviceApi, orderApi, planApi, userDeviceApi } from '../api/admin'
import { BILLING_ENABLED } from '../config/features'

const billingEnabled = BILLING_ENABLED

const cards = ref([
  { label: '设备总数', value: '-' },
  { label: '绑定关系', value: '-' }
])
if (billingEnabled) {
  cards.value.push({ label: '套餐数量', value: '-' }, { label: '订单数量', value: '-' })
}

onMounted(async () => {
  try {
    const [devices, relations] = await Promise.all([
      deviceApi.page({ pageNum: 1, pageSize: 1 }),
      userDeviceApi.page({ pageNum: 1, pageSize: 1, status: 1 })
    ])
    cards.value[0].value = devices.totalCount ?? 0
    cards.value[1].value = relations.totalCount ?? 0
  } catch (e) {
    // 首页统计失败不阻塞
  }

  if (!billingEnabled) {
    return
  }
  try {
    const [plans, orders] = await Promise.all([
      planApi.page({ pageNum: 1, pageSize: 1 }),
      orderApi.page({ pageNum: 1, pageSize: 1 })
    ])
    cards.value[2].value = plans.totalCount ?? 0
    cards.value[3].value = orders.totalCount ?? 0
  } catch (e) {
    // 忽略
  }
})
</script>

<style scoped>
.card {
  text-align: center;
}
.label {
  color: #909399;
  font-size: 13px;
}
.value {
  font-size: 28px;
  font-weight: 600;
  margin-top: 8px;
}
.tips {
  margin-top: 16px;
}
</style>
