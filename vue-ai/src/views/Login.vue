<template>
  <div class="login">
    <el-card class="box">
      <h2>道玄设备管理后台</h2>

      <el-tabs v-model="tab" stretch>
        <el-tab-pane label="登录" name="login">
          <el-form :model="loginForm" label-position="top" @keyup.enter="doLogin">
            <el-form-item label="用户名">
              <el-input v-model="loginForm.username" placeholder="用户名" autocomplete="username" />
            </el-form-item>
            <el-form-item label="密码">
              <el-input v-model="loginForm.password" type="password" show-password
                        placeholder="密码" autocomplete="current-password" />
            </el-form-item>
            <el-button type="primary" class="btn" :loading="loading" @click="doLogin">登录</el-button>
          </el-form>
        </el-tab-pane>

        <el-tab-pane label="注册" name="register">
          <el-alert type="info" :closable="false" show-icon
                    title="客户自助注册：注册后可绑定设备。第一个注册的账号为管理员。" />
          <el-form :model="registerForm" label-position="top" class="register" @keyup.enter="doRegister">
            <el-form-item label="用户名">
              <el-input v-model="registerForm.username" placeholder="用户名" />
            </el-form-item>
            <el-form-item label="密码">
              <el-input v-model="registerForm.password" type="password" show-password placeholder="密码" />
            </el-form-item>
            <el-form-item label="昵称">
              <el-input v-model="registerForm.nickname" placeholder="选填" />
            </el-form-item>
            <el-button type="success" class="btn" :loading="loading" @click="doRegister">注册并登录</el-button>
          </el-form>
        </el-tab-pane>
      </el-tabs>
    </el-card>
  </div>
</template>

<script setup>
import { reactive, ref } from 'vue'
import { useRouter } from 'vue-router'
import { ElMessage } from 'element-plus'
import { setToken, setUser } from '../api/request'
import { setCustomerToken } from '../api/customer'
import { authApi } from '../api/admin'

const router = useRouter()
const tab = ref('login')
const loading = ref(false)
const loginForm = reactive({ username: '', password: '' })
const registerForm = reactive({ username: '', password: '', nickname: '' })

async function doLogin() {
  if (!loginForm.username || !loginForm.password) {
    ElMessage.warning('请填写用户名和密码')
    return
  }
  loading.value = true
  try {
    const data = await authApi.login({ ...loginForm })
    afterLogin(data)
  } catch (e) {
    ElMessage.error(e.message || '登录失败')
  } finally {
    loading.value = false
  }
}

async function doRegister() {
  if (!registerForm.username || !registerForm.password) {
    ElMessage.warning('请填写用户名和密码')
    return
  }
  loading.value = true
  try {
    const data = await authApi.register({ ...registerForm })
    afterLogin(data)
  } catch (e) {
    ElMessage.error(e.message || '注册失败')
  } finally {
    loading.value = false
  }
}

function afterLogin(data) {
  if (!data || !data.token) {
    ElMessage.error('登录返回异常')
    return
  }
  setToken(data.token)
  setCustomerToken(data.token)
  setUser({ username: data.username, nickname: data.nickname, role: data.role })
  ElMessage.success('登录成功')
  if (data.role === 'USER') {
    router.push('/my')
  } else {
    router.push('/dashboard')
  }
}
</script>

<style scoped>
.login {
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
  background: #1f2d3d;
}
.box {
  width: 420px;
}
.register {
  margin-top: 12px;
}
.btn {
  width: 100%;
  margin-top: 8px;
}
</style>
