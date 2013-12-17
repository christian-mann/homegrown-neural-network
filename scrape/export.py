#!/usr/bin/python
import json


DATA_FILENAME = 'data/data.json'
OUTPUT_FILENAME = 'data/data.pat'

FIELDS = [
    #'day',
    'max',
    'min',
    'avg',
    'dep',
    'wtr',
    'snw',
    'dpth',
    'avg_spd',
    'mx_spd',
    'min_dir',
    'pk_dir',
    'pk_spd',
]

data = json.load(open(DATA_FILENAME, 'r'))

out_file = open(OUTPUT_FILENAME, 'w')

# data['2013']['12']['24']['max'] for instance

def iterate(data):
    for y in sorted(data, key=int):
        for m in sorted(data[y], key=int):
            for d in sorted(data[y][m], key=int):
                yield (y, m, d, data[y][m][d])

# actual output stuff
# preliminaries
#out_file.write('#' + '  '.join(FIELDS) + '\n')
# how many layers, etc
out_file.write('%d %d %d %d' % (len(FIELDS), len(FIELDS), 20, 20) + '\n')
# unused
out_file.write('4 4' + '\n')


prev_element = None
for (y, m, d, item) in iterate(data):
    if not prev_element:
        prev_element = (y, m, d, item)
    else:
        # input is prev_element
        # output is this_element
        this_element = (y, m, d, item)
        
        # so, output prev_element first...
        out_file.write(' '.join(str(prev_element[3][k]) for k in FIELDS))
        out_file.write(' # input %s/%s/%s' % prev_element[0:3])
        out_file.write('\n')

        # and then current element
        out_file.write(' '.join(str(this_element[3][k]) for k in FIELDS))
        out_file.write(' # output %s/%s/%s' % this_element[0:3])
        out_file.write('\n')

        # finally update prev_element
        prev_element = this_element
