#include "schedule_client.h"

int main() {
    SchedulerClient scheduler_client(grpc::CreateChannel(
        "localhost:50080", grpc::InsecureChannelCredentials()));

    spiderproto::BasicTask task;
    task.set_name("hyj");
    task.set_user("root");

    std::string taskresponse = scheduler_client.add_task(task);
    std::cout << "client received: " << taskresponse << std::endl;

    return 0;
}
