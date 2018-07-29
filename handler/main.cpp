#include <glog/logging.h>
#include "handler.h"
#include "proto/spider.pb.h"

int main(int argc, char* argv[]) {
    FLAGS_log_dir = "./log";

    google::InitGoogleLogging(argv[0]);

    FLAGS_logtostderr = true;

    Handler handler1;
    handler1.Start();
    LOG(INFO) << "handler start is done" << std::endl;
    handler1.Join();
    google::ShutdownGoogleLogging();
    return 0;
}
