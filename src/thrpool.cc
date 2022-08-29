#include "zmmfly/thrpool.h"
#include "spdlog/spdlog.h"

namespace zmmfly
{

thrpool::thrpool(size_t num)
{
    start(num);
}

thrpool::~thrpool()
{
    stop();
}

bool thrpool::start(size_t num)
{
    {
        std::unique_lock<std::mutex> lock(m_tasks_mtx);
        if (m_start)
            return false;
    }
    num = num == 0 ? std::thread::hardware_concurrency() : num;
    if (num == 0)
        num = 2;
    m_workers.clear();
    for (size_t i = 0; i < num; i++)
    {
        auto fn = [this, i] {
            spdlog::debug("zmmfly::thrpool thread {} start", i);
            for (;;)
            {
                task_t task;
                {
                    std::unique_lock<std::mutex> lock(m_tasks_mtx);
                    m_tasks_cv.wait(lock, [this] { return m_stop_req | !m_tasks.empty(); });
                    if (m_stop_req)
                        break;
                    std::get<1>(m_workers[i]) = false;
                    task = std::move(m_tasks.front());
                    m_tasks.pop();
                }
                task();
                {
                    std::unique_lock<std::mutex> lock(m_tasks_mtx);
                    std::get<1>(m_workers[i]) = true;
                }
            }
            spdlog::debug("zmmfly::thrpool thread {} stop", i);
        };
        m_workers.push_back(std::make_tuple(std::move(std::thread(fn)), true));
    }
    m_start = true;
    return true;
}

bool thrpool::stop()
{
    {
        std::unique_lock<std::mutex> lock(m_tasks_mtx);
        if (!m_start)
            return false;
        m_stop_req = true;
        m_tasks_cv.notify_all();
    }
    for (auto &it : m_workers)
    {
        std::get<0>(it).join();
    }
    m_start = false;
    m_workers.clear();
    return true;
}

bool thrpool::post(task_t fn, bool must_idle)
{
    std::unique_lock<std::mutex> lock(m_tasks_mtx);
    if (must_idle && prv_idle() == 0)
        return false;
    m_tasks.push(fn);
    m_tasks_cv.notify_all();
    return true;
}

size_t thrpool::idle()
{
    std::unique_lock<std::mutex> lock(m_tasks_mtx);
    return prv_idle();
}

size_t thrpool::prv_idle()
{
    size_t cnt = 0;
    if (!m_start)
        return 0;
    for (auto &it : m_workers)
    {
        cnt += std::get<1>(it) ? 1 : 0;
    }
    return cnt;
}
}; // namespace zmmfly