#include "handler_client.h"
int main() {
    HandlerClient handler_client(grpc::CreateChannel(
        "localhost:50000", grpc::InsecureChannelCredentials()));

    spiderproto::CrawlDoc doc;
    doc.set_taskid("0000000011111");
    doc.set_url("www.baidu.com");

    doc.set_status(200);
    doc.set_content("hello , every one");

    spiderproto::Storage* storage = doc.mutable_storage();
    storage->set_dest("www.w.ww.w.w");
    std::string docresponse = handler_client.add_crawldoc(doc);
    std::cout << "client received: " << docresponse << std::endl;

    return 0;
}
