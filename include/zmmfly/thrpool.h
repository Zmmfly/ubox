#ifndef __ZMMFLY_THRPOOL_H__
#define __ZMMFLY_THRPOOL_H__

#include <queue>
#include <mutex>
#include <functional>
#include <vector>
#include <thread>
#include <tuple>
#include <memory>
#include <condition_variable>

namespace zmmfly
{

class thrpool
{
    public:
        typedef std::function < void(void) > task_t;
        thrpool(size_t num = 0);
        ~thrpool();
        thrpool(const thrpool&) = delete;
        const thrpool& operator =(const thrpool&) = delete;

        bool post(task_t fn, bool must_idle = false);
        size_t idle();
        bool stop();
        bool start(size_t num = 0);

    private:
        size_t prv_idle();

    private:
        typedef std::tuple<std::thread,bool> worker_t;
        std::vector<std::tuple<std::thread,bool>> m_workers;
        std::queue<task_t> m_tasks;
        std::mutex m_tasks_mtx;
        std::condition_variable m_tasks_cv;
        bool m_start = false;
        bool m_stop_req = false;
};

typedef std::shared_ptr<thrpool> thrpool_ptr;

}

#endif /* __ZMMFLY_THRPOOL_H__ */
