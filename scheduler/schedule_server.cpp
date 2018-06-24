#include "schedule_server.h"
#include <glog/logging.h>
#include <chrono>
#include <string>
#include "fetcher_manager.h"
#include "task_manager.h"
ScheduleServer::ScheduleServer(
    const std::shared_ptr<TaskManager>& task_manager,
    const std::shared_ptr<FetcherManager>& fetcher_manager)
    : m_task_manager(task_manager),
      m_fetcher_manager(fetcher_manager),
      m_rpc_thd(nullptr),
      m_stop(false),
      m_bloom_filter(new BloomFilter(100, 0.1)) {}

ScheduleServer::~ScheduleServer() {}

grpc::Status ScheduleServer::add_task(grpc::ServerContext* context,
                                      const spiderproto::BasicTask* task,
                                      spiderproto::TaskResponse* response) {
    LOG(INFO) << "schedulerserver receive a new task, return taskid"
              << std::endl;
    if (task->taskid() == "") {
        std::string taskid = m_task_manager->AddTask(*task);
        response->set_taskid(taskid);
    } else {
        if (m_task_manager->FindTaskInfo(task->taskid())) {
            m_task_manager->UpdateTask(*task);
            response->set_taskid(task->taskid());
        } else {
            LOG(INFO) << "there are not this task" << std::endl;
        }
    }

    return grpc::Status::OK;
}

grpc::Status ScheduleServer::add_fetcher(grpc::ServerContext* context,
                                         const spiderproto::Fetcher* fetcher,
                                         spiderproto::TaskResponse* response) {
    std::cout << "schedulerserver receive a new task for add_fetcher"
              << std::endl;
    if (m_fetcher_manager->FindFetcher(fetcher->name())) {
        std::cout << "the fetcher have existed,can't add" << std::endl;
    } else {
        if (m_fetcher_manager->AddFetcher(*fetcher)) {
            LOG(INFO) << "add fetcher success" << std::endl;
        } else {
            LOG(INFO) << "add fetcher failed" << std::endl;
        }
    }

    return grpc::Status::OK;
}

grpc::Status ScheduleServer::add_crawledtask(
    grpc::ServerContext* context, const spiderproto::CrawledTask* task,
    spiderproto::TaskResponse* response) {
    LOG(INFO) << "receive a CrawledTask" << std::endl;
    int newlink_count = task->links_size();
    for (int i = 0; i < newlink_count; i++) {
        if (!m_bloom_filter->KeyMatch(task->links(i).url())) {
            m_bloom_filter->Insert(task->links(i).url());
            m_task_manager->AddCrawlUrl(task->taskid(), task->links(i));
        }
    }

    return grpc::Status::OK;
}

void ScheduleServer::RunServer() {
    std::string server_address("0.0.0.0:50080");

    ScheduleServer service(m_task_manager, m_fetcher_manager);

    grpc::ServerBuilder builder;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    LOG(INFO) << "Server listening on " << server_address << std::endl;

    server->Wait();

    while (!m_stop.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void ScheduleServer::Start() {
    m_rpc_thd.reset(new std::thread(&ScheduleServer::RunServer, this));
}

void ScheduleServer::Join() { m_rpc_thd->join(); }

void ScheduleServer::Stop() { m_stop.store(true, std::memory_order_relaxed); }
