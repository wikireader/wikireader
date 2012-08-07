import urllib2, sys
import wx
import threading
import disk

DownloadBaseUrl = "http://wrmlbeta.s3.amazonaws.com"
PackageUrl = "/wikireader.json"

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
    name = None
    saveToFile = False
    totalSize = 0
    existSize = 0
    
    #----------------------------------------------------------------------
    def __init__(self, name, callback, updateback=None, saveToFile=False, totalSize=None, existSize=0):
        """Init Worker Thread Class."""
        self.name = name
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
        try:
            resumeSize = disk.resumeFileSize(self.name)
            
            urlopener = urllib2.build_opener()
            
            if self.saveToFile:
                if resumeSize > 0:
                    f = open(disk.getDownloadTmpDir() + self.name, 'ab')
                    urlopener.addheaders = [('Range', 'bytes=%s-' % (resumeSize))]
                else:
                    f = open(disk.getDownloadTmpDir() + self.name, 'wb')
            
            response = urlopener.open(DownloadBaseUrl + self.name)
            
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
            print 'URLError: ' + DownloadBaseUrl + self.name
            if self.callback:
                wx.CallAfter(self.callback, None)
        
        if self.saveToFile:
            f.close()

def downloadPackageList(callback):
    """ download package list file """
    return DownloadThread(PackageUrl, callback)
    
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
            QueueFiles.append(packageFile['name'])
        
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
        name = self.QueueFiles[self.index]
        if name[0] != '/':
            name = '/' + name
        
        if len(self.QueueFiles) == self.index + 1:
            self.parent.thread = DownloadThread(name, self.callback, self.updateback, True, self.totalSize, self.existsSize)
        else:
            self.parent.thread = DownloadThread(name, self.startNext, self.updateback, True, self.totalSize, self.existsSize)
    
    def startNext(self):
        self.existsSize = self.existsSize + self.package['files'][self.index]['size']
        self.index = self.index + 1
        self.start()
    
s1 = '{"id":"cy","english":"Welsh","native":"Cymraeg","name":"Wikipedia","date":"20120503","folder":"cypedia","size":29521608,"files":[{"name":"cypedia-20120503.7z.001","size":28179498,"md5":"4065024fc1b77e1e4c889911a32e07f7"}]}'
s2 = '{"id":"de","english":"German","native":"Deutsch","name":"Wikipedia","date":"20120508","folder":"depedia","size":2256618548,"files":[{"name":"depedia-20120508.7z.001","size":1073741824,"md5":"672f763bc17b7a7d5b46854bf7746680"},{"name":"depedia-20120508.7z.002","size":1073741824,"md5":"fbe7c66833d6f91e0db57dfe870655d9"},{"name":"depedia-20120508.7z.003","size":90846486,"md5":"8e9250c5fa13120eec2d5432986aab4d"}]}'

#downloadPackage(simplejson.loads(s1))