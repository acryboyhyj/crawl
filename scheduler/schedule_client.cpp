#include "schedule_client.h"

spiderproto::BasicTask SchedulerClient::GetTask(std::string name,
                                                std::string url, double delay,
                                                int reqs) {
    spiderproto::BasicTask task;
    task.set_name(name);
    task.set_user("root");

    spiderproto::CrawlUrlList* crawlurl_list = task.mutable_crawl_list();
    spiderproto::CrawlUrl* crawl_url         = crawlurl_list->add_crawl_urls();
    crawl_url->set_url(url);
    crawl_url->set_level(spiderproto::LEVEL_LIST);

    spiderproto::CrawlUrl* crawl_url1 = crawlurl_list->add_crawl_urls();
    crawl_url1->set_url(
        "https://language.chinadaily.com.cn/news_bilingual/page_10.html");
    crawl_url1->set_level(spiderproto::LEVEL_LIST);

    spiderproto::LinkRuleList* rule_list = task.mutable_rule_list();
    spiderproto::LinkRule* linkrule      = rule_list->add_rules();
    linkrule->add_rules(
        R"(re:/news_bilingual/page_\d+\.html)");
    linkrule->set_in_level(spiderproto::LEVEL_LIST);
    linkrule->set_out_level(spiderproto::LEVEL_LIST);

    spiderproto::LinkRule* linkrule1 = rule_list->add_rules();
    linkrule1->add_rules(
        R"(re:language.chinadaily.com.cn/a.*?\.html)");
    linkrule1->set_in_level(spiderproto::LEVEL_LIST);
    linkrule1->set_out_level(spiderproto::LEVEL_CONTENT);
    std::cout << task.DebugString();

    spiderproto::LinkRule* linkrule2 = rule_list->add_rules();
    linkrule2->add_rules(
        R"(re:language.chinadaily.com.cn/a.*?\.html)");

    linkrule2->set_in_level(spiderproto::LEVEL_CONTENT);
    linkrule2->set_out_level(spiderproto::LEVEL_CONTENT);

    spiderproto::Runtime* runtime = task.mutable_runtime();

    runtime->set_download_delay(delay);
    runtime->set_concurrent_reqs(reqs);
    return task;
}
int main() {
    SchedulerClient scheduler_client(grpc::CreateChannel(
        "localhost:30000", grpc::InsecureChannelCredentials()));
    std::string url_shuangyu =
        "https://language.chinadaily.com.cn/news_bilingual";
    std::string name = "shuangyu";
    spiderproto::BasicTask task =
        scheduler_client.GetTask(name, url_shuangyu, 1.0, 24);
    std::string taskresponse = scheduler_client.add_task(task);
    std::cout << task.DebugString();
}
