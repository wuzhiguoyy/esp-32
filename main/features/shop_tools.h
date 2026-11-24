#ifndef _FEATURE_SHOP_TOOLS_H
#define _FEATURE_SHOP_TOOLS_H

class McpServer;

/**
 * 注册无人超市相关 MCP 工具（门锁、称重、RFID）。
 * 未启用的功能不会注册对应工具。
 */
void RegisterShopTools(McpServer& server);

#endif // _FEATURE_SHOP_TOOLS_H
