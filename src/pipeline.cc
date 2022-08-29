#include "zmmfly/pipeline.h"
#include "spdlog/spdlog.h"

namespace zmmfly::pipe
{

line::line(std::string name)
{
    m_name = name;
}

line::line(line_proc proc, std::string name)
{
    m_proc = proc;
}

line::line(line_proc proc, thrpool_ptr thrpool, std::string name)
{
    m_proc = proc;
    m_thrpool = thrpool;
}

line::~line()
{
    clear();
}

line_ptr line::ptr()
{
    return shared_from_this();
}

/**
 * @brief Input data to pipeline
 *
 * @param dat
 * @return true
 * @return false
 */
bool line::in(std::any dat)
{
    std::lock_guard<std::mutex> lock(m_inmtx);
    if (!m_thrpool)
        return false;

    auto data = std::make_shared<std::any>(std::move(dat));
    auto self = shared_from_this();

    m_thrpool->post([this, self, data] {
        std::unique_lock<std::timed_mutex> lock(m_proc_mtx);
        if (!m_proc)
        {
            spdlog::debug("pipeline {} proc invalid, drop", m_name);
            return;
        }
        m_proc(self, *data);
    });
}

/**
 * @brief Output data to downstream
 *
 * @param dat
 * @return true
 * @return false
 */
bool line::out(std::any dat)
{
    std::lock_guard<std::mutex> lock(m_outmtx);
    if (m_dslines.empty())
        return false;
    for (auto it : m_dslines)
    {
        it->in(dat);
    }
    return true;
}

bool line::set_proc(line_proc proc, uint32_t to_ms)
{
    std::unique_lock<std::timed_mutex> lock(m_proc_mtx, std::defer_lock);
    if (!lock.try_lock_for(std::chrono::milliseconds(to_ms)))
        return false;
    m_proc = proc;
    return true;
}

line &line::operator+(line &l)
{
    l.add_usline(ptr());
    add_dsline(l.ptr());
    return *this;
}

line &line::operator-(line &l)
{
    l.del_usline(ptr());
    del_dsline(l.ptr());
    return *this;
}

void line::clear()
{
}

line_ptr line::make(std::string name)
{
    return std::make_shared<line>(name);
}

line_ptr line::make(line_proc proc, std::string name)
{
    return std::make_shared<line>(proc, name);
}

line_ptr line::make(line_proc proc, thrpool_ptr thrpool, std::string name)
{
    return std::make_shared<line>(proc, thrpool, name);
}

void line::add_usline(line_ptr p)
{
    std::lock_guard<std::mutex> lock(m_usmtx);
    if (usline_exist(p))
    {
        spdlog::error("{} upstream({}) exist, relink skip", m_name, p->m_name);
        return;
    }
    if (!m_thrpool && p->m_thrpool)
        m_thrpool = p->m_thrpool;
    m_uslines.push_back(p);
}

void line::del_usline(line_ptr p)
{
    std::lock_guard<std::mutex> lock(m_usmtx);
    for (auto it = m_uslines.begin(); it != m_uslines.end();)
    {
        if (*it == p)
        {
            it = m_uslines.erase(it);
            break;
        }
        it++;
    }
}

void line::add_dsline(line_ptr p)
{
    std::lock_guard<std::mutex> lock(m_dsmtx);
    if (dsline_exist(p))
    {
        spdlog::error("{} downstream({}) exist, relink skip", m_name, p->m_name);
        return;
    }
    if (!m_thrpool && p->m_thrpool)
        m_thrpool = p->m_thrpool;
    m_dslines.push_back(p);
}

void line::del_dsline(line_ptr p)
{
    std::lock_guard<std::mutex> lock(m_dsmtx);
    for (auto it = m_dslines.begin(); it != m_dslines.end();)
    {
        if (*it == p)
        {
            it = m_dslines.erase(it);
            break;
        }
        it++;
    }
}

void line::del_usline_all()
{
    std::lock_guard<std::mutex> lock(m_dsmtx);
    auto self = shared_from_this();
    for (auto it : m_uslines)
    {
        it->del_dsline(self);
    }
    m_uslines.clear();
}

void line::del_dsline_all()
{
    std::lock_guard<std::mutex> lock(m_dsmtx);
    auto self = shared_from_this();
    for (auto it : m_dslines)
    {
        it->del_usline(self);
    }
    m_dslines.clear();
}

bool line::usline_exist(line_ptr p)
{
    for (auto it : m_uslines)
    {
        if (it == p)
            return true;
    }
    return false;
}

bool line::dsline_exist(line_ptr p)
{
    for (auto it : m_dslines)
    {
        if (it == p)
            return true;
    }
    return false;
}

} // namespace zmmfly::pipe
