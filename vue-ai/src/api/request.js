const BASE = import.meta.env.VITE_API_BASE || ''
const TOKEN_KEY = 'esp32_admin_token'
const USER_KEY = 'esp32_admin_user'

export function getToken() {
  return localStorage.getItem(TOKEN_KEY) || ''
}

export function setToken(token) {
  localStorage.setItem(TOKEN_KEY, token || '')
}

export function clearToken() {
  localStorage.removeItem(TOKEN_KEY)
  localStorage.removeItem(USER_KEY)
}

export function setUser(user) {
  localStorage.setItem(USER_KEY, JSON.stringify(user || {}))
}

export function getUser() {
  try {
    return JSON.parse(localStorage.getItem(USER_KEY) || '{}')
  } catch (e) {
    return {}
  }
}

async function request(path, options = {}) {
  const headers = {
    'Content-Type': 'application/json',
    ...(options.headers || {})
  }
  const token = getToken()
  if (token) {
    headers.Authorization = token.startsWith('Bearer ') ? token : `Bearer ${token}`
  }

  const response = await fetch(`${BASE}${path}`, { ...options, headers })
  if (response.status === 401) {
    clearToken()
    if (location.hash !== '#/login') {
      location.hash = '#/login'
    }
    throw new Error('登录已过期')
  }

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
    const message = (data && (data.message || data.error)) || `请求失败 ${response.status}`
    throw new Error(message)
  }
  return data
}

export function post(path, body) {
  return request(path, { method: 'POST', body: JSON.stringify(body || {}) })
}

export function get(path) {
  return request(path, { method: 'GET' })
}
