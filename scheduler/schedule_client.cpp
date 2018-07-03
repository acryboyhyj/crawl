#include "schedule_client.h"
int main() {
    SchedulerClient scheduler_client(grpc::CreateChannel(
        "localhost:50080", grpc::InsecureChannelCredentials()));

    spiderproto::BasicTask task;
    task.set_taskid("2018070352");
    task.set_name("tqc");
    task.set_user("root");

    spiderproto::CrawlUrlList* crawlurl_list = task.mutable_crawl_list();
    spiderproto::CrawlUrl* crawl_url         = crawlurl_list->add_crawl_urls();
    crawl_url->set_url("http://www.hdu.edu.cn/news/important");
    crawl_url->add_url_levels(spiderproto::LEVEL_LIST);

    spiderproto::LinkRuleList* rule_list = task.mutable_rule_list();
    spiderproto::LinkRule* linkrule      = rule_list->add_rules();
    linkrule->add_rules(
        "xpath:/html/body/div[@class='SearchPage "
        "general']/div[@class='m-wrap']/div[@class='content "
        "c']/div[@class='list-box']/div[@class='search-list']/"
        "div[@class='con']/ul/li[@class='cur']/div[@class='title cur']");

    linkrule->set_in_level(spiderproto::LEVEL_LIST);
    linkrule->set_out_level(spiderproto::LEVEL_CONTENT);

    std::string taskresponse = scheduler_client.add_task(task);
    std::cout << "client received: " << taskresponse << std::endl;

    return 0;
}
