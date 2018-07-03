#include "dispatcher.h"
#include <glog/logging.h>
#include <algorithm>
#include <random>
#include "fetch_client.h"
#include "fetcher_manager.h"
#include "proto/spider.pb.h"
#include "task_manager.h"
#include "taskinfo.h"
Dispatcher::Dispatcher(const std::shared_ptr<TaskManager>& task_manager,
                       const std::shared_ptr<FetcherManager> fetch_manager)
    : m_task_manager(task_manager),
      m_fetcher_manager(fetch_manager),
      m_dispatch_thd(nullptr),
      m_stop(false) {}

Dispatcher::~Dispatcher() {}

void Dispatcher::Start() {
    m_dispatch_thd.reset(new std::thread(&Dispatcher::DispatchInternal, this));
}

void Dispatcher::Join() { m_dispatch_thd->join(); }

void Dispatcher::Stop() { m_stop.store(true, std::memory_order_relaxed); }

void Dispatcher::DispatchInternal() {
    LOG(INFO) << "DispatchInternal start";
    while (!m_stop.load(std::memory_order_relaxed)) {
        m_seq++;
        std::shared_ptr<TaskInfo> taskinfo = m_task_manager->GetOptTask();
        taskinfo->SetSeq(m_seq.load(std::memory_order_relaxed));

        std::vector<std::shared_ptr<FetchClient>> fetch_clients =
            m_fetcher_manager->GetAllFetchers();

        int client_count = m_fetcher_manager->GetClientCount();
        int url_count    = taskinfo->GetCrawlurlCount();

        LOG(INFO) << client_count << " " << url_count;
        std::vector<std::vector<spiderproto::CrawlUrl>> tasks =
            taskinfo->SpilitTask(client_count, url_count);
        for (const auto& vec : tasks) {
            for (const auto& c : vec) {
                LOG(INFO) << c.url();
            }
        }
        // shuffle
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(fetch_clients.begin(), fetch_clients.end(), g);

        for (size_t i = 0; i < tasks.size(); i++) {
            spiderproto::CrawlingTask crawling_task = CombineCrawlingTask(
                *taskinfo, fetch_clients[i]->GetName(), tasks[i]);
            LOG(INFO) << "add_crawlingtask " << crawling_task.fetcher();
            fetch_clients[i]->add_crawlingtask(crawling_task);
        }
    }
}

spiderproto::CrawlingTask Dispatcher::CombineCrawlingTask(
    const TaskInfo& taskinfo, const std::string fetcher_name,
    std::vector<spiderproto::CrawlUrl> urls) {
    spiderproto::CrawlingTask crawling_task;
    spiderproto::BasicTask btask = taskinfo.GetBasicTask();
    crawling_task.set_taskid(btask.taskid());
    crawling_task.set_fetcher(fetcher_name);
    for (auto& url : taskinfo.GetAllCrawlurl()) {
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
