#!/usr/bin/python
import json

from collections import OrderedDict


DATA_FILENAME = 'data/data.json'
OUTPUT_FILENAME = 'data/data.pat'

FIELDS = OrderedDict()
FIELDS['max'] = (-40.0, 150.0) # max (high) temp
FIELDS['min'] = (-40.0, 150.0) # min (low)  temp
FIELDS['avg'] = (-40.0, 150.0) # average    temp
FIELDS['dep'] = (-40.0, 150.0) # departure from normal average
FIELDS['wtr'] = (0.0, 50.0)   # amount of rain
FIELDS['snw'] = (0.0, 50.0)   # amount of snow
FIELDS['dpth'] = (0.0, 50.0)  # depth of ground snow
FIELDS['avg_spd'] = (0.0, 80.0) # average wind speed
FIELDS['mx_spd'] = (0.0, 80.0) # maximum average wind speed
FIELDS['min_dir'] = (0.0, 360.0) # average wind direction
FIELDS['pk_dir'] = (0.0, 360.0) # peak wind direction
FIELDS['pk_spd'] = (0.0, 100.0) # peak wind speed

print [FIELDS[k] for k in FIELDS]

data = json.load(open(DATA_FILENAME, 'r'))

out_file = open(OUTPUT_FILENAME, 'w')

# data['2013']['12']['24']['max'] for instance

def iterate(data):
    for y in sorted(data, key=int):
        for m in sorted(data[y], key=int):
            for d in sorted(data[y][m], key=int):
                yield (y, m, d, data[y][m][d])

def size(data):
    return sum(1 for d in iterate(data))

def scale(num, minimum, maximum):
    if num is None:
        # no data for some reason
        # we will pretend it's right in the center
        return (minimum + maximum)/2
    num, minimum, maximum = float(num), float(minimum), float(maximum)
    if num < minimum or num > maximum:
        raise ValueError("num should be between minimum and maximum")
    offset = num - minimum
    scaled = offset / (maximum - minimum)
    assert 0 <= scaled <= 1
    return scaled

if __name__ == '__main__':
    # actual output stuff
    # preliminaries
    # how many layers, etc
    out_file.write('%d %d %d %d' % (len(FIELDS), len(FIELDS), 20, 20) + '\n')
    # unused
    out_file.write('%d %d\n' % (size(data) * 0.9, size(data)*0.1))


    prev_element = None
    for (y, m, d, item) in iterate(data):
        if not prev_element:
            prev_element = (y, m, d, item)
        else:
            # input is prev_element
            # output is this_element
            this_element = (y, m, d, item)
            
            # so, output prev_element first...
            out_file.write(' '.join(
                str(scale(prev_element[3][k], *FIELDS[k]))
                for k in FIELDS
            ))
            out_file.write(' # input %s/%s/%s' % prev_element[0:3])
            out_file.write('\n')

            # and then current element
            out_file.write(' '.join(
                str(scale(this_element[3][k], *FIELDS[k]))
                for k in FIELDS
            ))
            out_file.write(' # output %s/%s/%s' % this_element[0:3])
            out_file.write('\n')

            # finally update prev_element
            prev_element = this_element
