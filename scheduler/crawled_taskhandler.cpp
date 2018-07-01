#include "crawled_taskhandler.h"
#include "mysqlpp.h"
#include "task_manager.h"

CrawledtaskHandler::CrawledtaskHandler(
    const std::shared_ptr<TaskManager>& task_manager,
    const std::shared_ptr<MySqlpp> mysqlpp,
    const std::shared_ptr<ConcurrentQueue<spiderproto::CrawledTask>>
        concurrent_queue)
    : m_task_manager(task_manager),
      m_sqlpp(mysqlpp),
      m_concurrent_queue(concurrent_queue) {}

CrawledtaskHandler::~CrawledtaskHandler() {}
