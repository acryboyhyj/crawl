#ifndef DOCSMANAGER_H_
#define DOCSMANAGER_H_
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "proto/spider.pb.h"
class DocsManager {
public:
    DocsManager();
    ~DocsManager();

    DocsManager(const DocsManager&) = delete;
    DocsManager& operator=(const DocsManager&) = delete;
    bool Exist(const std::string& taskid);
    void AddCrawlDocToNewDocs(const spiderproto::CrawlDoc&);
    void AddCrawlDocToOldDocs(const spiderproto::CrawlDoc&);

    std::vector<spiderproto::CrawlDocs> GetAllDocs() const;

    void DelDocs(const std::string&);

    int GetOneDocsSize(const std::string& taskid);

private:
    mutable std::mutex m_mutex;
    std::map<std::string, spiderproto::CrawlDocs> m_docs;
};

#endif  // DOCSMANAGER_H_#endif
