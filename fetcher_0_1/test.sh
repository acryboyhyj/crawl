#!/bin/bash
usage="usage: bash run_fetcher.sh (start|stop)"

operation=$1

file_path=$(readlink -f $0)
dir_name=$(dirname $file_path)
echo "$file_path"
echo "$dir_name"

inst=$(basename $(dirname $(readlink -f $0)))
name=`echo $inst | cut -d "_" -f1`
echo "$inst"
echo "$name"
if [ "$name" == "$inst" ]; then
    replica_id=10
else
    replica_id=`echo $inst | awk -F"_" '{printf("%d", $NF)}'`
fi

addr=127.0.0.1
crawl="sspider"
port=$(($replica_id+30000))
echo "$port"
scheduler="127.0.0.1:30000"
handler="127.0.0.1:50000"


