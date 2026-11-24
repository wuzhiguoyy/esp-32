#include "rc522.h"

#include <cstdio>

namespace {
// RC522 寄存器地址
constexpr uint8_t kCommandReg = 0x01;
constexpr uint8_t kComIrqReg = 0x04;
constexpr uint8_t kErrorReg = 0x06;
constexpr uint8_t kFIFODataReg = 0x09;
constexpr uint8_t kFIFOLevelReg = 0x0A;
constexpr uint8_t kBitFramingReg = 0x0D;
constexpr uint8_t kTxControlReg = 0x14;
constexpr uint8_t kTxASKReg = 0x15;
constexpr uint8_t kTModeReg = 0x2A;
constexpr uint8_t kTPrescalerReg = 0x2B;
constexpr uint8_t kTReloadRegH = 0x2C;
constexpr uint8_t kTReloadRegL = 0x2D;
constexpr uint8_t kModeReg = 0x11;

constexpr uint8_t kPcdResetPhase = 0x0F;
constexpr uint8_t kPcdTransceive = 0x0C;
constexpr uint8_t kPiccReqIdl = 0x26;
constexpr uint8_t kPiccAnticoll = 0x93;

constexpr uint8_t kErrorMask = 0x1B;
}  // namespace

bool Rc522::Init() {
    if (bus_ == nullptr) {
        return false;
    }
    WriteRegister(kCommandReg, kPcdResetPhase);
    WriteRegister(kTModeReg, 0x8D);
    WriteRegister(kTPrescalerReg, 0x3E);
    WriteRegister(kTReloadRegL, 30);
    WriteRegister(kTReloadRegH, 0);
    WriteRegister(kTxASKReg, 0x40);
    WriteRegister(kModeReg, 0x3D);
    AntennaOn();
    return true;
}

uint8_t Rc522::ReadRegister(uint8_t address) {
    if (bus_ == nullptr) {
        return 0;
    }
    uint8_t tx[2] = { static_cast<uint8_t>(((address << 1) & 0x7E) | 0x80), 0x00 };
    uint8_t rx[2] = { 0, 0 };
    bus_->Transfer(tx, rx, 2);
    return rx[1];
}

void Rc522::WriteRegister(uint8_t address, uint8_t value) {
    if (bus_ == nullptr) {
        return;
    }
    uint8_t tx[2] = { static_cast<uint8_t>((address << 1) & 0x7E), value };
    bus_->Transfer(tx, nullptr, 2);
}

void Rc522::SetBitMask(uint8_t reg, uint8_t mask) {
    WriteRegister(reg, static_cast<uint8_t>(ReadRegister(reg) | mask));
}

void Rc522::ClearBitMask(uint8_t reg, uint8_t mask) {
    WriteRegister(reg, static_cast<uint8_t>(ReadRegister(reg) & (~mask)));
}

void Rc522::AntennaOn() {
    uint8_t value = ReadRegister(kTxControlReg);
    if ((value & 0x03) != 0x03) {
        SetBitMask(kTxControlReg, 0x03);
    }
}

bool Rc522::WaitForIrq(int max_loops) {
    for (int i = 0; i < max_loops; i++) {
        uint8_t irq = ReadRegister(kComIrqReg);
        if (irq & 0x30) {
            return true;
        }
    }
    return false;
}

bool Rc522::RequestCard() {
    WriteRegister(kBitFramingReg, 0x07);
    WriteRegister(kFIFODataReg, kPiccReqIdl);
    WriteRegister(kCommandReg, kPcdTransceive);
    SetBitMask(kBitFramingReg, 0x80);

    bool ok = WaitForIrq(100);
    ClearBitMask(kBitFramingReg, 0x80);
    if (!ok) {
        return false;
    }
    return (ReadRegister(kErrorReg) & kErrorMask) == 0;
}

bool Rc522::AntiCollision(std::string* uid) {
    WriteRegister(kBitFramingReg, 0x00);
    WriteRegister(kFIFODataReg, kPiccAnticoll);
    WriteRegister(kFIFODataReg, 0x20);
    WriteRegister(kCommandReg, kPcdTransceive);
    SetBitMask(kBitFramingReg, 0x80);

    bool ok = WaitForIrq(100);
    ClearBitMask(kBitFramingReg, 0x80);
    if (!ok) {
        return false;
    }
    if ((ReadRegister(kErrorReg) & kErrorMask) != 0) {
        return false;
    }
    if (ReadRegister(kFIFOLevelReg) != 5) {
        return false;
    }

    uint8_t buffer[5] = {};
    for (int i = 0; i < 5; i++) {
        buffer[i] = ReadRegister(kFIFODataReg);
    }

    char text[16];
    snprintf(text, sizeof(text), "%02X%02X%02X%02X", buffer[1], buffer[2], buffer[3], buffer[4]);
    *uid = text;
    return true;
}

bool Rc522::ReadUidOnce(std::string* uid) {
    if (uid == nullptr || bus_ == nullptr) {
        return false;
    }
    if (!RequestCard()) {
        return false;
    }
    return AntiCollision(uid);
}
