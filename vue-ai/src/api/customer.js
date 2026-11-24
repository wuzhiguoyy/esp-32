const BASE = import.meta.env.VITE_API_BASE || ''
const TOKEN_KEY = 'esp32_customer_token'

export function getCustomerToken() {
  return localStorage.getItem(TOKEN_KEY) || ''
}

export function setCustomerToken(token) {
  localStorage.setItem(TOKEN_KEY, token || '')
}

async function request(path, options = {}) {
  const headers = { 'Content-Type': 'application/json', ...(options.headers || {}) }
  const token = getCustomerToken()
  if (token) {
    headers.Authorization = token.startsWith('Bearer ') ? token : `Bearer ${token}`
  }
  const response = await fetch(`${BASE}${path}`, { ...options, headers })
  const text = await response.text()
  let data = null
  if (text) {
    try {
      data = JSON.parse(text)
    } catch (e) {
      data = text
    }
  }
  if (!response.ok) {
    throw new Error((data && (data.message || data.error)) || `请求失败 ${response.status}`)
  }
  return data
}

export const customerApi = {
  bind: (body) => request('/api/esp32/customer/bind', { method: 'POST', body: JSON.stringify(body) }),
  devices: () => request('/api/esp32/customer/devices?pageNum=1&pageSize=50', { method: 'GET' }),
  unbind: (body) => request('/api/esp32/customer/unbind', { method: 'POST', body: JSON.stringify(body) }),
  getModelConfig: () => request('/api/esp32/customer/model-config/get', { method: 'POST', body: '{}' }),
  saveModelConfig: (body) =>
    request('/api/esp32/customer/model-config/save', { method: 'POST', body: JSON.stringify(body) })
}
