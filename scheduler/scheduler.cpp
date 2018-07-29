#include "scheduler.h"
#include <glog/logging.h>
#include <mysql++/mysql++.h>
#include <string>
#include <utility>
#include <vector>
#include "concurrent_queue.h"
#include "crawled_taskhandler.h"
#include "dispatcher.h"
#include "fetcher_manager.h"
#include "mysqlpp.h"
#include "schedule_server.h"
#include "task_handler.h"
#include "task_manager.h"
Scheduler::Scheduler()
    : m_task_manager(new TaskManager()),
      m_fetcher_manager(new FetcherManager()),
      m_concurrent_queue(new ConcurrentQueue<spiderproto::CrawledTask>()),
      m_mysqlpp(new MySqlpp()),
      m_task_handler(new TaskHandler(m_task_manager, m_mysqlpp)),
      m_crawledtask_handler(new CrawledtaskHandler(m_task_manager, m_mysqlpp,
                                                   m_concurrent_queue)),
      m_schedule_server(new ScheduleServer(m_fetcher_manager, m_task_handler,
                                           m_concurrent_queue)),

      m_dispatcher(new Dispatcher(m_task_manager, m_fetcher_manager)) {}

Scheduler::~Scheduler() {}

bool Scheduler::Load() {
    std::vector<spiderproto::BasicTask> btasks = m_mysqlpp->QueryAllTblTask();
    if (!m_mysqlpp->QueryAllTblLink(&btasks)) {
        LOG(WARNING) << "query link is failed" << std::endl;
    }
    m_task_manager->AddTask(btasks);
    return false;
}

bool Scheduler::Start() {
    LOG(INFO) << "schedule server and dispathcer will start" << std::endl;

    m_schedule_server->Start();
    m_dispatcher->Start();
    m_crawledtask_handler->Start();
    return true;
}

void Scheduler::Join() {
    m_schedule_server->Join();
    m_dispatcher->Join();
    m_crawledtask_handler->Join();
}
