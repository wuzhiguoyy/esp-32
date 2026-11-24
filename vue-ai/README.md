# 道玄设备管理后台（vue-ai）

ESP32 设备管理后台前端，Vue 3 + Vite + Element Plus。

## 功能

| 模块 | 页面 |
| --- | --- |
| 概览 | 设备、绑定、套餐、订单数量 |
| 设备 | 列表、绑定/解绑、禁用/启用 |
| 绑定关系 | 列表、解绑 |
| 产品 | 列表、新增、编辑 |
| 设备配置 | 查询、保存（模式/音色/音量/唤醒词） |
| 计费 | 套餐、订单、订阅、用量 |
| 运维 | 固件、后台账号、操作审计 |

## 开发

```bash
cd vue-ai
npm install
npm run dev
```

默认地址 `http://localhost:5173`。

开发代理默认指向 `http://127.0.0.1:8282`，可改环境变量：

```bash
# .env.local
VITE_PROXY_TARGET=https://你的服务端地址
VITE_API_BASE=
```

## 登录

后台接口需要 JWT：

1. 用现有账号体系登录获取 Token；
2. 打开页面粘贴 Token；
3. Token 存 `localStorage`，失效自动跳回登录页。

## 构建

```bash
npm run build
```

产物在 `dist/`。

## 部署

Nginx 把 `dist/` 作为静态站点，并把 `/api/` 转发到 Java：

```nginx
location / {
    root /opt/esp32-admin;
    try_files $uri $uri/ /index.html;
}

location /api/ {
    proxy_pass http://127.0.0.1:8282;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
}
```

## 接口

全部走 `/api/esp32-console/**`，返回 DTO，分页统一 `PageBean`。

## 说明

- 前端不写死服务端地址，走同域 `/api`；
- 不包含任何密钥或域名配置；
- 生产环境建议开启 HTTPS 与登录鉴权。
