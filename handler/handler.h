#ifndef _HANDLER_H_
#define _HANDLER_H_
#include <memory>
#include "concurrent_queue.h"
#include "proto/spider.pb.h"
class DocsManager;
class HandleServer;
class Process;
class Handler {
public:
    Handler();
    ~Handler();
    Handler(const Handler&) = delete;
    Handler& operator=(const Handler&) = delete;

    void Start();

    void Join();

private:
    const std::shared_ptr<DocsManager> m_docs_manager;
    const std::shared_ptr<ConcurrentQueue<spiderproto::CrawlDoc>>
        m_concurrent_queue;

    std::unique_ptr<Process> m_process;
    std::unique_ptr<HandleServer> m_handle_server;
};

#endif  // _HANDLER_H_
