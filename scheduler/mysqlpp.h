#ifndef _CRAWL_MYSQLPP_H_
#define _CRAWL_MYSQLPP_H_
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "proto/spider.pb.h"
namespace mysqlpp {
class Connection;
}

class MySqlpp {
public:
    MySqlpp() : m_connected(false) {}
    virtual ~MySqlpp() {}

    bool UpdateTask(const spiderproto::BasicTask& task);
    bool InsertTask(const spiderproto::BasicTask& task);
    std::vector<spiderproto::BasicTask> QueryAllTblTask();
    bool QueryAllTblLink(std::vector<spiderproto::BasicTask>* btasks);
    bool DeleteTask(const spiderproto::BasicTask& task);

    std::vector<spiderproto::Fetcher> QueryAllFetchers();
    bool UpdateFetchers(const spiderproto::Fetcher& fetcher);
    bool InsertFetchers(const spiderproto::Fetcher& fetcher);
    bool DeleteFetcher(const spiderproto::Fetcher& fetcher);

private:
    bool Connect();
    std::string formatsql(spiderproto::CrawlUrl crawurl);

    std::string StrToSql(const std::string& str) const;

private:
    mutable std::stringstream m_string_stream;
    mutable std::string m_err_msg;
    bool m_connected;
    std::unique_ptr<mysqlpp::Connection> m_mysql_conn;
};

#endif  // _CRAWL_MYSQLPP_H_
