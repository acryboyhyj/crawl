#include "schedule_server.h"
#include <glog/logging.h>
#include <chrono>
#include <string>
#include "fetcher_manager.h"
#include "task_handler.h"
#include "task_manager.h"

ScheduleServer::ScheduleServer(
    const std::shared_ptr<FetcherManager>& fetcher_manager,
    const std::shared_ptr<TaskHandler>& task_handler,
    const std::shared_ptr<ConcurrentQueue<spiderproto::CrawledTask>>
        concurrent_queue)
    : m_fetcher_manager(fetcher_manager),
      m_task_handler(task_handler),
      m_concurrent_queue(concurrent_queue),
      m_rpc_thd(nullptr),
      m_stop(false) {}

ScheduleServer::~ScheduleServer() {}

grpc::Status ScheduleServer::add_task(grpc::ServerContext* context,
                                      const spiderproto::BasicTask* task,
                                      spiderproto::TaskResponse* response) {
    LOG(INFO) << "schedulerserver receive a new task, return taskid"
              << std::endl;
    if (task->taskid() == "") {
        m_task_handler->AddTask(*task);
        response->set_taskid(task->taskid());
    } else {
        if (m_task_handler->Exsit(task->taskid())) {
            m_task_handler->UpdateTask(*task);
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
        LOG(INFO) << "the fetcher have existed,can't add";
    } else {
        if (m_fetcher_manager->AddFetcher(*fetcher)) {
            LOG(INFO) << "add fetcher success: " << fetcher->name();
        } else {
            LOG(INFO) << "add fetcher failed: " << fetcher->name();
        }
    }

    return grpc::Status::OK;
}

grpc::Status ScheduleServer::add_crawledtask(
    grpc::ServerContext* context, const spiderproto::CrawledTask* task,
    spiderproto::TaskResponse* response) {
    LOG(INFO) << "receive a CrawledTask:" << task->taskid();
    m_concurrent_queue->push(*task);

    return grpc::Status::OK;
}

void ScheduleServer::RunServer() {
    std::string server_address("0.0.0.0:30000");

    ScheduleServer service(m_fetcher_manager, m_task_handler,
                           m_concurrent_queue);
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
