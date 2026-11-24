#include "door_lock_logic.h"

DoorLockLogic::DoorLockLogic(IGpio* gpio, int pin, bool active_high)
    : gpio_(gpio), pin_(pin), active_high_(active_high) {
}

bool DoorLockLogic::Init() {
    if (gpio_ == nullptr) {
        return false;
    }
    gpio_->ConfigureOutput(pin_);
    gpio_->SetLevel(pin_, OffLevel());
    open_ = false;
    initialized_ = true;
    return true;
}

bool DoorLockLogic::Open() {
    if (!initialized_ && !Init()) {
        return false;
    }
    gpio_->SetLevel(pin_, OnLevel());
    open_ = true;
    return true;
}

bool DoorLockLogic::Close() {
    if (!initialized_ && !Init()) {
        return false;
    }
    gpio_->SetLevel(pin_, OffLevel());
    open_ = false;
    return true;
}

bool DoorLockLogic::IsOpen() const {
    return open_;
}

int DoorLockLogic::OnLevel() const {
    return active_high_ ? 1 : 0;
}

int DoorLockLogic::OffLevel() const {
    return active_high_ ? 0 : 1;
}
