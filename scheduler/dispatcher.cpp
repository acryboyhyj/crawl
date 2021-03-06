#include "dispatcher.h"
#include <glog/logging.h>
#include <algorithm>
#include <chrono>
#include <random>
#include "fetch_client.h"
#include "fetcher_manager.h"
#include "proto/spider.pb.h"
#include "task_manager.h"
#include "taskinfo.h"

Dispatcher::Dispatcher(const std::shared_ptr<TaskManager>& task_manager,
                       const std::shared_ptr<FetcherManager>& fetch_manager)
    : m_task_manager(task_manager),
      m_fetcher_manager(fetch_manager),
      m_dispatch_thd(nullptr),
      m_stop(false),
      m_seq(0),
      m_g(std::random_device()()) {}

Dispatcher::~Dispatcher() {}

void Dispatcher::Start() {
    m_dispatch_thd.reset(new std::thread(&Dispatcher::DispatchInternal, this));
}

void Dispatcher::Join() { m_dispatch_thd->join(); }

void Dispatcher::Stop() { m_stop.store(true, std::memory_order_relaxed); }

bool Dispatcher::ShouldDispatch() {
    if (!HaveSurviveFetcher()) {
        return false;
    }

    if (m_task_manager->Empty()) return false;

    return true;
}
void Dispatcher::DispatchInternal() {
    LOG(INFO) << "DispatchInternal start";
    while (!m_stop.load(std::memory_order_relaxed)) {
        if (!ShouldDispatch()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        m_seq++;
        // find a  min seq task
        std::shared_ptr<TaskInfo> taskinfo = m_task_manager->GetOptTask();

        if (taskinfo == nullptr) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            continue;
        }

        Distribute(taskinfo);
        taskinfo->SetSeq(m_seq);
    }
}
void Dispatcher::ControlSpeed(int* send_url_count,
                              const std::shared_ptr<TaskInfo>& taskinfo) {
    int speed       = taskinfo->Getspeed();
    int url_count   = taskinfo->GetCrawlurlCount();
    int urled_count = taskinfo->GetCrawlingUrlCount();
    if (speed != 0) {
        *send_url_count = speed - urled_count;
        if (*send_url_count > url_count) {
            *send_url_count = url_count;
        }
    }
}

void Dispatcher::MaxSpeed(int* send_url_count, int url_count) {
    if (*send_url_count <= url_count) {
        *send_url_count = 1000;
    } else {
        *send_url_count = url_count;
    }
    LOG(INFO) << "this task diapathch url " << *send_url_count;
}
bool Dispatcher::Distribute(const std::shared_ptr<TaskInfo>& taskinfo) {
    int client_count   = m_fetcher_manager->GetClientCount();
    int send_url_count = 0;
    ControlSpeed(&send_url_count, taskinfo);

    // if actual_distribure_count is zero , will return a empty task
    if (send_url_count == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return false;
    }

    std::vector<std::vector<spiderproto::CrawlUrl>> tasks =
        SpilitTask(taskinfo, client_count, send_url_count);

    // shuffle
    std::vector<std::shared_ptr<FetchClient>> fetch_clients =
        m_fetcher_manager->GetAllFetchers();

    if (fetch_clients.size() == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return false;
    }
    std::shuffle(fetch_clients.begin(), fetch_clients.end(), m_g);

    for (size_t i = 0; i < tasks.size(); i++) {
        spiderproto::CrawlingTask crawling_task = CombineCrawlingTask(
            taskinfo, fetch_clients[i]->GetName(), tasks[i]);
        LOG(INFO) << "add_crawlingtask  " << crawling_task.taskid() << " to "
                  << crawling_task.fetcher();
        LOG(INFO) << "fetch_client port:" << fetch_clients[i]->GetName() << " ,"
                  << fetch_clients[i]->GetAddr();
        fetch_clients[i]->add_crawlingtask(crawling_task);
    }
    return true;
}

std::vector<std::vector<spiderproto::CrawlUrl>> Dispatcher::SpilitTask(
    const std::shared_ptr<TaskInfo>& taskinfo, int client_count,
    int url_count) {
    std::vector<std::vector<spiderproto::CrawlUrl>> result{};
    if (url_count == 0 || client_count == 0) return result;
    int every_url_count = url_count / client_count + 1;

    std::vector<spiderproto::CrawlUrl> temp{};

    std::vector<spiderproto::CrawlUrl> urls = taskinfo->GetCrawlurl(url_count);

    int count = 0;
    for (int i = 0; i < url_count; i++) {
        temp.push_back(urls[i]);
        count++;
        if (count == every_url_count) {
            result.push_back(temp);
            temp.clear();
            count = 0;
        }
    }
    result.push_back(temp);
    return result;
}

spiderproto::CrawlingTask Dispatcher::CombineCrawlingTask(
    const std::shared_ptr<TaskInfo>& taskinfo, const std::string& fetcher_name,
    std::vector<spiderproto::CrawlUrl>& urls) {
    spiderproto::CrawlingTask crawling_task;
    spiderproto::BasicTask btask = taskinfo->GetBasicTask();
    crawling_task.set_taskid(btask.taskid());

    crawling_task.set_fetcher(fetcher_name);
    for (auto& url : urls) {
        spiderproto::CrawlUrl* purl = crawling_task.add_crawl_urls();
        purl->CopyFrom(url);
    }
    spiderproto::LinkRuleList* rules = btask.mutable_rule_list();
    int rule_count                   = rules->rules_size();
    for (int i = 0; i < rule_count; i++) {
        spiderproto::LinkRule* temp_value = crawling_task.add_rules();
        temp_value->CopyFrom(rules->rules(i));
    }
    spiderproto::Storage* storage = crawling_task.mutable_storage();
    storage->CopyFrom(btask.storage());
    return crawling_task;
}

bool Dispatcher::HaveSurviveFetcher() {
    if (m_fetcher_manager->GetAllFetchers().size() == 0)
        return false;
    else
        return true;
}
