#include "shop_tools.h"

#include "mcp_server.h"
#include "features/door_lock.h"
#include "features/weight_sensor.h"
#include "features/rfid_reader.h"

void RegisterShopTools(McpServer& server) {
#if CONFIG_ESP32_SHOP_LOCK
    // 开门：远程/语音都可以触发，服务端也会调用
    server.AddTool("self.lock.open",
                   "Open the door lock (unlock). Use when the order is paid.",
                   PropertyList(),
                   [](const PropertyList&) -> ToolResult {
                       if (!DoorLock::GetInstance().Init()) {
                           return std::unexpected("Lock not configured");
                       }
                       return ReturnValue(DoorLock::GetInstance().Open());
                   });

    server.AddTool("self.lock.close",
                   "Close the door lock (lock).",
                   PropertyList(),
                   [](const PropertyList&) -> ToolResult {
                       if (!DoorLock::GetInstance().Init()) {
                           return std::unexpected("Lock not configured");
                       }
                       return ReturnValue(DoorLock::GetInstance().Close());
                   });

    server.AddTool("self.lock.status",
                   "Get door lock state. Return true when unlocked.",
                   PropertyList(),
                   [](const PropertyList&) -> ToolResult {
                       return ReturnValue(DoorLock::GetInstance().IsOpen());
                   });
#endif

#if CONFIG_ESP32_SHOP_WEIGHT
    server.AddTool("self.weight.read",
                   "Read the current weight on the shelf in grams.",
                   PropertyList(),
                   [](const PropertyList&) -> ToolResult {
                       if (!WeightSensor::GetInstance().Init()) {
                           return std::unexpected("Scale not configured");
                       }
                       int grams = static_cast<int>(WeightSensor::GetInstance().ReadGrams());
                       return ReturnValue(grams);
                   });

    server.AddTool("self.weight.tare",
                   "Set current weight as zero (tare the scale).",
                   PropertyList(),
                   [](const PropertyList&) -> ToolResult {
                       if (!WeightSensor::GetInstance().Init()) {
                           return std::unexpected("Scale not configured");
                       }
                       WeightSensor::GetInstance().Tare();
                       return ReturnValue(true);
                   });
#endif

#if CONFIG_ESP32_SHOP_RFID
    server.AddTool("self.rfid.read_uid",
                   "Wait for an RFID card and return its UID. Args:\n"
                   "  `timeout`: seconds to wait, default 5.",
                   PropertyList({Property("timeout", kPropertyTypeInteger, 5, 1, 30)}),
                   [](const PropertyList& properties) -> ToolResult {
                       if (!RfidReader::GetInstance().Init()) {
                           return std::unexpected("RFID not configured");
                       }
                       int seconds = 5;
                       try {
                           seconds = properties["timeout"].value<int>();
                       } catch (...) {
                           seconds = 5;
                       }
                       auto uid = RfidReader::GetInstance().ReadUid(seconds * 1000);
                       if (uid.empty()) {
                           return std::unexpected("No card detected");
                       }
                       return ReturnValue(uid);
                   });
#endif
}
