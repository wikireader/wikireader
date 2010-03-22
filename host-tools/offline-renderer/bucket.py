#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2010
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Article Rendering
# AUTHORS: Christopher Hall <hsw@openmoko.com>

import sys
import os


class Bucket(object):
    """maintain a set of containers"""


    def __init__(self, max_buckets = 10, bucket_size = 10240, max_items_per_bucket = 30):
        """set up the instance"""
        self.max_buckets = max_buckets
        self.bucket_size = bucket_size
        self.max_items_per_bucket = max_items_per_bucket
        self.buckets = []
        for i in range(max_buckets):
            self.buckets += [[]]
        self.bucket_remaining = [bucket_size] * max_buckets
        self.bucket_counts = [0] * max_buckets


    def write(self, data):
        """user supplied callback to output the list: [(size, data),...]"""
        raise AttributeError('Virtual function Bucket.write called')


    def find_bucket(self, size):
        """find a bucket with some space in it"""

        # bucket with maximum items
        for i in range(self.max_buckets):
            if self.bucket_counts[i] >= self.max_items_per_bucket:
                self.empty(i)
                return i

        # bucket with sufficient free space
        for i in range(self.max_buckets):
            if self.bucket_remaining[i] >= size:
                return i

        # empty the fullest bucket, but try to top it up first
        bucket = 0
        size = self.bucket_size
        for i in range(self.max_buckets):
            if self.bucket_remaining[i] < size:
                size = self.bucket_remaining[i]
                bucket = i

        #print('fullest bucket: {0:d}'.format(i))
        self.top_up(bucket)
        self.empty(bucket)
        return bucket


    def top_up(self, bucket):
        """try to top a bucket from others"""
        for i in range(self.max_buckets):
            if self.bucket_counts[bucket] >= self.max_items_per_bucket:
                break
            if i != bucket:
                #print('bucket: {0:d}'.format(i))
                for j in range(len(self.buckets[i]) - 1, -1, -1):
                    item = self.buckets[i][j]
                    #print('bucket: {0:d} {1:d} = {2!r:s}'.format(i, j, item))
                    size = item[0]
                    #print('bucket: {0:d} s = {1:d}  remaining = {2:d}'.format(i, size,  self.bucket_remaining[bucket]))
                    if [] != size and size <= self.bucket_remaining[bucket]:
                        self.buckets[bucket] += [(size, item[1])]
                        self.bucket_remaining[bucket] -= size
                        del self.buckets[i][j]
                        self.bucket_counts[i] -= 1
                        self.bucket_remaining[i] += size
                        if self.bucket_counts[bucket] >= self.max_items_per_bucket:
                            break

        self.empty(bucket)
        return bucket


    def empty(self, bucket):
        """empty a specific bucket"""
        if self.bucket_remaining[bucket] != self.bucket_size:
            self.write(self.buckets[bucket])
            self.bucket_remaining[bucket] = self.bucket_size
            self.bucket_counts[bucket] = 0
            self.buckets[bucket] = []


    def flush(self):
        """empty all buckets"""
        for bucket in range(self.max_buckets):
            self.top_up(bucket)
            self.empty(bucket)


    def add(self, data, size):
        """add a data item to the first bucket with space"""
        if size > self.bucket_size:
            self.write([(size, data)])
        else:
            bucket = self.find_bucket(size)
            #print('bucket {0!d}'.format(bucket))
            self.buckets[bucket] += [(size, data)]
            self.bucket_remaining[bucket] -= size
            self.bucket_counts[bucket] += 1


def main():
    """simple test"""
    class MyBucket(Bucket):
        def __init__(self, *args, **kw):
            super(MyBucket, self).__init__(*args, **kw)
        def write(self, data):
            print('write: {0!r:s}'.format(data))


    b = MyBucket(max_buckets = 5, bucket_size = 17, max_items_per_bucket = 10)

    b.add('big_item', 1000)
    b.add('A_init', 1)
    b.add('A_one', 3)
    b.add('A_two', 3)
    b.add('A_three', 5)
    b.add('A_four', 4)
    b.add('A_five', 4)
    b.add('A_six', 3)
    b.add('A_seven', 5)
    b.add('A_eight', 5)
    b.add('A_nine', 4)
    b.add('A_ten', 3)
    b.add('A_eleven', 6)
    b.add('A_twelve', 6)
    b.add('A_thirteen', 8)
    b.add('B_init', 1)
    b.add('B_one', 3)
    b.add('B_two', 3)
    b.add('B_three', 5)
    b.add('B_four', 4)
    b.add('B_five', 4)
    b.add('B_six', 3)
    b.add('B_seven', 5)
    b.add('B_eight', 5)
    b.add('B_nine', 4)
    b.add('B_ten', 3)
    b.add('B_eleven', 6)
    b.add('B_twelve', 6)
    b.add('B_thirteen', 8)
    b.add('B_end1', 2)
    b.add('B_end2', 2)
    b.add('B_end3', 2)

    b.flush()


# run the program
if __name__ == "__main__":
    main()

