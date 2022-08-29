#include "zmmfly/thrpool.h"
#include <chrono>
#include "spdlog/spdlog.h"

int main(int argc, char **argv)
{
    zmmfly::thrpool pool(5);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    for (size_t i=0; i<10; i++) {
        pool.post([i]{
            spdlog::info("Hello world! {}", i);
        });
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}