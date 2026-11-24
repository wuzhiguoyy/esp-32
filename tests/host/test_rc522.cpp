#include <cstdint>
#include <cstdio>
#include <deque>
#include <map>
#include <string>

#include "features/rc522.h"

/**
 * 假 SPI 总线：用内存寄存器模拟 RC522。
 */
class FakeSpiBus : public ISpiBus {
public:
    void Transfer(const uint8_t* tx, uint8_t* rx, size_t len) override {
        if (tx == nullptr || len < 2) {
            return;
        }
        uint8_t address = static_cast<uint8_t>((tx[0] >> 1) & 0x7F);
        bool read = (tx[0] & 0x80) != 0;

        if (read) {
            uint8_t value = ReadValue(address);
            if (rx != nullptr) {
                rx[1] = value;
            }
            return;
        }
        WriteValue(address, tx[1]);
    }

    void SetCs(int) override {}

    // 读取时按顺序返回 FIFO 数据
    uint8_t ReadValue(uint8_t address) {
        if (address == 0x09) {  // FIFODataReg
            if (fifo_.empty()) {
                return 0;
            }
            uint8_t value = fifo_.front();
            fifo_.pop_front();
            return value;
        }
        if (address == 0x0A) {  // FIFOLevelReg
            return static_cast<uint8_t>(fifo_.size());
        }
        if (address == 0x04) {  // ComIrqReg
            return 0x30;
        }
        if (address == 0x06) {  // ErrorReg
            return 0x00;
        }
        return registers_[address];
    }

    void WriteValue(uint8_t address, uint8_t value) {
        registers_[address] = value;
    }

    void LoadCardUid(const uint8_t uid[5]) {
        fifo_.clear();
        for (int i = 0; i < 5; i++) {
            fifo_.push_back(uid[i]);
        }
    }

    std::map<uint8_t, uint8_t> registers_;
    std::deque<uint8_t> fifo_;
};

static int g_failed = 0;

#define CHECK(cond)                                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond);             \
            g_failed++;                                                        \
        }                                                                      \
    } while (0)

int main() {
    FakeSpiBus bus;
    Rc522 rc522(&bus);

    CHECK(rc522.Init());

    // 寄存器读写
    rc522.WriteRegister(0x0C, 0xAB);
    CHECK(rc522.ReadRegister(0x0C) == 0xAB);

    // 有卡：UID 0x00,0xB2,0xC3,0xD4,0xE5 → 期望 B2C3D4E5
    {
        const uint8_t uid[5] = { 0x00, 0xB2, 0xC3, 0xD4, 0xE5 };
        bus.LoadCardUid(uid);
        std::string result;
        CHECK(rc522.ReadUidOnce(&result));
        CHECK(result == "B2C3D4E5");
    }

    // 无卡：FIFO 为空，FIFOLevel=0 → 失败
    {
        bus.fifo_.clear();
        std::string result;
        CHECK(!rc522.ReadUidOnce(&result));
        CHECK(result.empty());
    }

    // 空总线
    {
        Rc522 null_rc522(nullptr);
        CHECK(!null_rc522.Init());
        std::string result;
        CHECK(!null_rc522.ReadUidOnce(&result));
    }

    if (g_failed == 0) {
        printf("rc522: all tests passed\n");
        return 0;
    }
    printf("rc522: %d test(s) failed\n", g_failed);
    return 1;
}
