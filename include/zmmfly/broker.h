#ifndef __ZMMFLY_BROKER_H__
#define __ZMMFLY_BROKER_H__

#include "zmmfly/thrpool.h"
#include <any>
#include <atomic>

namespace zmmfly::broker
{

typedef std::function<void(std::wstring&, std::any&)> handler_fn_t;
typedef std::function<bool(std::wstring,std::wstring)> matcher_fn_t;

typedef struct sub_info_t {
    std::wstring topic;
    handler_fn_t fn;
    std::atomic<uint32_t> ecnt;
}sub_info_t;

typedef std::shared_ptr<sub_info_t> sub_info_ptr;
typedef std::tuple<std::wstring, std::any> pub_info_t;
typedef std::shared_ptr<pub_info_t> pub_info_ptr;

bool wstr_split(std::vector<std::wstring> &out, std::wstring src, std::wstring dlim);
bool topic_matcher_mqtt(std::wstring pub, std::wstring sub);

class core;

template<class BR=core, typename ID=sub_info_ptr>
class sub_guard
{
    public:
        sub_guard(BR &br, ID id) :m_br(br), m_id(id){}
        ~sub_guard() { m_br.unsub(m_id); }

    private:
        BR m_br;
        ID m_id;
};


class core
{
    public:
        core(thrpool_ptr thrpool, matcher_fn_t fn = topic_matcher_mqtt);
        ~core();

        bool pub(std::wstring topic, std::any msg);
        sub_info_ptr sub(std::wstring topic, handler_fn_t fn);
        bool unsub(sub_info_ptr info);
        void clear();

    private:
        void pub_handler(pub_info_ptr pinfo);
        void sub_executer(pub_info_ptr pinfo, sub_info_ptr sinfo);

    private:
        thrpool_ptr m_thrpool;
        std::mutex m_submtx;
        std::vector<sub_info_ptr> m_subs;

        matcher_fn_t m_matcher;
};

typedef std::shared_ptr<core> broker_ptr;

}

#endif /* __ZMMFLY_BROKER_H__ */
