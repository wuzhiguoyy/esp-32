import { post } from './request'

const P = '/api/esp32-console'

export const authApi = {
  login: (body) => post(`${P}/auth/login`, body),
  register: (body) => post(`${P}/auth/register`, body)
}

export const deviceApi = {
  page: (body) => post(`${P}/device/page`, body),
  detail: (body) => post(`${P}/device/detail`, body),
  status: (body) => post(`${P}/device/status`, body),
  bind: (body) => post(`${P}/device/bind`, body),
  unbind: (body) => post(`${P}/device/unbind`, body)
}

export const userDeviceApi = {
  page: (body) => post(`${P}/user-device/page`, body),
  unbind: (body) => post(`${P}/user-device/unbind`, body)
}

export const productApi = {
  page: (body) => post(`${P}/product/page`, body),
  save: (body) => post(`${P}/product/save`, body)
}

export const profileApi = {
  get: (body) => post(`${P}/profile/get`, body),
  save: (body) => post(`${P}/profile/save`, body)
}

export const planApi = {
  page: (body) => post(`${P}/plan/page`, body),
  save: (body) => post(`${P}/plan/save`, body)
}

export const orderApi = {
  page: (body) => post(`${P}/order/page`, body)
}

export const subscriptionApi = {
  page: (body) => post(`${P}/subscription/page`, body)
}

export const usageApi = {
  page: (body) => post(`${P}/usage/page`, body)
}

export const firmwareApi = {
  page: (body) => post(`${P}/firmware/page`, body),
  save: (body) => post(`${P}/firmware/save`, body)
}

export const accountApi = {
  page: (body) => post(`${P}/account/page`, body),
  save: (body) => post(`${P}/account/save`, body)
}

export const auditApi = {
  page: (body) => post(`${P}/audit/page`, body)
}

export const modelConfigApi = {
  get: (body) => post(`${P}/model-config/get`, body),
  save: (body) => post(`${P}/model-config/save`, body)
}

export { post }
