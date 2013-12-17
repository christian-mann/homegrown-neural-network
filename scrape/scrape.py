#!/usr/bin/python

import itertools
import json
import os
import pprint
import sys
import threading

from collections import defaultdict
from multiprocessing.pool import ThreadPool

import requests

from bs4 import BeautifulSoup as bs

MONTHS = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Oct', 'Nov', 'Dec']
YEARS = range(1983, 1990)

BASE_URL = 'http://www.srh.noaa.gov/tsa/f6/f6.php?siteName=Tulsa_International_Airport&month=%s&year=%d'

def scrape():
    data = defaultdict(dict)
    threads = {}

    p = ThreadPool(8)
    map(parseYear, [(y, data[y]) for y in YEARS])

    return data

def parseYear((y, data)): # data is by "reference"
    print y
    for m in MONTHS:
        print m
        data[MONTHS.index(m)+1] = scrapePage(m, y)
    return data


def scrapePage(m, y):
    req = requests.get(BASE_URL % (m, y))
    req.raise_for_status()

    soup = bs(req.text)
    tables = soup.select('div#localcontent > table > tr > td > p')
    if len(tables) != 1:
        raise Exception("Page format not recognized")
    table = tables[0]

    lines = table.text.replace(u'\xa0', ' ').split('\n')

    # fingerprint -- which style is it?
    ret = None
    if 'WS FORM: F-6' in lines[0]:
        ret = scrapePageA(lines)
    elif lines[0].startswith('TULSA OKLAHOMA LOCAL CLIMATOLOGICAL DATA'):
        ret = scrapePageB(lines)
    elif lines[0].startswith('PRELIMINARY LOCAL CLIMATOLOGICAL DATA') and lines[2].startswith('LATITUDE'):
        # roughly 1990 - 2000
        ret = scrapePageC(lines)
    elif lines[0].startswith('PRELIMINARY LOCAL CLIMATOLOGICAL DATA') and lines[4].startswith('LATITUDE'):
        # roughly 1983 - 1989
        ret = scrapePageD(lines)
    else:
        print lines[0]
        raise Exception("table format not recognized")

    for d in ret:
        ret[d]['month'] = MONTHS.index(m)
        ret[d]['year'] = y


def scrapePageA(lines):
    # the actual data begins two lines after the second line with ========
    i = 0
    while '========' not in lines[i]: i += 1
    i += 1
    while '========' not in lines[i]: i += 1
    i += 2
    # and ends one line before the next one
    j = i
    while '========' not in lines[j]: j+= 1

    dataLines = lines[i:j] # beautiful

    data = {}
    for line in dataLines:
        record = parseLineA(line)
        data[record['day']] = record
    return data

def parseLineA(line):
    data = {}
    data['day'] = int(line[0:2])
    # temperature
    data['max'] = tfloat(line[3:6])
    data['min'] = tfloat(line[7:10])
    data['avg'] = tfloat(line[11:14])
    data['dep'] = tfloat(line[15:18])
    #data['hdd'] = int(line[19:22]) # derived from AVG
    #data['cdd'] = int(line[23:26]) # derived from AVG
    data['wtr'] = tfloat(line[28:31])
    # snow
    data['snw'] = tfloat(line[33:36])
    data['dpth'] = tfloat(line[37:41])
    # wind
    data['avg_spd'] = tfloat(line[42:46])
    data['mx_spd'] = tfloat(line[46:49])
    data['pk_spd'] = tfloat(line[73:76]) # yes, it's out of order
    data['pk_dir'] = tdirection(line[77:80]) # yes, it's out of order
    data['min_dir'] = tdirection(line[50:52])
    # cloud cover
    data['cover'] = tfloat(line[63:66])
    return data

def scrapePageB(lines):
    i = 9
    j = i
    while not lines[j].startswith('SUM'):
        j += 1
    dataLines = lines[i:j]
    data = {}
    for line in dataLines:
        record = parseLineB(line)
        data[record['day']] = record
    return data

def parseLineB(line):
    data = {}
    data['day'] = int(line[0:3])
    data['max'] = tfloat(line[5:8])
    data['min'] = tfloat(line[11:14])
    data['avg'] = tfloat(line[17:20])
    data['dep'] = tfloat(line[23:26])
    data['wtr'] = tfloat(line[41:46])
    data['snw'] = tfloat(line[48:55])
    data['dpth'] = tfloat(line[57:66])
    data['avg_spd'] = tfloat(line[68:73])
    data['mx_spd'] = tfloat(line[75:80])
    data['min_dir'] = tdirection(line[82:86])
    data['pk_dir'] = tdirection(line[123:126])
    data['pk_spd'] = tfloat(line[127:130])
    return data

def scrapePageC(lines):
    i = 10
    j = i
    while not lines[j].startswith('SUM'):
        j += 1
    dataLines = lines[i:j]
    data = {}
    for line in dataLines:
        record = parseLineC(line)
        data[record['day']] = record
    return data

def parseLineC(line):
    return parseLineB(line) # this just might work
    

def scrapePageD(lines):
    i = 15
    j = i
    while not lines[j].startswith('SUM'):
        j += 1
    dataLines = lines[i:j]
    data = {}
    for line in dataLines:
        record = parseLineD(line)
        data[record['day']] = record
    return data

def parseLineD(line):
    data = {}
    data['day'] = int(line[0:3])
    data['max'] = tfloat(line[5:8])
    data['min'] = tfloat(line[11:14])
    data['avg'] = tfloat(line[17:20])
    data['dep'] = tfloat(line[23:26])
    data['wtr'] = tfloat(line[41:46])
    data['snw'] = tfloat(line[48:55])
    data['dpth'] = tfloat(line[57:66])
    data['avg_spd'] = tfloat(line[68:73])
    data['mx_spd'] = tfloat(line[75:80])
    data['min_dir'] = tdirection(line[82:86])
    data['pk_dir'] = tdirection(line[123:126])
    data['pk_spd'] = tfloat(line[127:130])
    return data


def tfloat(s):
    s = s.strip()
    s = s.replace('O', '0') # >_>
    s = s.replace('(', '') # >_>
    if s.strip() == 'T': return 0.1 # trace, w/e
    if s.strip() == 'N': return 0.0 # sure
    if s.strip() == 'M': return None # no data
    if s.strip() == '': return 0.0 # I guess?
    if s.strip() == '0..': return 0.0 # people are dumb
    else: return float(s)

def tdirection(s):
    if s.strip() == 'N': return 360.0
    if s.strip() == 'NE': return 45.0
    if s.strip() == 'E': return 90.0
    if s.strip() == 'SE': return 135.0
    if s.strip() == 'S': return 180.0
    if s.strip() == 'SW': return 225.0
    if s.strip() == 'W': return 270.0
    if s.strip() == 'NW': return 315.0
    else: return tfloat(s)

if __name__ == '__main__':
    data = scrape()
    with open('data/data.json', 'w') as f:
        json.dump(data, f)
