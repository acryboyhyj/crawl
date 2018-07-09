#include "schedule_client.h"
int main() {
    SchedulerClient scheduler_client(grpc::CreateChannel(
        "localhost:50080", grpc::InsecureChannelCredentials()));

    spiderproto::BasicTask task;
    task.set_name("hdu");
    task.set_user("root");

    spiderproto::CrawlUrlList* crawlurl_list = task.mutable_crawl_list();
    spiderproto::CrawlUrl* crawl_url         = crawlurl_list->add_crawl_urls();
    crawl_url->set_url("http://www.hdu.edu.cn/news/important");
    crawl_url->set_level(spiderproto::LEVEL_LIST);

    spiderproto::LinkRuleList* rule_list = task.mutable_rule_list();
    spiderproto::LinkRule* linkrule      = rule_list->add_rules();
    linkrule->add_rules(
        "xpath:/html/body/div[@class='SearchPage "
        "general']/div[@class='m-wrap']/div[@class='content "
        "c']/div[@class='list-box']/div[@class='search-list']/"
        "div[@class='con']/ul");

    linkrule->set_in_level(spiderproto::LEVEL_LIST);

    spiderproto::Runtime* runtime = task.mutable_runtime();
    runtime->set_download_delay(1.0);
    std::string taskresponse = scheduler_client.add_task(task);
    std::cout << "client received: " << taskresponse << std::endl;

    return 0;
}
