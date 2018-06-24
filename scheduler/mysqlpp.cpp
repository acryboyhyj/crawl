#include "mysqlpp.h"
#include <glog/logging.h>
#include <mysql++/dbdriver.h>
#include <mysql++/mysql++.h>
#include <algorithm>
#include <map>
#include <typeinfo>
#include "taskinfo.h"
#include "util.h"
bool MySqlpp::Connect() {
    m_mysql_conn.reset(new mysqlpp::Connection());
    m_connected =
        m_mysql_conn->connect("crawl", "localhost", "root", "hyj147258");
    if (!m_connected) {
        LOG(ERROR) << "connect erro" << std::endl;
        return m_connected;
    }
    return m_connected;
}

std::string MySqlpp::StrToSql(const std::string& str) const {
    std::string result = str;
    mysqlpp::DBDriver::escape_string_no_conn(&result);
    return "'" + result + "'";
}

std::string MySqlpp::formatsql(spiderproto::CrawlUrl crawlurl) {
    std::vector<std::string> items;
    for (auto level : crawlurl.url_levels()) {
        items.push_back(std::to_string(static_cast<int>(level)));
    }
    std::string value = Join(items, ",");  // 如果是char的话，就','
    return value;
}

bool MySqlpp::InsertTask(const spiderproto::BasicTask& task) {
    if (!Connect()) return false;
    std::string storage;
    task.storage().SerializeToString(&storage);
    std::string rule_list;
    task.rule_list().SerializeToString(&rule_list);
    std::string feature;
    task.feature().SerializeToString(&feature);
    std::string run_time;
    task.runtime().SerializeToString(&run_time);

    m_string_stream.str("");
    m_string_stream
        << "insert into "
           "tbl_task(taskid, name, user, feature, storage, runtime, rule_"
           "list) values("
        << StrToSql(task.taskid()) << "," << StrToSql(task.name()) << ","
        << StrToSql(task.user()) << "," << StrToSql(StringToHex(feature)) << ","
        << StrToSql(StringToHex(storage)) << ","
        << StrToSql(StringToHex(run_time)) << ","
        << StrToSql(StringToHex(rule_list)) << ");";

    LOG(INFO) << m_string_stream.str() << std::endl;
    mysqlpp::Query query = m_mysql_conn->query();
    query << m_string_stream.str();
    query.execute();

    int crawlurl_count = task.crawl_list().crawl_urls_size();
    for (int i = 0; i < crawlurl_count; ++i) {
        const spiderproto::CrawlUrl crawlurl = task.crawl_list().crawl_urls(i);

        m_string_stream.str("");
        std::string levels = formatsql(crawlurl);
        m_string_stream << "insert into "
                           "tbl_link(taskid, url, levels) values("
                        << StrToSql(task.taskid()) << ","
                        << StrToSql(crawlurl.url()) << "," << StrToSql(levels)
                        << ");";
        LOG(INFO) << m_string_stream.str() << std::endl;
        query.reset();
        query << m_string_stream.str();
        query.execute();
    }
    return true;
}

bool MySqlpp::UpdateTask(const spiderproto::BasicTask& task) {
    if (!Connect()) return false;

    std::string storage;
    task.storage().SerializeToString(&storage);
    std::string rule_list;
    task.rule_list().SerializeToString(&rule_list);
    std::string feature;
    task.feature().SerializeToString(&feature);
    std::string run_time;
    task.runtime().SerializeToString(&run_time);

    m_string_stream.str("");
    m_string_stream << "update tbl_task "
                       "set name="
                    << StrToSql(task.name())
                    << ", user=" << StrToSql(task.user())
                    << ", feature=" << StrToSql(StringToHex(feature))
                    << ", storage=" << StrToSql(StringToHex(storage))
                    << ", runtime=" << StrToSql(StringToHex(run_time))
                    << ", rule_list=" << StrToSql(StringToHex(rule_list))
                    << "where taskid=" << StrToSql(task.taskid()) << ";";
    LOG(INFO) << m_string_stream.str() << std::endl;
    mysqlpp::Query query = m_mysql_conn->query();
    query << m_string_stream.str();
    query.execute();

    return true;
}

std::vector<spiderproto::BasicTask> MySqlpp::QueryAllTblTask() {
    if (!Connect()) return {};

    mysqlpp::Query query = m_mysql_conn->query();
    query << "select* from tbl_task";

    LOG(INFO) << "select* from tbl_task" << std::endl;

    mysqlpp::UseQueryResult res = query.use();

    std::vector<spiderproto::BasicTask> btasks;

    while (mysqlpp::Row row = res.fetch_row()) {
        spiderproto::BasicTask task;

        task.set_taskid(row["taskid"].c_str());
        task.set_name(row["name"].c_str());
        task.set_user(row["user"].c_str());

        std::string feature;
        HexToString(row["feature"].c_str(), &feature);
        task.mutable_feature()->ParseFromString(feature);

        std::string storage;
        HexToString(row["storage"].c_str(), &storage);
        task.mutable_storage()->ParseFromString(storage);

        std::string runtime;
        HexToString(row["runtime"].c_str(), &runtime);
        task.mutable_runtime()->ParseFromString(runtime);

        std::string rule_list;
        HexToString(row["rule_list"].c_str(), &rule_list);
        task.mutable_rule_list()->ParseFromString(rule_list);

        btasks.push_back(task);
    }
    if (m_mysql_conn->errnum()) {
        LOG(WARNING) << "erro in fetch_row" << std::endl;
    }

    return btasks;
}

