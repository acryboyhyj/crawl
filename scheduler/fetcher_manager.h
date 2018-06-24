#ifndef _FETCHER_MANAGER_
#define _FETCHER_MANAGER_
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "proto/spider.pb.h"
class FetchClient;
class FetcherManager {
public:
    FetcherManager();
    ~FetcherManager();
    bool AddFetcher(const std::vector<spiderproto::Fetcher>& fetchers);
    bool FindFetcher(const std::string& fetcher_name);
    bool AddFetcher(const spiderproto::Fetcher& fetcher);
    std::vector<std::shared_ptr<FetchClient>> GetAllFetchers();
    int GetClientCount();

    void ShowFetcher();

private:
    std::map<std::string, std::shared_ptr<FetchClient>> m_fetchers;
    std::mutex m_mutex;
};

#endif  //_FETCHER_MANAGER_
