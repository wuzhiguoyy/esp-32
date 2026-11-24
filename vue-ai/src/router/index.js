import { createRouter, createWebHashHistory } from 'vue-router'
import { getToken } from '../api/request'
import { BILLING_ENABLED } from '../config/features'
import Layout from '../layout/MainLayout.vue'

const children = [
  { path: 'dashboard', component: () => import('../views/Dashboard.vue'), meta: { title: '概览' } },
  { path: 'devices', component: () => import('../views/devices/DeviceList.vue'), meta: { title: '设备管理' } },
  { path: 'user-devices', component: () => import('../views/devices/UserDeviceList.vue'), meta: { title: '绑定关系' } },
  { path: 'products', component: () => import('../views/catalog/ProductList.vue'), meta: { title: '产品管理' } },
  { path: 'profiles', component: () => import('../views/catalog/ProfileEdit.vue'), meta: { title: '设备配置' } },
  { path: 'firmware', component: () => import('../views/ops/FirmwareList.vue'), meta: { title: '固件管理' } },
  { path: 'model-keys', component: () => import('../views/ops/ModelKey.vue'), meta: { title: '模型 Key' } },
  { path: 'accounts', component: () => import('../views/ops/AccountList.vue'), meta: { title: '后台账号' } },
  { path: 'audits', component: () => import('../views/ops/AuditList.vue'), meta: { title: '操作审计' } }
]

if (BILLING_ENABLED) {
  children.push(
    { path: 'plans', component: () => import('../views/billing/PlanList.vue'), meta: { title: '套餐管理' } },
    { path: 'orders', component: () => import('../views/billing/OrderList.vue'), meta: { title: '订单管理' } },
    { path: 'subscriptions', component: () => import('../views/billing/SubscriptionList.vue'), meta: { title: '订阅管理' } },
    { path: 'usage', component: () => import('../views/billing/UsageList.vue'), meta: { title: '用量统计' } }
  )
}

const routes = [
  {
    path: '/my',
    component: () => import('../views/customer/CustomerHome.vue'),
    meta: { public: true, title: '我的设备' }
  },
  {
    path: '/login',
    component: () => import('../views/Login.vue'),
    meta: { public: true }
  },
  {
    path: '/',
    component: Layout,
    redirect: '/dashboard',
    children
  },
  { path: '/:pathMatch(.*)*', redirect: '/dashboard' }
]

const router = createRouter({
  history: createWebHashHistory(),
  routes
})

router.beforeEach((to) => {
  if (to.meta.public) {
    return true
  }
  if (!getToken()) {
    return '/login'
  }
  return true
})

export default router
