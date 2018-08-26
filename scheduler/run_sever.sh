#! /bin/bash

usage="usage:bash run_scheduler.sh (stop|compile)"

if [ $# -lt 1 ]; then
	echo $usage
fi
operation=$1

function_compile(){
	bin="g++ -g -o scheduler  main.cpp scheduler.cpp schedule_server.cpp dispatcher.cpp task_handler.cpp crawled_taskhandler.cpp task_manager.cpp mysqlpp.cpp fetcher_manager.cpp taskinfo.cpp fetch_client.cpp util.cpp  proto/spider.grpc.pb.cc proto/spider.pb.cc  -lmysqlclient -lmysqlpp  -lgrpc++ -lprotobuf -lglog -lgflags -lpthread -Wall -Werror -std=c++11"
	echo "compile:$bin"	
	$bin
	return 0
}

function_stop(){
	pid=$(ps -ef | grep "./scheduler"| grep -v grep | awk '{print $2}')
	echo $pid
	if [ "$pid" == "" ]; then
		return 0
	else
		echo "kill $pid"
		kill $pid
		return 0
	fi

}

if [ $operation == "compile" ]; then
	function_compile
elif [ $operation == "stop" ]; then
	function_stop
else
	echo $usage
fi


