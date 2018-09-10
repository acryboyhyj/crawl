#include "fetch_client.h"
#include <glog/logging.h>
FetchClient::FetchClient(const std::string& name, const std::string& addr,
                         std::shared_ptr<grpc::Channel> channel)
    : m_name(name),
      m_addr(addr),
      m_stub(spiderproto::Fetch::NewStub(channel)) {}

std::string FetchClient::add_crawlingtask(
    const spiderproto::CrawlingTask& crawling_task) {
    spiderproto::CrawlingTask ctask;
    ctask.CopyFrom(crawling_task);

    spiderproto::TaskResponse response;

    grpc::ClientContext context;

    grpc::Status status = m_stub->add_crawlingtask(&context, ctask, &response);

    if (status.ok()) {
        LOG(INFO) << "add crawlingTask rpc success :" << ctask.taskid()
                  << std::endl;
        return response.taskid();
    } else {
        LOG(INFO) << status.error_code() << ":" << status.error_message()
                  << std::endl;
        return "add crawlingtask rpc failed";
    }
}
std::string FetchClient::GetName() { return m_name; }

std::string FetchClient::GetAddr() { return m_addr; }
