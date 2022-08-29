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

    try {
        for (size_t i=0; i<10; i++) {
            auto stage = zmmfly::pipe::line::make([i](zmmfly::pipe::line_ptr ptr, std::any& dat){
                if (dat.type() == typeid(std::string)) {
                    auto value = std::any_cast<std::string>(dat);
                    auto out = fmt::format("{}, stage {}", value, i);
                    spdlog::info("stage {}, in: [{}], out: [{}]", i, value, out);
                    ptr->out(out);
                }
            }, thrpool);
            pipelines.push_back(stage);
            if (i>=1) {
                *pipelines[i-1] + *pipelines[i];
            }
        }
        pipelines[0]->in(std::string("Hello"));
    }catch(std::exception &e) {
        spdlog::error("exception: {}", e.what());
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
