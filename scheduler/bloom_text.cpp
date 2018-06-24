#include "bloomfilter.h"
#include "glog/logging.h"

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);

    FLAGS_logtostderr = 1;
    BloomFilter bloomfilter(100, 0.2);

    bloomfilter.Insert("http://www.baidu.com");
    if (bloomfilter.KeyMatch("http://www.baidu.com")) {
        std::cout << "yes" << std::endl;
    } else {
        std::cout << "no" << std::endl;
    }

    if (bloomfilter.KeyMatch("http://www.jingdong.com")) {
        std::cout << "yes" << std::endl;
    } else {
        std::cout << "no" << std::endl;
    }
}
