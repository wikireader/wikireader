#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2012
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Create Article Indices
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>
#          Hank Wang <hank@openmoko.com>

import urllib2, sys
import wx
import threading
import disk

PackageListUrl = "http://en.thewikireader.com/downloads/wikireader.json"
PackageListName = '/wikireader.json'

def chunk_report(bytes_so_far, chunk_size, total_size, updateback=None):
    percent = float(bytes_so_far) / total_size
    percent = round(percent*100, 2)

    if updateback:
        wx.CallAfter(updateback, ("%0.1f%%" % percent))
    else:
        sys.stdout.write("Downloaded %d of %d bytes (%0.2f%%)\r" % (bytes_so_far, total_size, percent))
        if bytes_so_far >= total_size:
            sys.stdout.write('\n')

class DownloadThread(threading.Thread):
    need_abort = False
    callback = None
    updateback = None
    data = None
    saveToFile = False
    totalSize = 0
    existSize = 0

    #----------------------------------------------------------------------
    def __init__(self, data, callback, updateback=None, saveToFile=False, totalSize=None, existSize=0):
        """Init Worker Thread Class."""
        self.data = data
        self.callback = callback
        self.updateback = updateback
        self.saveToFile = saveToFile
        self.totalSize = totalSize
        self.existSize = existSize
        threading.Thread.__init__(self)
        self.start()

    def stop(self):
        self.need_abort = True

    #----------------------------------------------------------------------
    def run(self):
        """Run Worker Thread."""

        name = self.data['name']
        url = self.data['url']

        if name[0] != '/':
            name = '/' + name

        try:
            resumeSize = disk.resumeFileSize(name)

            urlopener = urllib2.build_opener()

            if self.saveToFile:
                if resumeSize > 0:
                    f = open(disk.getDownloadTmpDir() + name, 'ab')
                    urlopener.addheaders = [('<Ra></Ra>nge', 'bytes=%s-' % (resumeSize))]
                else:
                    f = open(disk.getDownloadTmpDir() + name, 'wb')

            response = urlopener.open(url)

            chunk_size = 8192
            if self.totalSize:
                total_size = self.totalSize
            else:
                total_size = response.info().getheader('Content-Length').strip()
            total_size = int(total_size)

            bytes_so_far = resumeSize
            if self.existSize > 0:
                bytes_so_far = self.existSize
            data = ''

            while 1:
                if self.need_abort:
                    break

                chunk = response.read(chunk_size)

                if not chunk:
                    break

                bytes_so_far += len(chunk)
                if self.saveToFile:
                    f.write(chunk)

                if not self.saveToFile:
                    data += chunk

                chunk_report(bytes_so_far, chunk_size, total_size, self.updateback)

            if self.callback:
                # download package
                if not self.need_abort:
                    if self.saveToFile:
                        wx.CallAfter(self.callback)
                    else: # download list
                        wx.CallAfter(self.callback, data)
        except urllib2.URLError:
            print 'URLError: ' + url
            if self.callback:
                wx.CallAfter(self.callback, None)

        if self.saveToFile:
            f.close()

def downloadPackageList(callback):
    """ download package list file """
    return DownloadThread({'url':PackageListUrl, 'name':PackageListName}, callback)

def downloadPackage(parent, package, callback=None, updateback=None):
    """ download package by package name """

    QueueFiles = []
    totalSize = 0
    existsSize = 0
    for packageFile in package['files']:
        if disk.checkFileSize(packageFile['name'], packageFile['size']):
        #if disk.checkMD5sum(packageFile['name'], packageFile['md5']):
            existsSize = existsSize + packageFile['size']
        else:
            QueueFiles.append(packageFile)

        totalSize = totalSize + packageFile['size']

    if len(QueueFiles) > 0:
        queue = Queue(parent, package, QueueFiles, callback, updateback, totalSize, existsSize)
        queue.start()
    else:
        callback()


class Queue():
    def __init__(self, parent, package, QueueFiles, callback, updateback, totalSize, existsSize):
        self.index = 0
        self.parent = parent
        self.package = package
        self.QueueFiles = QueueFiles
        self.callback = callback
        self.updateback = updateback
        self.totalSize = totalSize
        self.existsSize = existsSize

    def start(self):
        qf = self.QueueFiles[self.index]

        if len(self.QueueFiles) == self.index + 1:
            self.parent.thread = DownloadThread(qf, self.callback, self.updateback, True, self.totalSize, self.existsSize)
        else:
            self.parent.thread = DownloadThread(qf, self.startNext, self.updateback, True, self.totalSize, self.existsSize)

    def startNext(self):
        self.existsSize = self.existsSize + self.package['files'][self.index]['size']
        self.index = self.index + 1
        self.start()
