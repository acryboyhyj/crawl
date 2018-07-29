#include "docsmanager.h"
#include <glog/logging.h>
DocsManager::DocsManager() {}
DocsManager::~DocsManager() {}

void DocsManager::AddCrawlDocToNewDocs(const spiderproto::CrawlDoc& doc) {
    std::lock_guard<std::mutex> lock(m_mutex);
    spiderproto::CrawlDocs docs;
    docs.set_taskid(doc.taskid());
    spiderproto::CrawlDoc* temp_doc = docs.add_docs();
    temp_doc->CopyFrom(doc);
    m_docs[doc.taskid()] = docs;
}

void DocsManager::AddCrawlDocToOldDocs(const spiderproto::CrawlDoc& doc) {
    std::lock_guard<std::mutex> lock(m_mutex);

    spiderproto::CrawlDoc* temp_doc = m_docs[doc.taskid()].add_docs();
    temp_doc->CopyFrom(doc);
}

bool DocsManager::Exist(const std::string& taskid) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_docs.find(taskid) != m_docs.end()) {
        return true;

    } else {
        return false;
    }
}

std::vector<spiderproto::CrawlDocs> DocsManager::GetAllDocs() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<spiderproto::CrawlDocs> result{};
    for (const auto& docs : m_docs) {
        result.push_back(docs.second);
    }
    return result;
}

void DocsManager::DelDocs(const std::string& taskid) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (taskid.empty()) {
        LOG(INFO) << "taskid is empty,delldocs not work out";
    }

    m_docs.erase(taskid);
}

int DocsManager::GetOneDocsSize(const std::string& taskid) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_docs[taskid].docs_size();
}
