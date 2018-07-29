#include "handler.h"
#include <vector>
#include "docsmanager.h"
#include "handler_server.h"  // NOLINT
#include "process.h"
Handler::Handler()
    : m_docs_manager(new DocsManager()),
      m_concurrent_queue(new ConcurrentQueue<spiderproto::CrawlDoc>),
      m_process(new Process(m_concurrent_queue, m_docs_manager)),
      m_handle_server(new HandleServer(m_concurrent_queue)) {}

Handler::~Handler() {}

void Handler::Join() {
    m_handle_server->Join();
    m_process->Join();
}
void Handler::Start() {
    m_handle_server->Start();
    m_process->Start();
}
