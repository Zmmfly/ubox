#ifndef __ZMMFLY_PIPELINE_H__
#define __ZMMFLY_PIPELINE_H__

#include <any>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "zmmfly/thrpool.h"

namespace zmmfly::pipe
{

class line;
typedef std::shared_ptr<line> line_ptr;
typedef std::function<void(line_ptr, std::any &)> line_proc;
typedef std::shared_ptr<line_ptr> link_id_t;
typedef std::tuple<link_id_t, link_id_t> link_state;

class line : public std::enable_shared_from_this<line>
{

  public:
    line(std::string name = "default");
    line(line_proc proc, std::string name = "default");
    line(line_proc proc, thrpool_ptr thrpool, std::string name = "default");
    ~line();

    line(const line &) = delete;
    const line &operator=(const line &) = delete;

    line_ptr ptr();
    bool in(std::any dat);
    bool out(std::any dat);
    bool set_proc(line_proc proc, uint32_t to_ms = 1000);

    line &operator+(line &l);
    line &operator-(line &l);
    void clear();

  public:
    static line_ptr make(std::string name = "default");
    static line_ptr make(line_proc proc, std::string name = "default");
    static line_ptr make(line_proc proc, thrpool_ptr thrpool, std::string name = "default");

  private:
    void add_usline(line_ptr p);
    void del_usline(line_ptr p);
    void add_dsline(line_ptr p);
    void del_dsline(line_ptr p);

    void del_usline_all();
    void del_dsline_all();

  private:
    bool usline_exist(line_ptr p);
    bool dsline_exist(line_ptr p);

  private:
    std::vector<line_ptr> m_uslines, m_dslines;
    std::mutex m_usmtx, m_dsmtx;

    thrpool_ptr m_thrpool;
    std::string m_name;

    line_proc m_proc;
    std::timed_mutex m_proc_mtx;

    std::mutex m_inmtx, m_outmtx;
};

} // namespace zmmfly::pipe

#endif /* __ZMMFLY_PIPELINE_H__ */
