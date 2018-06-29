#!/bin/bash

usage="usage: bash run_fetcher.sh start"

if [ $# -lt 1 ]; then
    echo $usage
    exit -1
fi

operation=$1

file_path=$(readlink -f $0)
dir_name=$(dirname $file_path)

crawl="sspider"
addr=127.0.0.1
port=50000
scheduler="127.0.0.1:50080"


cd $dir_name
bin="python -u fetcher.py"
cmd="$bin 
    --name="fetcher1" 
    --addr=$addr:$port
    --crawl=$crawl
    --scheduler=$scheduler" 
  #  --handler=$handler  
function_start() {
    echo "start with: $cmd"
    rm log.txt
    rm spider.log
    $cmd > log.txt 2>&1 &
    return 1
}


if [ $operation == "start" ]; then
    function_start
    echo $usage
fi

