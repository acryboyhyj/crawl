#include "mysqlpp.h"
#include <glog/logging.h>
#include <mysql++/dbdriver.h>
#include <mysql++/mysql++.h>
#include <algorithm>
#include <map>
#include "taskinfo.h"
#include "util.h"
MySqlpp::MySqlpp()
    : m_connected(false), m_mysql_conn(new mysqlpp::TCPConnection()) {}

bool MySqlpp::Connect() {
    m_connected =
        m_mysql_conn->connect("localhost", "crawl", "root", "hyj147258");
    m_connected = m_mysql_conn->connected();
    if (!m_connected) {
        LOG(ERROR) << "connect erro" << std::endl;
        return false;
    }

    // set options
    mysqlpp::ReconnectOption option(true);
    if (!m_mysql_conn->set_option(&option)) {
        LOG(ERROR) << "can't set reconnect";
    }
    return m_connected;
}

std::string MySqlpp::StrToSql(const std::string& str) const {
    std::string result = str;
    mysqlpp::DBDriver::escape_string_no_conn(&result);
    return "'" + result + "'";
}

bool MySqlpp::InsertTask(const spiderproto::BasicTask& task,
                         const std::string& taskid) {
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
        << StrToSql(taskid) << "," << StrToSql(task.name()) << ","
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
        std::string level = std::to_string(static_cast<int>(crawlurl.level()));
        m_string_stream << "insert into "
                           "tbl_link(taskid, url, level) values("
                        << StrToSql(taskid) << "," << StrToSql(crawlurl.url())
                        << "," << StrToSql(level) << ");";
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
void MySqlpp::UpdateLink(const std::string taskid,
                         const spiderproto::CrawlUrl& crawlurl, int status) {
    if (!Connect()) return;
    m_string_stream.str("");
    std::string url = crawlurl.url();
    m_string_stream << "update tbl_link set code=" << status
                    << " where url=" << StrToSql(url) << ";";
    mysqlpp::Query query = m_mysql_conn->query();
    query << m_string_stream.str();
    query.execute();
}
bool MySqlpp::AddNewLink(const std::string taskid,
                         const spiderproto::CrawlUrl& crawlurl) {
    if (!Connect()) return false;
    std::string url   = crawlurl.url();
    std::string level = std::to_string(static_cast<int>(crawlurl.level()));

    m_string_stream.str("");
    m_string_stream << "insert into "
                       "tbl_link(taskid, url, level) values("
                    << StrToSql(taskid) << "," << StrToSql(url) << ","
                    << StrToSql(level) << ");";
    // LOG(INFO) << m_string_stream.str() << std::endl;
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
        if (row["code"] == "0") {
            for (size_t i = 0; i < btasks->size(); ++i) {
                if ((*btasks)[i].taskid() == row["taskid"].c_str()) {
                    spiderproto::CrawlUrlList* crawl_list =
                        (*btasks)[i].mutable_crawl_list();
                    spiderproto::CrawlUrl* crawlurl =
                        crawl_list->add_crawl_urls();

                    crawlurl->set_url(row["url"].c_str());

                    std::string level = row["level"].c_str();
                    int temp          = std::stoi(level);
                    crawlurl->set_level((spiderproto::UrlLevel)temp);
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
