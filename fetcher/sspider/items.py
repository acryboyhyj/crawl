# -*- coding: utf-8 -*-

# Define here the models for your scraped items
#
# See documentation in:
# https://doc.scrapy.org/en/latest/topics/items.html

import scrapy


class SspiderItem(scrapy.Item):
    # define the fields for your item here like:
    # name = scrapy.Field()  
    taskid = scrapy.Field()
    url = scrapy.Field()
    actual_url = scrapy.Field()
    status = scrapy.Field()
    content_encoding = scrapy.Field() 
    content = scrapy.Field()
    time_spend = scrapy.Field()

    crawling_task = scrapy.Field()
    extracted_urls = scrapy.Field()


    pass