bool MySqlpp::QueryAllTblLink(std::vector<spiderproto::BasicTask>* btasks) {
    if (!Connect()) return false;
    mysqlpp::Query query = m_mysql_conn->query();

    query << "select * from tbl_link";
    LOG(INFO) << "queryalltbllink";
    mysqlpp::UseQueryResult res = query.use();

    while (mysqlpp::Row row = res.fetch_row()) {
        for (size_t i = 0; i < btasks->size(); ++i) {
            if ((*btasks)[i].taskid() == row["taskid"].c_str()) {
                spiderproto::CrawlUrlList* crawl_list =
                    (*btasks)[i].mutable_crawl_list();

                spiderproto::CrawlUrl* crawlurl = crawl_list->add_crawl_urls();

                crawlurl->set_url(row["url"].c_str());

                std::string levels                 = row["levels"].c_str();
                std::vector<std::string> levelsvec = Split(levels, ',');
                for (const auto& level : levelsvec) {
                    int tempint = std::stoi(level);
                    crawlurl->add_url_levels((spiderproto::UrlLevel)tempint);
                }
            }
        }
    }
    if (m_mysql_conn->errnum()) {
        LOG(WARNING) << "erro in fetch_row" << std::endl;
    }
    return true;
}

bool MySqlpp::DeleteTask(const spiderproto::BasicTask& task) {
    if (!Connect()) return false;
    mysqlpp::Query query = m_mysql_conn->query();

    m_string_stream.str("");
    m_string_stream << "delete from tbl_task "
                    << "where taskid=" << StrToSql(task.taskid()) << ";";
    query << m_string_stream.str();
    query.execute();
    m_string_stream.str("");
    m_string_stream << "delete from tbl_link "
                    << "where taskid=" << StrToSql(task.taskid()) << ";";
    query.reset();
    query << m_string_stream.str();
    query.execute();
    LOG(INFO) << "delete task" << std::endl;
    return true;
}

std::vector<spiderproto::Fetcher> MySqlpp::QueryAllFetchers() {
    if (!Connect()) {
        std::cout << "connect faled,QueryAllFetchers failed" << std::endl;
        return {};
    }
    mysqlpp::Query query = m_mysql_conn->query();

    m_string_stream.str("");
    m_string_stream << "select* from fetcher";

    LOG(INFO) << m_string_stream.str() << std::endl;
    query << m_string_stream.str();

    mysqlpp::UseQueryResult res = query.use();

    std::vector<spiderproto::Fetcher> fetchers;
    while (mysqlpp::Row row = res.fetch_row()) {
        spiderproto::Fetcher temp_fecher;
        temp_fecher.set_name(row["name"].c_str());
        temp_fecher.set_addr(row["addr"].c_str());
        fetchers.push_back(temp_fecher);
    }

    return fetchers;
}

bool MySqlpp::UpdateFetchers(const spiderproto::Fetcher& fetcher) {
    if (!Connect()) return false;

    m_string_stream.str("");
    m_string_stream << "update fetcher "
                       "set name="
                    << StrToSql(fetcher.name())
                    << ", addr=" << StrToSql(fetcher.addr()) << ";";
    mysqlpp::Query query = m_mysql_conn->query();
    query << m_string_stream.str();
    query.execute();
    LOG(INFO) << "update fetcher" << std::endl;
    return true;
}

bool MySqlpp::InsertFetchers(const spiderproto::Fetcher& fetcher) {
    if (!Connect()) return false;

    m_string_stream.str("");

    m_string_stream << "insert into fetcher(name,addr) values("
                    << StrToSql(fetcher.name()) << ","
                    << StrToSql(fetcher.addr()) << ");";
    LOG(INFO) << m_string_stream.str() << std::endl;
    mysqlpp::Query query = m_mysql_conn->query();
    query << m_string_stream.str();
    query.execute();

    return true;
}

bool MySqlpp::DeleteFetcher(const spiderproto::Fetcher& fetcher) {
    if (!Connect()) return false;

    m_string_stream.str("");
    m_string_stream << "delete from fetcher"
                    << "where name=" << fetcher.name() << ";";
    LOG(INFO) << m_string_stream.str() << std::endl;
    mysqlpp::Query query = m_mysql_conn->query();
    query << m_string_stream.str();
    query.execute();

    return true;
}
