#include <cstdio>
#include <map>
#include <set>

#include "features/door_lock_logic.h"

/**
 * 假 GPIO：记录每个引脚的电平，用于验证门锁逻辑。
 */
class FakeGpio : public IGpio {
public:
    void ConfigureOutput(int pin) override { outputs_.insert(pin); }
    void ConfigureInput(int pin, bool) override { inputs_.insert(pin); }
    void SetLevel(int pin, int level) override { levels_[pin] = level; }
    int GetLevel(int pin) override { return levels_[pin]; }

    int Level(int pin) const {
        auto it = levels_.find(pin);
        return it == levels_.end() ? -1 : it->second;
    }

    std::map<int, int> levels_;
    std::set<int> outputs_;
    std::set<int> inputs_;
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
    // 高电平触发
    {
        FakeGpio gpio;
        DoorLockLogic lock(&gpio, 5, true);

        CHECK(lock.Init());
        CHECK(gpio.Level(5) == 0);   // 初始化后为关锁

        CHECK(lock.Open());
        CHECK(lock.IsOpen());
        CHECK(gpio.Level(5) == 1);

        CHECK(lock.Close());
        CHECK(!lock.IsOpen());
        CHECK(gpio.Level(5) == 0);
    }

    // 低电平触发
    {
        FakeGpio gpio;
        DoorLockLogic lock(&gpio, 6, false);

        CHECK(lock.Init());
        CHECK(gpio.Level(6) == 1);   // 关闭时高电平

        CHECK(lock.Open());
        CHECK(gpio.Level(6) == 0);

        CHECK(lock.Close());
        CHECK(gpio.Level(6) == 1);
    }

    // 未注入 GPIO
    {
        DoorLockLogic lock(nullptr, 5, true);
        CHECK(!lock.Init());
        CHECK(!lock.Open());
    }

    if (g_failed == 0) {
        printf("door_lock_logic: all tests passed\n");
        return 0;
    }
    printf("door_lock_logic: %d test(s) failed\n", g_failed);
    return 1;
}
