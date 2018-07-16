#include "schedule_client.h"
int main() {
    SchedulerClient scheduler_client(grpc::CreateChannel(
        "localhost:30000", grpc::InsecureChannelCredentials()));

    spiderproto::BasicTask task;
    task.set_name("hdu");
    task.set_user("root");

    spiderproto::CrawlUrlList* crawlurl_list = task.mutable_crawl_list();
    spiderproto::CrawlUrl* crawl_url         = crawlurl_list->add_crawl_urls();
    crawl_url->set_url("https://www.jianshu.com/");
    crawl_url->set_level(spiderproto::LEVEL_LIST);

    spiderproto::LinkRuleList* rule_list = task.mutable_rule_list();
    spiderproto::LinkRule* linkrule      = rule_list->add_rules();
    linkrule->add_rules("xpath://ul[@class='note-list']/li");

    linkrule->set_in_level(spiderproto::LEVEL_LIST);

    spiderproto::Runtime* runtime = task.mutable_runtime();
    runtime->set_download_delay(1.0);
    runtime->set_concurrent_reqs(10);
    std::string taskresponse = scheduler_client.add_task(task);
    std::cout << "client received: " << taskresponse << std::endl;

    return 0;
}
