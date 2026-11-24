<template>
  <el-container class="layout">
    <el-aside width="210px" class="aside">
      <div class="logo">道玄设备后台</div>
      <el-menu :default-active="$route.path" router background-color="#1f2d3d" text-color="#bfcbd9"
               active-text-color="#409eff">
        <el-menu-item index="/dashboard">概览</el-menu-item>
        <el-sub-menu index="device">
          <template #title>设备</template>
          <el-menu-item index="/devices">设备管理</el-menu-item>
          <el-menu-item index="/user-devices">绑定关系</el-menu-item>
        </el-sub-menu>
        <el-sub-menu index="catalog">
          <template #title>产品</template>
          <el-menu-item index="/products">产品管理</el-menu-item>
          <el-menu-item index="/profiles">设备配置</el-menu-item>
        </el-sub-menu>
        <el-sub-menu index="billing">
          <template #title>计费</template>
          <el-menu-item index="/plans">套餐管理</el-menu-item>
          <el-menu-item index="/orders">订单管理</el-menu-item>
          <el-menu-item index="/subscriptions">订阅管理</el-menu-item>
          <el-menu-item index="/usage">用量统计</el-menu-item>
        </el-sub-menu>
        <el-sub-menu index="ops">
          <template #title>运维</template>
          <el-menu-item index="/firmware">固件管理</el-menu-item>
          <el-menu-item index="/model-keys">模型 Key</el-menu-item>
          <el-menu-item index="/accounts">后台账号</el-menu-item>
          <el-menu-item index="/audits">操作审计</el-menu-item>
        </el-sub-menu>
      </el-menu>
    </el-aside>

    <el-container>
      <el-header class="header">
        <span class="title">{{ $route.meta.title || '管理后台' }}</span>
        <div class="right">
          <span class="user">{{ user.nickname || user.username || '管理员' }}</span>
          <el-button link type="primary" @click="logout">退出登录</el-button>
        </div>
      </el-header>
      <el-main>
        <router-view />
      </el-main>
    </el-container>
  </el-container>
</template>

<script setup>
import { useRouter } from 'vue-router'
import { clearToken, getUser } from '../api/request'

const router = useRouter()
const user = getUser()

function logout() {
  clearToken()
  router.push('/login')
}
</script>

<style scoped>
.layout {
  height: 100%;
}
.aside {
  background: #1f2d3d;
}
.logo {
  height: 56px;
  line-height: 56px;
  color: #fff;
  text-align: center;
  font-weight: 600;
  letter-spacing: 1px;
}
.header {
  background: #fff;
  display: flex;
  align-items: center;
  justify-content: space-between;
  border-bottom: 1px solid #ebeef5;
}
.title {
  font-size: 16px;
  font-weight: 600;
}
.right {
  display: flex;
  align-items: center;
  gap: 12px;
}
.user {
  color: #606266;
  font-size: 13px;
}
</style>
