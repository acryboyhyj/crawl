# -*- coding: utf-8 -*-

import grpc
from proto import spider_pb2
from proto.spider_pb2_grpc import FetchStub


def run():
    conn = grpc.insecure_channel("127.0.0.1:50000")
    client = FetchStub(conn)

    task = spider_pb2.CrawlingTask()
    task.taskid = str("Test-20180503095100-0001")
    
    crawl_url = task.crawl_urls.add()
    crawl_url.url = "http://www.hdu.edu.cn/news/important"
    crawl_url.url_levels.append(spider_pb2.LEVEL_LIST)
   
    linkrule = task.rules.add()
    linkrule.in_level = 1
    

    linkrule.rules.append('xpath://*[@class="foot"]')
    
    linkrule.url_levels.append(spider_pb2.LEVEL_CONTENT)
    linkrule.out_level = 1
        print "add_crawlingtask with task %s" % str(task)
    response = client.add_crawlingtask(task)
    print "response %s" % str(response)

if __name__ == "__main__":
    run()
