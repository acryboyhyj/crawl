#ifndef _CRAWL_MYSQLPP_H_
#define _CRAWL_MYSQLPP_H_
#include <mysql++/connection.h>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "proto/spider.pb.h"
namespace mysqlpp {
class Connection;
}
class TaskInfo;
class MySqlpp {
public:
    MySqlpp();
    virtual ~MySqlpp() {}

    bool UpdateTask(const spiderproto::BasicTask& task);
    bool InsertTask(const spiderproto::BasicTask& task,
                    const std::string& taskid);
    std::vector<spiderproto::BasicTask> QueryAllTblTask();
    bool QueryAllTblLink(std::vector<spiderproto::BasicTask>* btasks);

    bool DeleteTask(const spiderproto::BasicTask& task);
    bool AddNewLink(const std::string taskid,
                    const spiderproto::CrawlUrl& crawlurl);
    void UpdateLink(const std::string taskid,
                    const spiderproto::CrawlUrl& crawlurl, int status);

private:
    bool Connect();
    std::string StrToSql(const std::string& str) const;

private:
    mutable std::stringstream m_string_stream;
    mutable std::string m_err_msg;
    bool m_connected;
    std::unique_ptr<mysqlpp::TCPConnection> m_mysql_conn;
};

#endif  // _CRAWL_MYSQLPP_H_
