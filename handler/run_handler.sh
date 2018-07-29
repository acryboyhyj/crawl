#! /bin/bash



bin="g++ -o handler main.cpp handler.cpp process.cpp handler_server.cpp docsmanager.cpp proto/spider.grpc.pb.cc proto/spider.pb.cc -lgrpc++ -lprotobuf -lglog -lgflags -lpthread -Wall"

echo "compile:$bin"
$bin
cmd="g++ -o handler_client handler_client.cpp proto/spider.grpc.pb.cc proto/spider.pb.cc -lgrpc++ -lprotobuf -Wall"

echo "compile:$cmd"
$cmd



