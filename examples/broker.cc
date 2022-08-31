#include "zmmfly/thrpool.h"
#include "zmmfly/broker.h"
#include <chrono>
#include "spdlog/spdlog.h"

int main(int argc, char **argv)
{
    auto thrpool = std::make_shared<zmmfly::thrpool>();
    auto broker = std::make_shared<zmmfly::broker::core>(thrpool);

    auto sub1 = broker->sub(L"default/sub1", [](std::wstring &topic, std::any &msg){
        if (msg.type() == typeid(std::string)) {
            spdlog::info("sub1 str: {}", std::any_cast<std::string>(msg));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });

    auto sub2 = broker->sub(L"default/sub2", [](std::wstring &topic, std::any &msg){
        if (msg.type() == typeid(int)) {
            spdlog::info("sub2 num: {}", std::any_cast<int>(msg));
        }
    });

    for (int i=0; i<100; i++) {
        auto str = fmt::format("Hello World {}", i);
        broker->pub(L"default/sub1", str);
    }
    broker->pub(L"default/sub2", 1234567);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    if (broker->unsub(sub1)) spdlog::info("sub1 unsub success");

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}