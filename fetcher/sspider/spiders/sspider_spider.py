# -*- coding: utf-8 -*-

import scrapy
import os,sys
import time
import Queue
import re

from scrapy.linkextractors import LinkExtractor
from sspider.items import SspiderItem
from proto.spider_pb2 import *

import copy
import urllib

import logging
logger = logging.getLogger(__name__)

class SspiderSpider(scrapy.Spider):
    name = "sspider"
    queue = Queue.Queue()
    
    def __init__(self, *args, **kwargs):
        super(SspiderSpider, self).__init__(*args, **kwargs)
        self.stopping = False

    def stop(self):
        self.stopping = True
    
    def start_requests(self):
        print"start_requests\n"
        
        while not self.stopping:
            if self.queue.empty():
                yield None
            else:
                task = self.queue.get()
                if len(task.crawl_urls) == 0:
                    yield None
                else:
                    taskid = task.taskid
                    url = urllib.unquote(task.crawl_urls[0].url)
                    item = SspiderItem(taskid=taskid, url=url,\
                            crawling_task=task)
                    
                    yield scrapy.Request(url=url, meta={'item':item}, \
                            dont_filter=True, callback=self.parse)

    
    def parse(self, response):
        item = response.meta['item']
        item["actual_url"] = response.url
        item["status"] = response.status
        if "Content-Encoding" in response.headers:
            item["content_encoding"] = response.headers.get("Content-Encoding")
        content = item.get('content', "")
        item["content"] = response.body
        item['time_spend'] = response.meta.get("download_latency", -1)

        logger.debug("start extract links for %s" % response.url)
        
        extracted_urls = self._extract_links(response, item)
        item["extracted_urls"] = extracted_urls
        return item

    def _extract_links(self, response, item):
        crawling_task = item["crawling_task"]
        crawl_url = crawling_task.crawl_urls[0]

        extracted_urls = []
        for link_rule in crawling_task.rules:
            logger.debug("start to check rule %s" % link_rule.in_level)
            if not self._should_rule(link_rule, crawl_url):
                continue
            logger.debug("apply rule %s to url %s" \
                    % (link_rule.in_level, response.url))
            urls = self._extract_crawlurls(link_rule, crawling_task, response)
            extracted_urls.extend(urls)
        
        return extracted_urls

    def _should_rule(self, link_rule, crawl_url):
        if link_rule.in_level ==  crawl_url.level :
            return True
        return False

    def _extract_crawlurls(self, link_rule, crawling_task, response):
        """Return CrawlUrls"""
        logger.debug("cuurent rule %s" % str(link_rule))

        re_patterns = []
        xpaths = []
        for rule in link_rule.rules:
            if rule.startswith("re:"):
                logger.debug("apply re pattern %s" % rule[3:])
                re_patterns.append(rule[3:])
            elif rule.startswith("xpath:"):
                logger.debug("apply xpath %s" % rule[6:])
                xpaths.append(rule[6:])
            else:
                logger.error("unsupported rule %s for url %s from task %s" \
                        % (rule, response.url, crawling_task.taskid.taskid))

        logger.debug("xpaths:  %s" %xpaths)
        link_extractor = LinkExtractor(allow=re_patterns,restrict_xpaths=xpaths)
        links = link_extractor.extract_links(response)
        logger.debug("links : %s" %str(links))
        extracted_urls = []
        for link in links:
            crawl_url = CrawlUrl()
            crawl_url.level = link_rule.out_level
            crawl_url.url = link.url
            extracted_urls.append(crawl_url)
            logger.debug("extract link %s" % str(crawl_url))        

        return extracted_urls

