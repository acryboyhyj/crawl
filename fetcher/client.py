# -*- coding: utf-8 -*-

import grpc
from proto import spider_pb2
from proto.spider_pb2_grpc import FetchStub


def run():
    conn = grpc.insecure_channel("127.0.0.1:40000")
    client = FetchStub(conn)

    task = spider_pb2.CrawlingTask()
    task.taskid = str("201809011234")
    crawl_url=task.crawl_urls.add()
    crawl_url.url = "https://language.chinadaily.com.cn/news_bilingual/page_10.html"
    crawl_url.level  = spider_pb2.LEVEL_LIST

   
    linkrule1 = task.rules.add()
    linkrule1.in_level = spider_pb2.LEVEL_LIST 
    linkrule1.out_level = spider_pb2.LEVEL_LIST
    linkrule1.rules.append("re:/news_bilingual/page_\d+\.html")



    linkrule2 = task.rules.add()
    linkrule2.in_level = spider_pb2.LEVEL_LIST
    linkrule2.out_level = spider_pb2.LEVEL_CONTENT
    linkrule2.rules.append("re:language.chinadaily.com.cn/a.*?\.html")

    linkrule3 = task.rules.add()
    linkrule3.in_level = spider_pb2.LEVEL_CONTENT
    linkrule3.out_level = spider_pb2.LEVEL_CONTENT
    linkrule3.rules.append("re:language.chinadaily.com.cn/a.*?\.html")


    print "add_crawlingtask with task %s" % str(task)
    response = client.add_crawlingtask(task)

if __name__ == "__main__":
    run()
