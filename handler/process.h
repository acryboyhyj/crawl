#ifndef PROCESS_H_
#define PROCESS_H_
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include "concurrent_queue.h"
#include "proto/spider.pb.h"
class DocsManager;
class Process {
public:
    Process(const std::shared_ptr<ConcurrentQueue<spiderproto::CrawlDoc>>&
                conocurrent_queue,
            const std::shared_ptr<DocsManager>& docs_manager);
    ~Process();
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;

    void Start();
    void Stop();
    void Join();

    void Run();
    bool Handle();
    bool HandleQueue();
    bool HandleDocs(bool force = false);
    bool HandleDoc(const spiderproto::CrawlDoc& doc);
    bool ShouldDump(int docs_size, bool force);
    void Dump(const spiderproto::CrawlDocs& docs);

private:
    std::string GetToday();
    bool IsDirExist(const std::string& path);
    bool MakePath(const std::string& path);

private:
    int m_dumpsize;
    int m_index;
    std::string m_doc_result_dir;
    const std::shared_ptr<ConcurrentQueue<spiderproto::CrawlDoc>>
        m_concurrent_queue;
    const std::shared_ptr<DocsManager> m_docs_manager;
    std::unique_ptr<std::thread> m_thd;
    std::atomic<bool> m_stop;
};

#endif  // PROCESS_H_
