#include "handler_server.h"  // NOLINT
#include <glog/logging.h>
#include <string>
HandleServer::HandleServer(
    const std::shared_ptr<ConcurrentQueue<spiderproto::CrawlDoc>>&
        concurrent_queue)
    : m_concurrent_queue(concurrent_queue), m_rpc_thd(nullptr), m_stop(false) {}
HandleServer::~HandleServer() {}

grpc::Status HandleServer::add_crawldoc(grpc::ServerContext* context,
                                        const spiderproto::CrawlDoc* crawldoc,
                                        spiderproto::TaskResponse* response) {
    LOG(INFO) << "receive a crawldoc,taskid: " << crawldoc->taskid();
    m_concurrent_queue->push(*crawldoc);
    return grpc::Status::OK;
}

void HandleServer::RunServer() {
    std::string server_address("0.0.0.0:50000");

    HandleServer service(m_concurrent_queue);
    grpc::ServerBuilder builder;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    LOG(INFO) << "Hanle Server listening on " << server_address << std::endl;

    server->Wait();
    while (!m_stop.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void HandleServer::Start() {
    m_rpc_thd.reset(new std::thread(&HandleServer::RunServer, this));
}

void HandleServer::Join() { m_rpc_thd->join(); }

void HandleServer::Stop() { m_stop.store(true, std::memory_order_relaxed); }
