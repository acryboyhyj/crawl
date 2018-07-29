#!/bin/bash

usage="usage: bash run_fetcher.sh (start|stop)"

if [ $# -lt 1 ]; then
    echo $usage
    exit -1
fi

operation=$1

file_path=$(readlink -f $0)
dir_name=$(dirname $file_path)

crawl="sspider"
addr=127.0.0.1
port=40000
scheduler="127.0.0.1:30000"
handler="127.0.0.1:50000"


cd $dir_name
bin="python -u fetcher.py"
cmd="$bin 
    --name="fetcher1" 
    --addr=$addr:$port
    --crawl=$crawl
    --scheduler=$scheduler
    --handler=$handler"  

    function_start() {
    echo "start with: $cmd"
    ulimit -c unlimited
    rm log.txt
    rm spider.log
    $cmd > log.txt 2>&1 &

    pid=$(ps aux | grep "$bin" | grep "addr=$addr:$port" | awk '{print $2}')
    time_left=1
    while [ $time_left -gt 1 ]; do
        sleep 1
        pid=$(ps aux | grep "$bin" | grep "addr=$addr:$port" | awk '{print $2}')
        if [ "$pid" != "" ]; then
            echo "start $bin with pid $pid"
            return 0
        fi
        time_left=$(($time_left-1))
    done
    return 1
}

function_stop() {
    pid=$(ps aux | grep "$bin" | grep "addr=$addr:$port"  | awk '{print $2}')
    echo "pid:$pid"
    if [ "$pid" == "" ]; then
        echo "$bin not found, regard as stop succ."
        return 0
    fi

    echo "stop $bin with pid $pid"
    kill $pid
    time_left=10
 
}

if [ $operation == "start" ]; then
    function_start
elif [ $operation == "stop" ]; then
    function_stop
else
    echo $usage
fi

succ=$?
if [ $succ -eq 0 ]; then
    echo $bin $ioperation accomplished
else
    echo $bin $operation failed
fi
