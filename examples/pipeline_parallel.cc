#include "zmmfly/thrpool.h"
#include "zmmfly/pipeline.h"
#include <chrono>
#include "spdlog/spdlog.h"
#include "fmt/core.h"
#include "fmt/format.h"

int main(int argc, char **argv)
{
    auto thrpool = std::make_shared<zmmfly::thrpool>();
    std::vector<zmmfly::pipe::line_ptr> pipelines;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    spdlog::set_level(spdlog::level::debug);

    for (size_t i=0; i<5; i++) {
        auto stage = zmmfly::pipe::line::make([i](zmmfly::pipe::line_ptr ptr, std::any& dat){
            if (dat.type() == typeid(std::string)) {
                auto value = std::any_cast<std::string>(dat);
                auto out = fmt::format("{}, stage {}", value, i);
                spdlog::info("stage {}, in: [{}], out: [{}]", i, value, out);
            }
        }, thrpool);
        pipelines.push_back(stage);
    }

    auto stage1 = zmmfly::pipe::line::make([](zmmfly::pipe::line_ptr ptr, std::any& dat){
        if (dat.type() == typeid(std::string)) {
            auto value = std::any_cast<std::string>(dat);
            auto out = fmt::format("{}, stage start", value);
            spdlog::info("stage start, in: [{}], out: [{}]", value, out);
            ptr->out(out);
        }
    }, thrpool);

    for (auto it:pipelines) {
        *stage1 + *it;
    }

    stage1->in(std::string("parallel"));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    *stage1 - *pipelines[2];
    stage1->in(std::string("parallel2"));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
