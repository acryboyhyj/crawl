#! /bin/bash

bin="g++ -g -o scheduler  main.cpp scheduler.cpp schedule_server.cpp dispatcher.cpp task_handler.cpp crawled_taskhandler.cpp task_manager.cpp mysqlpp.cpp fetcher_manager.cpp taskinfo.cpp fetch_client.cpp util.cpp  proto/spider.grpc.pb.cc proto/spider.pb.cc  -lmysqlclient -lmysqlpp  -lgrpc++ -lprotobuf -lglog -lgflags -lpthread -Wall -Werror"

echo "compile:$bin"
$bin



