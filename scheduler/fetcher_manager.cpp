#include "fetcher_manager.h"
#include <glog/logging.h>
#include <iostream>
#include "debug.h"
#include "fetch_client.h"
FetcherManager::FetcherManager() {}

FetcherManager::~FetcherManager() {}
bool FetcherManager::AddFetcher(
    const std::vector<spiderproto::Fetcher>& fetchers) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& fetcher : fetchers) {
            std::shared_ptr<FetchClient> temp_value(new FetchClient(
                fetcher.name(), fetcher.addr(),
                grpc::CreateChannel(fetcher.addr(),
                                    grpc::InsecureChannelCredentials())));
            m_fetchers[fetcher.name()] = temp_value;
        }
    }
    LOG(INFO) << "add fetcher from db success" << std::endl;
    return true;
}

bool FetcherManager::FindFetcher(const std::string& fetcher_name) {
    if (m_fetchers.find(fetcher_name) != m_fetchers.end()) {
        return true;
    }
    return false;
}

std::vector<std::shared_ptr<FetchClient>> FetcherManager::GetAllFetchers() {
    std::vector<std::shared_ptr<FetchClient>> fetch_clients;
    for (auto& fetcher : m_fetchers) {
        fetch_clients.push_back(fetcher.second);
    }
    return fetch_clients;
}

int FetcherManager::GetClientCount() { return m_fetchers.size(); }

bool FetcherManager::AddFetcher(const spiderproto::Fetcher& fetcher) {
    std::shared_ptr<FetchClient> temp_value(new FetchClient(
        fetcher.name(), fetcher.addr(),
        grpc::CreateChannel(fetcher.addr(),
                            grpc::InsecureChannelCredentials())));
    m_fetchers[fetcher.name()] = temp_value;
    return true;
}

void FetcherManager::ShowFetcher() {
    for (const auto& fetcher : m_fetchers)
        LOG(INFO) << "fetcher name:" << fetcher.first << std::endl;
}
