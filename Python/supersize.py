#!/usr/bin/env python3
import time
import sys
import os

os.environ['LD_LIBRARY_PATH'] = '/opt/mysql/5.6.25/lib' # Needed for CEXT
import mysql.connector

from math import log

def pretty_size(n,pow=0,b=1024,u='B',pre=['']+[p+'i'for p in'KMGTPEZY']):
    """Pretty print storage length

    http://stackoverflow.com/questions/1094841/reusable-library-to-get-human-readable-version-of-file-size
    """
    pow,n=min(int(log(max(n*b**pow,1),b)),len(pre)-1),n*b**pow
    return "%%.%if %%s%%s"%abs(pow%(-pow-1))%(n/b**float(pow),pre[pow],u)

def insert_data(x):
    start = time.time()
    print('Inserting {} chars ({})'.format(x, pretty_size(x)))
    data = 'a' * x
    stmt = 'INSERT INTO supersize(data) VALUES(%s)'
    cur.execute(stmt, (data,))
    end = time.time()
    runtime = end - start
    c.rollback()
    rollbacktime = time.time() - end
    runtime_per_mb = runtime / (x / 1024 / 1024)
    print('Runtime: {} (per MiB: {}), Rollback time: {}'.format(
           round(runtime,2), round(runtime_per_mb,2), round(rollbacktime,2)))

if __name__ == "__main__":
    dbconfig = {
        'host': '::1',
        'port': 5610,
        'user': 'msandbox',
        'password': 'msandbox',
        'database': 'test'
    }
    if mysql.connector.__version_info__ > (2, 1) and mysql.connector.HAVE_CEXT:
        print('Using CAPI')
        dbconfig['use_pure'] = False
    c = mysql.connector.connect(**dbconfig)
    try:
        cur = c.cursor(prepared=True)
    except NotImplementedError:
        cur = c.cursor()
    cur.execute('DROP TABLE IF EXISTS supersize')
    cur.execute('''CREATE TABLE supersize(
id int unsigned auto_increment primary key,
data longblob) DEFAULT CHARSET latin1 ENGINE=BLACKHOLE''')

    if len(sys.argv) == 2:
        slength = int(sys.argv[1])
        insert_data(slength)
    else:
        # Insert up to 10MiB with steps of 100KiB
        for x in range(1, 10*1024*1024, 100*1024):
            insert_data(x)

        # Insert up to 10GiB with steps of 100MiB
        for x in range(100*1024*1024, 10*1024*1024*1024, 100*1024*1024):
            insert_data(x)

    cur.close()
    c.close()
