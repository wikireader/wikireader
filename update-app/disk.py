import os
import wx
import re
import hashlib
import threading
from commands import getoutput
from datetime import datetime
from shutil import copy

if wx.Platform == '__WXMSW__':
    import winapi
    import wmi
    import subprocess
    from Utilities import resource_path
      
#from commands import getoutput
#print getoutput('df -h')
tmpDirName = '/wikireader'
downloadDirName = '/Download'

def getSDCardFullPath():
    if wx.Platform == '__WXMAC__':
        return getSDCardRootPath() + '/' + getMountList()[0]
    else:
        return getMountList()[0]

def getSDCardRootPath():
    if wx.Platform == '__WXMAC__':
        return '/Volumes'
    else:
        return ""

def getProbablyMount():
    mounts = re.findall(r'/dev/disk(?!0).s.\s+([a-zA-Z0-9.]+)Gi.+/Volumes/(.+)', getoutput('df -h'))
    results = []
    for size, mount in mounts:
        if float(size) < 62:
            results.append(mount)
    
    return results

def getMountList():
    if wx.Platform == '__WXMAC__':
        scanPath = getSDCardRootPath()
        mount = getProbablyMount()

        tmpList = mount[:]

        for m in tmpList:
            checkPath = scanPath + '/' + m
            if not os.path.ismount(checkPath):
                mount.remove(m)
            elif not os.access(checkPath, os.W_OK):
                mount.remove(m)
        
        return mount
    else:
        return winapi.getRemovableDisk()
        #return []

def getVersion():
    for mount in getMountList():
        try:
            if wx.Platform == '__WXMSW__':
                f = open(mount + '/version.txt', 'r')
            else:
                f = open(getSDCardRootPath() + '/' + mount + '/version.txt', 'r')
                
            try:
                # Read the entire contents of a file at once.
                string = f.read()
                result = re.match("^VERSION: (.+)$", string)
                if result:
                    try:
                        d = datetime.strptime(result.group(1), '%Y%m%d')
                    except ValueError:
                        return None
                    
                    return "%d.%02d.%d" % (d.year, d.month, d.day)
            finally:
                f.close()
        except IOError:
            pass
    return None

def getFreeSpace(path):
    if wx.Platform == "__WXMSW__":
        t, f = winapi.getDiskSize(path)
        return int(t)/1024.0, int(f)/1024.0
    else:
        s = os.statvfs(getSDCardRootPath() + '/' + path)
        return s.f_blocks * s.f_frsize / 1024.0, s.f_bavail * s.f_frsize / 1024.0

def enoughSpace(kPackages, checkedItems):
    mounts = getMountList()
    
    InstallSize = 0
    for item in checkedItems:
        InstallSize = InstallSize + kPackages['volumes'][item]['size']
    
    if len(mounts) > 0:
        Total, Free = getFreeSpace(mounts[0])
    
    if InstallSize / 1024.0 > Free:
        return False
    
    return True

def humanable(Size):
    if (Size > 1024 * 1024 * 1024):
        return '%.1f GB' % (Size / (1024.0 * 1024 * 1024))
    elif (Size > 1024 * 1024):
        return '%.1f MB' % (Size / (1024.0 * 1024))
    else:
        return 1
    
def getHomePath():

    if wx.Platform == "__WXMSW__":
        return "C:\\"
    else:
        return os.path.expanduser("~")

    # try:
    #     #from win32com.shell import shellcon, shell            
    #     #return shell.SHGetFolderPath(0, shellcon.CSIDL_APPDATA, 0, 0)
    # except ImportError: # quick semi-nasty fallback for non-windows/win32com case
    #     return os.path.expanduser("~")
        
        
        
def getTmpDir(DownloadDir = ''):
    """ make sure folder exists and writeable """
    tmpDir = getHomePath() + tmpDirName + DownloadDir
    if os.path.exists(tmpDir):
        if not os.access(tmpDir, os.W_OK):
            os.chmod(tmpDir, 0755)
    else:
        os.mkdir(tmpDir)
        
    return tmpDir

def getDownloadTmpDir():
    return getTmpDir(downloadDirName)
    
def resumeFileSize(filename):
    checkfile = getDownloadTmpDir() + filename
    
    if os.path.exists(checkfile):
        return os.stat(checkfile).st_size
        
    return 0

