#include "crawled_taskhandler.h"
#include <mysql++/mysql++.h>
#include "mysqlpp.h"
#include "task_manager.h"
#include "taskinfo.h"
CrawledtaskHandler::CrawledtaskHandler(
    const std::shared_ptr<TaskManager>& task_manager,
    const std::shared_ptr<MySqlpp> mysqlpp,
    const std::shared_ptr<ConcurrentQueue<spiderproto::CrawledTask>>
        concurrent_queue,
    const std::shared_ptr<BloomFilter<std::string>> bf)
    : m_task_manager(task_manager),
      m_mysqlpp(mysqlpp),
      m_concurrent_queue(concurrent_queue),
      m_bf(bf),
      m_thread(nullptr),
      m_stop(false) {}

CrawledtaskHandler::~CrawledtaskHandler() {}

void CrawledtaskHandler::AddCrawledTask() {
    while (!m_stop.load(std::memory_order_relaxed)) {
        // read a task  from queue
        // bloomfilter
        // write mysqlpp
        // wrie taskmanager
        spiderproto::CrawledTask cdtask;
        m_concurrent_queue->pop(cdtask);

        std::shared_ptr<TaskInfo> task =
            m_task_manager->FindTask(cdtask.taskid());
        if (task == nullptr) {
            LOG(INFO) << "task is nullptr";
            continue;
        }

        task->DelCrawledUrl(cdtask.crawl_url());
        m_mysqlpp->UpdateLink(cdtask.taskid(), cdtask.crawl_url(),
                              cdtask.status());
        for (int i = 0; i < cdtask.links_size(); ++i) {
            std::string url = cdtask.links(i).url();
            if (sizeof(url) > 200) continue;
            if (!m_bf->contains(url)) {
                m_bf->insert(url);
                task->AddCrawlUrl(cdtask.links(i));
                m_mysqlpp->AddNewLink(cdtask.taskid(), cdtask.links(i));
            }
        }
    }
}

void CrawledtaskHandler::Start() {
    m_thread.reset(new std::thread(&CrawledtaskHandler::AddCrawledTask, this));
}

void CrawledtaskHandler::Join() { m_thread->join(); }

void CrawledtaskHandler::Stop() {
    m_stop.store(true, std::memory_order_relaxed);
}
