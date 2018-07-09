#include <glog/logging.h>
#include <mysql++/mysql++.h>
#include "mysqlpp.h"
#include "proto/spider.pb.h"
#include "scheduler.h"
int main(int argc, char* argv[]) {
    FLAGS_log_dir = "./log";

    google::InitGoogleLogging(argv[0]);

    FLAGS_logtostderr = true;

    Scheduler scheduler1;
    scheduler1.Load();
    LOG(INFO) << "load is done" << std::endl;
    scheduler1.Start();
    LOG(INFO) << "start is done" << std::endl;
    scheduler1.Join();
    google::ShutdownGoogleLogging();
    return 0;
}
