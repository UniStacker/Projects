#!/bin/python

from wanikani_api.client import Client
from os import getenv

API_KEY = getenv("WANIKANI_API_KEY")
client = Client(API_KEY)

radicals_1 = client.subjects(types=["radical"], levels=[4], fetch_all=True)

for radical in radicals_1:
    print(radical)