def getFolderSize(start_path):
    total_size = 0
    for dirpath, dirnames, filenames in os.walk(start_path):
        for f in filenames:
            fp = os.path.join(dirpath, f)
            total_size += os.path.getsize(fp)
    return total_size

def checkFileSize(filename, size):
    checkfile = getDownloadTmpDir() + '/' + filename
    
    if os.path.exists(checkfile):
        return os.stat(checkfile).st_size == size
    
    return None

def checkMD5sum(filename, md5):
    checkfile = getDownloadTmpDir() + '/' + filename
    
    if os.path.exists(checkfile):
        f = open(checkfile, 'r')
        return md5_for_file(f) == md5
    
    return None
    
def md5_for_file(f, block_size=2**20):
    md5 = hashlib.md5()
    while True:
        data = f.read(block_size)
        if not data:
            break
        md5.update(data)
        
    f.close()
    return md5.hexdigest()

def installPackage(packages, checkItems, callback=None, updateback=None):
    InstallThread(packages, checkItems, callback, updateback)

class InstallThread(threading.Thread):
    need_abort = False
    callback = None
    packages = None
    checkItems = None
    
    #----------------------------------------------------------------------
    def __init__(self, packages, checkItems, callback, updateback):
        """Init Worker Thread Class."""
        self.packages = packages
        self.checkItems = checkItems
        self.callback = callback
        self.updateback = updateback
        self.fullPath = getSDCardFullPath()
        threading.Thread.__init__(self)
        self.start()
        
    def stop(self):
        self.need_abort = True
 
    #----------------------------------------------------------------------
    def run(self):
        # local test
        
        if wx.Platform == '__WXMSW__':
            try:
                copy(resource_path('') + "7za.exe", getTmpDir())
            except:
                pass
        else:
            try:
                copy("./tools/7za", getTmpDir())
                os.chmod(getTmpDir() + '/7za', 0755)
            except:
                pass
        
            try:
                copy("../Resources/7za", getTmpDir())
                os.chmod(getTmpDir() + '/7za', 0755)
            except:
                pass
        
        installQueue = []
        installQueue.append(self.packages['bases'][0]['files'][0]['name'])
        
        volumes = self.packages['volumes']
        for item in self.checkItems:
            currentExistsSize = getFolderSize(self.fullPath + '/' + volumes[item]['folder'])
            if currentExistsSize < volumes[item]['size']:
                for packageFile in volumes[item]['files']:
                    installQueue.append(packageFile['name'])
        
        i = 0
        for filename in installQueue:
            extractFile = getDownloadTmpDir() + '/' + filename
            extractPath = self.fullPath
            
            #if not re.match(r"^base", filename):
            #    extractPath = extractPath + '/' + filename.split('.')[0].split('-')[0]
            #    
            #    if not os.path.exists(extractPath):
            #        os.mkdir(extractPath)
            
            # ./7za x ./Download/base-20120620.7z -o/Volumes/UNTITLED/ -y
            if wx.Platform == "__WXMSW__":
                startupinfo = subprocess.STARTUPINFO()
                startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
                subprocess.call("%s/7za x %s -o\"%s\" -y" % (getTmpDir(), extractFile, extractPath), startupinfo=startupinfo)
                #os.system("%s/7za x %s -o\"%s\" -y" % (getTmpDir(), extractFile, extractPath))
            else:
                getoutput("%s/7za x %s -o\"%s\" -y" % (getTmpDir(), extractFile, extractPath))
            
            i = i + 1
            
            if self.updateback:
                wx.CallAfter(self.updateback, int(i / float(len(installQueue)) * 100))
        
        if self.callback:
            wx.CallAfter(self.callback)
        
#installPackage("base-20120620.7z", "/Volumes/UNTITLED")
#installPackage("cypedia-20120503.7z.001", "/Volumes/UNTITLED")

#print getMountList()
#print getVersion()
#print getMountList()
#a, b = getFreeSpace("UNTITLED")
#print a 
#print humanable(a)
#print humanable(b)
#print getTmpDir()
#print getDownloadTmpDir()
#print checkMD5sum("base-20120620.7z", "25cd13ec2476ba14a6901010aaba9545")
