#include "zmmfly/broker.h"

namespace zmmfly::broker
{

core::core(thrpool_ptr thrpool, matcher_fn_t fn)
{
    m_thrpool = thrpool;
    m_matcher = fn;
}

core::~core() {
    clear();
}

bool core::pub(std::wstring topic, std::any msg)
{
    if (topic.empty())
        return false;
    if (!m_thrpool)
        return false;
    auto info = std::make_shared<pub_info_t>(std::make_tuple(std::move(topic), std::move(msg)));
    auto fn = std::bind(&core::pub_handler, this, info);
    m_thrpool->post(fn);
    return true;
}

sub_info_ptr core::sub(std::wstring topic, handler_fn_t fn)
{
    auto info = std::make_shared<sub_info_t>();
    info->topic = std::move(topic);
    info->fn = std::move(fn);

    std::lock_guard<std::mutex> lock(m_submtx);
    m_subs.push_back(info);
    return info;
}

bool core::unsub(sub_info_ptr info)
{
    bool erased = false;
    std::lock_guard<std::mutex> lock(m_submtx);
    for (auto it = m_subs.begin(); it != m_subs.end();)
    {
        if (*it == info)
        {
            m_subs.erase(it);
            erased = true;
            break;
        }
    }
    if (!erased)
        return false;
    while (info->ecnt != 0)
        std::this_thread::yield();
    return true;
}

void core::clear() {
    for (auto it = m_subs.begin(); it != m_subs.end();) {
        while((*it)->ecnt != 0) std::this_thread::yield();
        it = m_subs.erase(it);
    }
}

void core::pub_handler(pub_info_ptr pinfo)
{
    std::lock_guard<std::mutex> lock(m_submtx);
    if (!m_matcher)
        return;
    for (auto it : m_subs)
    {
        auto &ptopic = std::get<0>(*pinfo);
        auto &stopic = it->topic;
        auto &ecnt = it->ecnt;
        if (m_matcher(ptopic, stopic))
        {
            ecnt++;
            auto fn = std::bind(&core::sub_executer, this, pinfo, it);
            m_thrpool->post(fn);
        }
    }
}

void core::sub_executer(pub_info_ptr pinfo, sub_info_ptr sinfo)
{
    if (sinfo->fn)
    {
        sinfo->fn(std::get<0>(*pinfo), std::get<1>(*pinfo));
    }
    sinfo->ecnt--;
}

bool wstr_split(std::vector<std::wstring> &out, std::wstring src, std::wstring dlim)
{
    if (src.empty())
        return false;
    size_t pos = 0;
    size_t dlen = dlim.length();
    while (pos < src.length())
    {
        size_t p2 = src.find(dlim, pos);
        if (p2 == std::wstring::npos)
        {
            out.push_back(src.substr(pos));
            break;
        }
        out.push_back(src.substr(pos, p2 - pos));
        pos = p2 + dlen;
    }
    return true;
}

bool topic_matcher_mqtt(std::wstring pub, std::wstring sub)
{
    std::vector<std::wstring> vecPub, vecSub;
    wstr_split(vecPub, pub, L"/");
    wstr_split(vecSub, sub, L"/");

    if (vecPub.size() < vecSub.size())
        return false;
    for (size_t i = 0; i < vecSub.size(); i++)
    {
        if (vecSub.at(i) == L"#")
            break;
        if (vecSub.at(i) == L"+")
            continue;
        if (vecSub.at(i) != vecPub.at(i))
            return false;
    }
    return true;
}

} // namespace zmmfly::broker
