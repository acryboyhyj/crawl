# -*- coding: utf-8 -*-

import grpc
from proto import spider_pb2
from proto.spider_pb2_grpc import FetchStub


def run():
    conn = grpc.insecure_channel("127.0.0.1:40000")
    client = FetchStub(conn)

    task = spider_pb2.CrawlingTask()
    task.taskid = str("20180503095100-0001")
    
    crawl_url = task.crawl_urls.add()
    crawl_url.url = "https://www.jianshu.com/"
    crawl_url.level = spider_pb2.LEVEL_LIST
   
    linkrule = task.rules.add()
    linkrule.in_level = spider_pb2.LEVEL_LIST
    

    linkrule.rules.append("xpath://ul[@class='note-list']/li")
    
    print "add_crawlingtask with task %s" % str(task)
    response = client.add_crawlingtask(task)
    print "response %s" % str(response)

if __name__ == "__main__":
    run()
