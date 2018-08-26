#include "process.h"
#include <errno.h>     // errno, ENOENT, EEXIST
#include <sys/stat.h>  // stat
#include <ctime>
#include <fstream>
#include <utility>
#include <vector>
#include "docsmanager.h"
#include "proto/spider.pb.h"
Process::Process(const std::shared_ptr<ConcurrentQueue<spiderproto::CrawlDoc>>&
                     conocurrent_queue,
                 const std::shared_ptr<DocsManager>& docs_manager)
    : m_dumpsize(1024),
      m_index(0),
      m_doc_result_dir("docresult"),
      m_concurrent_queue(conocurrent_queue),
      m_docs_manager(docs_manager),
      m_thd(nullptr),
      m_stop(false) {}

Process::~Process() {}

void Process::Start() {
    m_thd.reset(new std::thread(&Process::Run, this));
    LOG(INFO) << "handler's process module start";
}

void Process::Join() { m_thd->join(); }

void Process::Stop() { m_stop.store(true, std::memory_order_relaxed); }
void Process::Run() {
    LOG(INFO) << "process run";
    while (!m_stop.load(std::memory_order_relaxed)) {
        if (!Handle()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    HandleQueue();
    HandleDocs();
}

bool Process::Handle() {
    bool result = false;
    if (HandleQueue()) {
        result = true;
    }
    if (HandleDocs()) {
        result = true;
    }
    return result;
}
bool Process::HandleQueue() {
    if (m_concurrent_queue->empty()) {
        return false;
    }

    while (!m_concurrent_queue->empty()) {
        spiderproto::CrawlDoc doc;
        m_concurrent_queue->pop(doc);
        if (HandleDoc(doc)) {
            return true;
        }
    }

    LOG(INFO) << "handle queue";
    return true;
}
// force default value is false
bool Process::HandleDocs(bool force) {
    std::vector<std::string> dumped_taskids{};
    for (auto& docs : m_docs_manager->GetAllDocs()) {
        if (ShouldDump(docs.docs_size(), force)) {
            Dump(docs);
            LOG(INFO) << "dump a docs with taskid:" << docs.taskid();
            dumped_taskids.push_back(docs.taskid());
        }
    }

    for (auto& taskid : dumped_taskids) {
        m_docs_manager->DelDocs(taskid);
        LOG(INFO) << "delete docs with taskid:" << taskid;
    }

    return dumped_taskids.size() > 0;
}

bool Process::HandleDoc(const spiderproto::CrawlDoc& doc) {
    if (m_docs_manager->Exist(doc.taskid())) {
        m_docs_manager->AddCrawlDocToOldDocs(doc);
        return m_docs_manager->GetOneDocsSize(doc.taskid()) > m_dumpsize;
    } else {
        m_docs_manager->AddCrawlDocToNewDocs(doc);
        return false;
    }
}

bool Process::ShouldDump(int docs_size, bool force) {
    // if (force || docs_size >= m_dumpsize) {
    //    return true;
    //}
    return true;
}

void Process::Dump(const spiderproto::CrawlDocs& docs) {
    std::string today = GetToday();
    std::string path  = m_doc_result_dir + "/" + today + "/";
    MakePath(path);
    LOG(INFO) << "make path: " << path;

    std::string filename_prefix = docs.taskid() + "-" + std::to_string(m_index);
    std::string filename        = path + filename_prefix + ".docs";

    LOG(INFO) << "dump file " << filename << " and taskid is " << docs.taskid();

    std::ofstream outfile;
    outfile.open(filename, std::ios::binary | std::ios::out);
    if (outfile.is_open()) {
        if (!docs.SerializeToOstream(&outfile)) {
            LOG(INFO) << "serial to ostream fail";
        }

        LOG(INFO) << "debugstring:" << docs.DebugString();
    }
    outfile.close();

    std::ifstream input(filename, std::ios::in | std::ios::binary);
    spiderproto::CrawlDocs docs2;
    docs2.ParseFromIstream(&input);
    input.close();
    LOG(INFO) << "debugstring:" << docs2.DebugString();

    m_index += 1;
    if (m_index >= 9000) {
        m_index = 0;
    }
}

std::string Process::GetToday() {
    std::time_t now_time = std::time(nullptr);
    char timestr[100];
    if (std::strftime(timestr, sizeof(timestr), "%Y%m%d",
                      std::localtime(&now_time))) {
        return std::string(timestr);
    }
    return "";
}

bool Process::IsDirExist(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

bool Process::MakePath(const std::string& path) {
    mode_t mode = 0755;
    int ret     = mkdir(path.c_str(), mode);
    if (ret == 0) return true;

    switch (errno) {
        case ENOENT:
            // parent didn't exist, try to create it
            {
                std::string::size_type pos = path.find_last_of('/');
                if (pos == std::string::npos) return false;
                if (!MakePath(path.substr(0, pos))) return false;
            }
            // now, try to create again
            return 0 == mkdir(path.c_str(), mode);

        case EEXIST:
            // done!
            return IsDirExist(path);

        default:
            return false;
    }
}
