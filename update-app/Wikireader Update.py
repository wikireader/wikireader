#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2012
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Create Article Indices
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>
#          Hank Wang <hank@openmoko.com>

"""
WikiReader update app - Main program
"""

import sys, os
import wx
import wx.lib.mixins.listctrl as listmix
try:
    from agw import hyperlink as hl
except ImportError: # if it's not there locally, try the wxPython lib.
    import wx.lib.agw.hyperlink as hl

import simplejson
import disk
import download
import images

## Debug for deploy version
#import logging
# logger = logging.getLogger('WikiReader')
# hdlr = logging.FileHandler('/var/tmp/WikiReader.log')
# formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
# hdlr.setFormatter(formatter)
# logger.addHandler(hdlr)
# logger.setLevel(logging.INFO)
# logger.info('test')

# stuff for debugging
#print "Python", sys.version
#print "wx.version:", wx.version()
#print "pid:", os.getpid()
#print "uname:", os.uname()
#print "platform:", wx.Platform

kPackages = None
checkedItems = []

defaultSmallSize = 11
defaultFontSize = 14
defaultBigFontSize = 16
defaultMaxFontSize = 24
if wx.Platform == '__WXMSW__':
    defaultSmallSize = 10
    defaultFontSize = 12
    defaultBigFontSize = 14
    defaultMaxFontSize = 18

class AboutDialog(wx.Dialog):
    def __init__(self, parent, ID, title, size=wx.DefaultSize, pos=wx.DefaultPosition, 
                style=wx.DEFAULT_DIALOG_STYLE,
            ):
        pre = wx.PreDialog()
        pre.SetExtraStyle(wx.DIALOG_EX_CONTEXTHELP)
        pre.Create(parent, ID, title, pos, size, style)

        self.PostCreate(pre)
        wrap = 360
        linkPosX = 0
        copyRightPosX = 0
        if wx.Platform == '__WXMSW__':
            wrap = 260
            linkPosX = 30
            copyRightPosX = -30

        whitePanel = wx.Panel(self, -1, size=(499, 440), pos=(0, 0))
        whitePanel.SetBackgroundColour("White")

        logoImage = images.geticon_128x128Bitmap()
        logo = wx.StaticBitmap(whitePanel, -1, logoImage, size=(128, 128), pos=(136, 20))

        versionLabel = wx.StaticText(whitePanel, -1, "Version 1.0.3", pos=(156, 150))

        descLabel = wx.StaticText(whitePanel, -1, 
"From day one, our goal was to give physical form to Wikipedia, one of the most interesting aspects of the Internet, so people can bring it with them everywhere they go. We want to promote personal growth and the ability to enjoy those unexpected moments when curiosity strikes. WikiReader is our gift for those who have the openness to experience life and the willingness to be changed by it.",
        pos=(20, 180))
        descLabel.Wrap(wrap)
        descLabelFont = wx.Font(defaultFontSize, wx.DEFAULT, wx.NORMAL, wx.NORMAL, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        descLabel.SetFont(descLabelFont)

        hl.HyperLinkCtrl(whitePanel, wx.ID_ANY, "WikiReader uses only open source software",
                                        URL="https://github.com/wikireader", pos=(70+linkPosX, 340))

        copyrightLabel = wx.StaticText(whitePanel, -1, u"Copyright \N{Copyright Sign} 2012 Openmoko Inc. All rights reserved. The \nWikipedia name and logo are registered trademarks owned by \nthe Wikimedia Foundation and used with permission.", size=(499, 40), pos=(50+copyRightPosX, 370), style=wx.ALIGN_CENTER)
        copyrightLabelFont = wx.Font(defaultSmallSize, wx.DEFAULT, wx.NORMAL, wx.NORMAL, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        copyrightLabel.SetFont(copyrightLabelFont)


class WindowsMenuBar(wx.MenuBar):
    def OnAbout(self, evt):
        dlg = AboutDialog(self, -1, "About WikiReader Update", size=(400, 462), style=wx.DEFAULT_DIALOG_STYLE)
        dlg.ShowModal()

    def OnFAQ(self, evt):
        h = hl.HyperLinkCtrl(self.parent, wx.ID_ANY)
        h.GotoURL("http://thewikireader.com/#faq", True, True)

    def OnTwitter(self, evt):
        h = hl.HyperLinkCtrl(self.parent, wx.ID_ANY)
        h.GotoURL("http://twitter.com/wikireader", True, True)

    def __init__(self, parent):

        if wx.Platform == '__WXMSW__':

            wx.MenuBar.__init__(self)
            self.parent = parent
            fileMenu = wx.Menu()
            qitem = fileMenu.Append(201, "&Exit")

            helpMenu = wx.Menu()
            faqItem = helpMenu.Append(302, "&Frequently Asked Questions")
            titem = helpMenu.Append(303, "&Twitter")
            helpMenu.AppendSeparator()
            aitem =helpMenu.Append(301, "&About WikiReader Update")

            self.Append(fileMenu, '&File')
            self.Append(helpMenu, '&Help')

            parent.Bind(wx.EVT_MENU, self.OnAbout, aitem)
            parent.Bind(wx.EVT_MENU, parent.parent.OnClose, qitem)
            parent.Bind(wx.EVT_MENU, self.OnFAQ, faqItem)
            parent.Bind(wx.EVT_MENU, self.OnTwitter, titem)

            parent.SetMenuBar(self)


class MacSystemMenuFrame(wx.Frame):
    def OnAbout(self, evt):
        dlg = AboutDialog(self, -1, "About WikiReader Update", size=(400, 462), style=wx.DEFAULT_DIALOG_STYLE)
        dlg.ShowModal()


    def OnFAQ(self, evt):
        h = hl.HyperLinkCtrl(self, wx.ID_ANY)
        h.GotoURL("http://thewikireader.com/#faq", True, True)

    def OnTwitter(self, evt):
        h = hl.HyperLinkCtrl(self, wx.ID_ANY)
        h.GotoURL("http://twitter.com/wikireader", True, True)

    def __init__(self, parent):
        wx.Frame.__init__(self, None, -1, "WikiReader Update", size=(499, 360))
        self.parent = parent

        menubar = wx.MenuBar()
        if wx.Platform == '__WXMAC__':
            menu = wx.Menu()
            qitem = menu.Append(wx.ID_EXIT,"Quit","Quit Applications")
            aitem = menu.Append(wx.ID_ABOUT, "&About WikiReader Update")
            faqItem = menu.Append(555, "Frequently Asked Questions")
            titem = menu.Append(556, "Twitter")
            menubar.Append(menu, '&Help')

            self.Bind(wx.EVT_MENU, self.OnAbout, aitem)
            self.Bind(wx.EVT_MENU, parent.OnClose, qitem)
            self.Bind(wx.EVT_MENU, self.OnFAQ, faqItem)
            self.Bind(wx.EVT_MENU, self.OnTwitter, titem)
            self.SetMenuBar(menubar)
        else:
            self.Destroy()


class MainFrame(wx.Frame):
    def __init__(self, parent):

        frameHeight = 0
        if wx.Platform == '__WXMSW__':
           frameHeight = 30

        wx.Frame.__init__(self, None, -1, "WikiReader Update", size=(499, 360 + frameHeight))
        self.SetBackgroundColour("black")
        self.parent = parent
        self.Bind(wx.EVT_CLOSE, parent.OnClose)
        WindowsMenuBar(self)
        
        
        whitePanel = wx.Panel(self, -1, size=(499, 93), pos=(0, 245))
        whitePanel.SetBackgroundColour("White")
        whitePanel.Bind(wx.EVT_KEY_DOWN, parent.OnKeyDown)
        whitePanel.SetFocus()
        
        blueLinePanel = wx.Panel(self, -1, size=(499, 10), pos=(0, 236))
        blueLinePanel.SetBackgroundColour(wx.Colour(51, 153, 255))
        
        logoImage = images.getlogo_frontBitmap()
        wx.StaticBitmap(self, -1, logoImage, (39, 17), size=(430, 127))
        
        LastLabel = wx.StaticText(self, -1, "Last update on", size=(30, 40), pos=(49, 187))
        LastLabel.SetForegroundColour("#3399ff")
        LastLabelFont = wx.Font(defaultFontSize, wx.DEFAULT, wx.NORMAL, wx.BOLD, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        LastLabel.SetFont(LastLabelFont)
        self.LastLabel = LastLabel
        
        LastUpdateLabel = wx.StaticText(self, -1, "0000.00.00", size=(50, 40), pos=(49, 203))
        LastUpdateLabel.SetForegroundColour("white")
        LastUpdateLabelFont = wx.Font(defaultMaxFontSize, wx.DEFAULT, wx.NORMAL, wx.BOLD, underline=False, faceName="Arial, Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        LastUpdateLabel.SetFont(LastUpdateLabelFont)
        self.LastUpdateLabel = LastUpdateLabel
        lastVersion = checkingVersionDialog(self, False)

        if lastVersion == "0000.00.00":
            self.LastLabel.Hide()
            self.LastUpdateLabel.Hide()
        else:
            self.LastLabel.Show()
            self.LastUpdateLabel.Show()
            LastUpdateLabel.SetLabel(lastVersion)
        
        HintLabel = wx.StaticText(
                whitePanel, -1,
                "The Update tool downloads updates to your PC for easier\ntransfering to the WikiReader.",
                size=(50, 40), pos=(40, 5)
            )
        HintLabel.SetForegroundColour("black")
        HintLabelFont = wx.Font(defaultFontSize, wx.DEFAULT, wx.NORMAL, wx.BOLD, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        HintLabel.SetFont(HintLabelFont)
        
        updateButton = wx.Button(whitePanel, 40, "Check for updates", pos=(180, 50))
        updateButton.SetDefault()
        parent.Bind(wx.EVT_LEFT_DOWN, parent.OnUpdateClick, updateButton)
        
        self.Centre()
        self.Show(True)
        self.SetIcon(images.icon.GetIcon())

class CheckUpdateFrame(wx.Frame):
    def __init__(self, parent=None):

        pos = (140, 42)
        if wx.Platform == '__WXMSW__':
           pos = (110, 42)
        
        wx.Frame.__init__(self, None, -1, "WikiReader Update", size=(499, 307))
        self.SetBackgroundColour("black")
        self.parent = parent
        self.Bind(wx.EVT_CLOSE, parent.OnClose)
        
        whitePanel = wx.Panel(self, -1, size=(499, 252), pos=(0, 51))
        whitePanel.SetBackgroundColour("White")
        
        logoImage = images.getlogo_topBitmap()
        wx.StaticBitmap(self, -1, logoImage, pos=(8, 12), size=(336, 39))

        CheckUpdateLabel = wx.StaticText(whitePanel, -1, "Checking for new updates...", size=(50, 40), pos=pos)
        CheckUpdateLabelFont = wx.Font(defaultBigFontSize, wx.DEFAULT, wx.NORMAL, wx.BOLD, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        CheckUpdateLabel.SetFont(CheckUpdateLabelFont)

        cancelButton = wx.Button(whitePanel, 9, "Cancel", (211, 137))
        self.Bind(wx.EVT_BUTTON, parent.OnClose, cancelButton)
        
        gauge = wx.Gauge(self, -1, 50, pos=(125, 130), size=(250, 25))
        gauge.Pulse()
            
        self.Centre()
        self.SetIcon(images.icon.GetIcon())
        

class PackagesListCtrl(wx.ListCtrl, listmix.CheckListCtrlMixin, listmix.ListCtrlAutoWidthMixin):

    def __init__(self, parent, ID, pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.LC_REPORT|wx.LC_VIRTUAL|wx.LC_HRULES|wx.LC_VRULES):
        wx.ListCtrl.__init__(self, parent, ID, pos, size, style)
        listmix.CheckListCtrlMixin.__init__(self)
        listmix.ListCtrlAutoWidthMixin.__init__(self)
        self.parent = parent
        #self.setColumns()
    
    def OnCheckItem(self, index, flag):
        global checkedItems
        if flag:
            checkedItems.append(index)
        else:
            checkedItems.remove(index)

        if len(checkedItems) > 0:
            self.parent.parent.downloadButton.Enable()
            self.parent.parent.downloadButton.SetDefault()
        else:
            self.parent.parent.downloadButton.Disable()   

        self.parent.parent.updateSelectedSize()
    
    def setColumns(self):
        self.InsertColumn(0, 'name')
        self.InsertColumn(1, 'size', wx.LIST_FORMAT_LEFT)
        
        data = {}
        i = 0
        for volume in kPackages['volumes']:
            name = "%s / %s / %s" % (volume['native'].encode("utf-8"), volume['name'].encode("utf-8"), volume['date'].encode("utf-8"))
            size = volume['size']
            data[i] = (name, size)
            i = i + 1
        
        items = data.items()
        for key, values in items:
            index = self.InsertStringItem(sys.maxint, unicode(values[0], "utf-8"))
            if index % 2 == 0:
                self.SetItemBackgroundColour(index, "#F0F0FE")
                
            for i in range(len(values)):
                gap = ""
                if i == 0:
                    self.SetStringItem(index, i, gap + unicode(values[i], "utf-8"))
                else:
                    self.SetStringItem(index, i, disk.humanable(values[i]))
                
        self.SetColumnWidth(0, 330)
        self.SetColumnWidth(1, 70)


class SelectPackageFrame(wx.Frame):

    mswTotalSpace, mswFreeSpace = 0, 0

    def __init__(self, parent=None):

        frameHeight = 0
        posY = 0
        if wx.Platform == '__WXMSW__':
           frameHeight = 40
           posY = -10

        wx.Frame.__init__(self, None, -1, "WikiReader Update", size=(499, 507+frameHeight))
        self.SetBackgroundColour("black")
        self.parent = parent
        self.Bind(wx.EVT_CLOSE, parent.OnClose)
        WindowsMenuBar(self)
        
        whitePanel = wx.Panel(self, -1, size=(499, 451+frameHeight), pos=(0, 51))
        whitePanel.SetBackgroundColour("White")
        
        logoImage = images.getlogo_topBitmap()
        wx.StaticBitmap(self, -1, logoImage, pos=(8, 12), size=(336, 39))

        WaringLabel = wx.StaticText(whitePanel, -1, "Not Enough Disk Space!", size=(30, 40), pos=(36, 20+posY))
        WaringLabelFont = wx.Font(defaultBigFontSize, wx.DEFAULT, wx.NORMAL, wx.BOLD, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        WaringLabel.SetForegroundColour('red')
        WaringLabel.SetFont(WaringLabelFont)
        WaringLabel.Hide()
        self.WaringLabel = WaringLabel
        
        TopHintLabel = wx.StaticText(whitePanel, -1, "New updates are available for your WikiReader.", size=(30, 40), pos=(36, 16+posY))
        TopHintLabelFont = wx.Font(defaultBigFontSize, wx.DEFAULT, wx.NORMAL, wx.BOLD, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        TopHintLabel.SetFont(TopHintLabelFont)
        
        DescLabel = wx.StaticText(whitePanel, -1, "Downloading the updates may take some time.\nIf you're not ready to download now, press Cancel.\nTo continue, check the desired updates and press Download.", size=(30, 40), pos=(36, 36+posY))
        DescLabel.SetForegroundColour("#666666")
        DescLabelFont = wx.Font(defaultFontSize, wx.DEFAULT, wx.NORMAL, wx.NORMAL, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        DescLabel.SetFont(DescLabelFont)
        
        SelectedSizeLabel = wx.StaticText(whitePanel, -1, "No SDCard detected.", size=(30, 40), pos=(36, 90+posY))
        SelectedSizeLabel.SetForegroundColour("#666666")
        SelectedSizeLabelFont = wx.Font(defaultFontSize, wx.DEFAULT, wx.NORMAL, wx.NORMAL, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        SelectedSizeLabel.SetFont(SelectedSizeLabelFont)
        self.SelectedSizeLabel = SelectedSizeLabel
        
        cancelButton = wx.Button(whitePanel, 9, "Cancel", (150, 395+posY))
        self.Bind(wx.EVT_BUTTON, self.OnClick, cancelButton)

        downloadButton = wx.Button(whitePanel, 10, "Download", (260, 395+posY))
        downloadButton.Disable()
        self.downloadButton = downloadButton
        self.Bind(wx.EVT_BUTTON, self.OnClick, downloadButton)
        
        PackagesListPanel = wx.Panel(whitePanel, -1, size=(420, 270), pos=(36, 116+posY))
        PackagesListPanel.parent = self
        self.packagesListCtrl = PackagesListCtrl(PackagesListPanel, -1, size=(420, 270), style=wx.LC_REPORT | wx.LC_NO_HEADER)
        self.packagesListCtrl.setColumns()
        
            
        self.Centre()
        self.SetIcon(images.icon.GetIcon())
        self.updateSelectedSize()

    def OnClick(self, event):
        btnId = event.GetId() 
        if btnId == 9:
            self.parent.OnClose(self)
        elif btnId == 10:
            self.parent.OnDownload(self)
        else:
            event.Skip()
    
    def updateSelectedSize(self):

        # Set flag, don't checkFreeSpace everytime on Windows
        if self.mswTotalSpace != 0:
            return

        TotalSpace, FreeSpace = checkFreeSpace()

        if wx.Platform == '__WXMSW__':    
            self.mswTotalSpace = TotalSpace
            self.mswFreeSpace = FreeSpace

        if TotalSpace:
            SelectedSpace = 0
            for i in checkedItems:
                currentExistsSize = disk.getFolderSize(disk.getSDCardFullPath() + '/' + kPackages['volumes'][i]['folder'])
                
                if currentExistsSize <= kPackages['volumes'][i]['size']:
                    SelectedSpace = SelectedSpace + kPackages['volumes'][i]['size'] - currentExistsSize
                else:
                    SelectedSpace = SelectedSpace + kPackages['volumes'][i]['size']
                    
            SelectedSpace = SelectedSpace / 1024.0
            
            UsedSpace = TotalSpace - FreeSpace
            SelectedSizeText = "%.1fGB of %.1fGB selected." % ((UsedSpace+SelectedSpace)/1024.0/1024.0, TotalSpace/1024.0/1024.0)
            
            if (UsedSpace+SelectedSpace) > TotalSpace:
                self.WaringLabel.Show()
                self.downloadButton.Disable()
            else:
                self.WaringLabel.Hide()
                if len(checkedItems) > 0:
                    self.downloadButton.Enable()
        else:
            SelectedSizeText = "No SDCard detected."
            
        self.SelectedSizeLabel.SetLabel(SelectedSizeText)


class DownloadPackagesListCtrl(wx.ListCtrl, listmix.ListCtrlAutoWidthMixin):
    
    def __init__(self, parent, ID, pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.LC_REPORT|wx.LC_VIRTUAL|wx.LC_HRULES|wx.LC_VRULES):
        wx.ListCtrl.__init__(self, parent, ID, pos, size, style)
        #listmix.CheckListCtrlMixin.__init__(self)
        listmix.ListCtrlAutoWidthMixin.__init__(self)
        self.setColumns()
    
    def setColumns(self):
        self.InsertColumn(0, 'Name')
        self.InsertColumn(1, 'Progress', wx.LIST_FORMAT_CENTER)
        self.InsertColumn(2, 'Status', wx.LIST_FORMAT_CENTER)
        
        #data = {"1": ("Cymraeg / Wikipedia / 20120623", "255.4MB"),
        #"2": ("Dansk", "1.27GB"),
        #"3": ("Deutsch", "328MB"),
        #"4": ("English", "11MB")}
        
        data = {}
        i = 0
        for item in checkedItems:
            volume = kPackages['volumes'][item]
            name = "%s / %s / %s" % (volume['native'].encode("utf-8"), volume['name'].encode("utf-8"), volume['date'].encode("utf-8"))
            progress = "0%"
            time = "Wait"
            data[i] = (name, progress, time)
            i = i + 1
        
        items = data.items()
        for key, values in items:
            index = self.InsertStringItem(sys.maxint, unicode(values[0], "utf-8"))
            if index % 2 == 0:
                self.SetItemBackgroundColour(index, "#F0F0FE")
                
            for i in range(len(values)):
                if i==0:
                    gap = ' '
                self.SetStringItem(index, i, gap + unicode(values[i], "utf-8"))
                
        self.SetColumnWidth(0, 270)
        self.SetColumnWidth(1, 70)
        self.SetColumnWidth(2, 60)

class DownloadPackageFrame(wx.Frame):

    thread = None
    currentDownloadIndex = 0
    dlg = None
        
    def __init__(self, parent=None):
        
        frameHeight = 0
        posY = 0
        if wx.Platform == '__WXMSW__':
           frameHeight = 40
           posY = -60

        wx.Frame.__init__(self, None, -1, "WikiReader Update", size=(499, 507+frameHeight))
        
        self.parent = parent
        self.SetBackgroundColour("black")
        self.Bind(wx.EVT_CLOSE, parent.OnClose)
        WindowsMenuBar(self)
        
        whitePanel = wx.Panel(self, -1, size=(499, 451+frameHeight), pos=(0, 51))
        whitePanel.SetBackgroundColour("White")
        
        logoImage = images.getlogo_topBitmap()
        wx.StaticBitmap(self, -1, logoImage, pos=(8, 12), size=(336, 39))
        
        TopHintLabel = wx.StaticText(whitePanel, -1, "Downloading %d updates.. " % (len(checkedItems)), size=(30, 40), pos=(36, 66+posY))
        TopHintLabelFont = wx.Font(defaultBigFontSize, wx.DEFAULT, wx.NORMAL, wx.BOLD, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        TopHintLabel.SetFont(TopHintLabelFont)
        self.TopHintLabel = TopHintLabel
        
        DescLabel = wx.StaticText(whitePanel, -1, "This will take a while. Make sure your computer is connected\nto a power source and please do not disconnect the Internet\nconnection while downloads are in progress.", size=(30, 40), pos=(36, 86+posY))
        DescLabel.SetForegroundColour("#666666")
        DescLabelFont = wx.Font(defaultFontSize, wx.DEFAULT, wx.NORMAL, wx.NORMAL, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        DescLabel.SetFont(DescLabelFont)

        cancelButton = wx.Button(whitePanel, 9, "Cancel", (210, 450+posY))
        self.Bind(wx.EVT_BUTTON, parent.OnCancelDownload, cancelButton)
        
        PackagesListPanel = wx.Panel(whitePanel, -1, size=(420, 270), pos=(36, 166+posY))
        self.packagesListCtrl = DownloadPackagesListCtrl(PackagesListPanel, -1, size=(420, 270), style=wx.LC_REPORT)
        
            
        self.Centre()
        #self.Show(True)
        self.SetIcon(images.icon.GetIcon())
    
    def checkBaseFile(self):
        self.dlg = wx.ProgressDialog("WikiReader Updater",
                               "Checking base files ...",
                               maximum = 100,
                               parent=self,
                               style = 0
                                | wx.PD_APP_MODAL
                                #| wx.PD_CAN_ABORT
                                #| wx.PD_CAN_SKIP
                                #| wx.PD_ELAPSED_TIME
                                #| wx.PD_ESTIMATED_TIME
                                #| wx.PD_REMAINING_TIME
                                | wx.PD_AUTO_HIDE
                                )
        
        base = kPackages['bases'][0]
        baseFile = base['files'][0]
        if not disk.checkFileSize(baseFile['name'], baseFile['size']):
            self.dlg.Update(0, "Downloading base files ...")
            download.downloadPackage(self, base, callback=self.finishBaseFile, updateback=self.updateBaseFile)
        else:
            self.dlg.Update(100) # set 100 to close the dlg
            self.finishBaseFile()
    
    def updateBaseFile(self, percent):
        self.dlg.Update(int(float(percent[:-1])))
        
    def finishBaseFile(self):
        self.downloadStart()
        
    def downloadStart(self):
        download.downloadPackage(self, kPackages['volumes'][checkedItems[0]], callback=self.finishDownload, updateback=self.updateProgress)
    
    def finishDownload(self):
        if self:
            self.packagesListCtrl.SetStringItem(self.currentDownloadIndex, 1, "100%")
            self.packagesListCtrl.SetStringItem(self.currentDownloadIndex, 2, "Finished")
            self.currentDownloadIndex = self.currentDownloadIndex + 1
            
            if len(checkedItems) == self.currentDownloadIndex:
                self.parent.GoPrepareInstall()
            else:
                download.downloadPackage(self, kPackages['volumes'][checkedItems[self.currentDownloadIndex]], callback=self.finishDownload, updateback=self.updateProgress)
        
    def updateProgress(self, percent):
        if self:
            self.packagesListCtrl.SetStringItem(self.currentDownloadIndex, 1, percent)
            self.packagesListCtrl.SetStringItem(self.currentDownloadIndex, 2, "Download")
        return


class PrepareInstallFrame(wx.Frame):
    def __init__(self, parent=None):
        
        frameHeight = 0
        posY = 0
        posX = 0
        imagePosY = 0
        imagePosX = 0
        if wx.Platform == '__WXMSW__':
            frameHeight = 40
            posY = -0
            posX = -40
            imagePosY = -0
            imagePosX = -30    

        wx.Frame.__init__(self, None, -1, "WikiReader Update", size=(499, 320+frameHeight))
        self.SetBackgroundColour("black")
        self.parent = parent
        self.Bind(wx.EVT_CLOSE, parent.OnClose)
        WindowsMenuBar(self)
        
        whitePanel = wx.Panel(self, -1, size=(499, 252+frameHeight), pos=(0, 51))
        whitePanel.SetBackgroundColour("White")
        
        logoImage = images.getlogo_topBitmap()
        wx.StaticBitmap(self, -1, logoImage, pos=(8, 12), size=(336, 39))


        self.posX = posX
        self.posY = posY
        
        HintLabel = wx.StaticText(whitePanel, -1, "Prepare to install ...", size=(499, 40), pos=(68+posX, 20+posY), style=wx.ALIGN_CENTER)
        HintLabelFont = wx.Font(defaultFontSize, wx.DEFAULT, wx.NORMAL, wx.BOLD, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        HintLabel.SetFont(HintLabelFont)
        self.HintLabel = HintLabel
        
        deviceImage = images.getdeviceBitmap()
        wx.StaticBitmap(whitePanel, -1, deviceImage, pos=(75+imagePosX, 65+posY+imagePosY), size=(388, 112))
        
        quitButton = wx.Button(whitePanel, 8, "Quit", (150, 195+posY))
        self.Bind(wx.EVT_BUTTON, parent.OnClose, quitButton)

        self.installButton = wx.Button(whitePanel, 9, "Install", (270, 195+posY))
        self.Bind(wx.EVT_BUTTON, parent.OnInstall, self.installButton)
        
        tryButton = wx.Button(whitePanel, 10, "Try again", (270, 195+posY), (20, 20))
        self.Bind(wx.EVT_BUTTON, self.OnTryClick, tryButton)
        tryButton.SetDefault()
        tryButton.SetSize(tryButton.GetBestSize())
        self.tryButton = tryButton
        
        self.Centre()
        self.SetIcon(images.icon.GetIcon())
        
        self.checkSDcard()
    
    def OnTryClick(self, event):
        self.checkSDcard()
        
    def checkSDcard(self):
        mounts = disk.getMountList()
        self.HintLabel.SetForegroundColour("red")
        self.installButton.Show()
        self.tryButton.Hide()
            
        if len(mounts) == 0:
            self.HintLabel.SetLabel("Insert WikiReader's SD card into a memory card reader\nthat is plugged into this computer.")
            self.installButton.Hide()
            self.tryButton.Show()
            self.HintLabel.SetPosition((68+self.posX, 20+self.posY))
        elif not disk.enoughSpace(kPackages, checkedItems):
            self.HintLabel.SetLabel("This is microSD card doesn't have enough free space.")
            self.installButton.Hide()
            self.tryButton.Show()
            self.HintLabel.SetPosition((68+self.posX, 20+self.posY))
        # elif not disk.getVersion():
        #     self.HintLabel.SetLabel("This is not wikireader microSD card. continue installation?")
        #     self.HintLabel.SetPosition((48+self.posX, 20+self.posY))
        else:
            self.HintLabel.SetLabel("Click install to continue.")
            self.HintLabel.SetForegroundColour("black")
            self.HintLabel.SetPosition((188+self.posX, 20+self.posY))


class InstallFrame(wx.Frame):
    def __init__(self, parent=None):
        
        posY = 0
        posX = 0
        if wx.Platform == '__WXMSW__':
            posY = -20
            posX = -20

        wx.Frame.__init__(self, None, -1, "WikiReader Update", size=(499, 307))
        self.SetBackgroundColour("black")
        
        whitePanel = wx.Panel(self, -1, size=(499, 252), pos=(0, 51))
        whitePanel.SetBackgroundColour("White")
        self.Bind(wx.EVT_CLOSE, parent.OnClose)
        
        logoImage = images.getlogo_topBitmap()
        wx.StaticBitmap(self, -1, logoImage, pos=(8, 12), size=(336, 39))
        
        HintLabel = wx.StaticText(whitePanel, -1, "Installing updates to microSD card..", size=(499, 40), pos=(115+posX, 60+posY))
        HintLabelFont = wx.Font(defaultBigFontSize, wx.DEFAULT, wx.NORMAL, wx.BOLD, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        HintLabel.SetFont(HintLabelFont)
        
        SubHintLabel = wx.StaticText(whitePanel, -1, "We're almost there..", size=(499, 40), pos=(115+posX, 80+posY))
        SubHintLabelFont = wx.Font(defaultFontSize, wx.DEFAULT, wx.NORMAL, wx.NORMAL, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        SubHintLabel.SetForegroundColour('gray')
        SubHintLabel.SetFont(SubHintLabelFont)
        
        self.gauge = wx.Gauge(whitePanel, -1, 100, pos=(100, 140+posY), size=(300, 25))
        self.gauge.SetBezelFace(3)  
        self.gauge.SetShadowWidth(3)
        
            
        self.Centre()
        self.SetIcon(images.icon.GetIcon())
        
    def updateGauge(self, count):
        self.gauge.SetValue(count)
        

class FinishFrame(wx.Frame):
    def __init__(self, parent=None):
        
        frameHeight = 0
        posY = 0
        posX = 0
        if wx.Platform == '__WXMSW__':
            frameHeight = 70
            posY = 0
            posX = -20

        wx.Frame.__init__(self, None, -1, "WikiReader Update", size=(499, 307+frameHeight))
        self.SetBackgroundColour("black")
        self.parent = parent
        self.Bind(wx.EVT_CLOSE, parent.OnClose)
        WindowsMenuBar(self)
        
        whitePanel = wx.Panel(self, -1, size=(499, 252+frameHeight), pos=(0, 51))
        whitePanel.SetBackgroundColour("White")
        
        logoImage = images.getlogo_topBitmap()
        wx.StaticBitmap(self, -1, logoImage, pos=(8, 12), size=(336, 39))
        
        HintLabel = wx.StaticText(whitePanel, -1, "Installation complete!", size=(499, 40), pos=(155+posX, 50+posY))
        HintLabelFont = wx.Font(defaultBigFontSize, wx.DEFAULT, wx.NORMAL, wx.BOLD, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        HintLabel.SetFont(HintLabelFont)
        
        SubHintLabel = wx.StaticText(whitePanel, -1, "Thank you for being so patient. You may now safely eject\nthe microSD card and place it back in your WikiReader.", size=(10, 40), pos=(65+posX, 80+posY), style=wx.ALIGN_CENTER)
        SubHintLabelFont = wx.Font(defaultFontSize, wx.DEFAULT, wx.NORMAL, wx.NORMAL, underline=False, faceName="Helvetica", encoding=wx.FONTENCODING_DEFAULT)
        # SubHintLabel.SetForegroundColour('777777')
        SubHintLabel.SetFont(SubHintLabelFont)

        doneButton = wx.Button(whitePanel, 8, "Done", (200, 230))
        doneButton.SetDefault()
        self.Bind(wx.EVT_BUTTON, parent.OnClose, doneButton)
        
        self.Centre()
        self.SetIcon(images.icon.GetIcon())


class WikiReaderUpdater(wx.App):
    currentThread = None
    
    def __init__(self):
        wx.App.__init__(self, redirect=False)

    def OnInit(self):
        MacSystemMenuFrame(self).Hide()
        self.mainFrame = MainFrame(self)
        self.SetTopWindow(self.mainFrame)

        self.Bind(wx.EVT_KEY_DOWN, self.OnKeyDown)
        self.Bind(wx.EVT_BUTTON, self.OnClose)
        

        # init tmp dir
        disk.getTmpDir()
        
        # debug
        # self.mainFrame.Destroy()
        # self.OnUpdateClick(None)
        # SelectPackageFrame(self).show()
        # FinishFrame(self).Show()
        #self.goSelect()
        
        return True
    
    def OnClose(self, event):
        if self.currentThread:
            self.currentThread.stop()
        wx.Exit()
    
    def OnUpdateClick(self, event):
        self.mainFrame.Destroy()
        self.checkUpdateFrame = CheckUpdateFrame(self)
        self.checkUpdateFrame.Show()
        self.checkUpdate()
    
    def OnKeyDown(self, event):
        if event.GetKeyCode() == wx.WXK_ESCAPE:
            wx.Exit()
        event.Skip()
        
    def checkUpdate(self):
        self.currentThread = download.downloadPackageList(self.finishCheckUpdate)
    
    def finishCheckUpdate(self, packages):
        global kPackages
        self.currentThread = None
        kPackages = simplejson.loads(str(packages))
        self.goSelect()

    def goSelect(self):
        self.checkUpdateFrame.Destroy()
        self.selectPackageFrame = SelectPackageFrame(self)
        self.selectPackageFrame.Show()
        
    def OnDownload(self, event):
        if len(checkedItems) == 0:
            showMessage("Please select packages to download.")
        else:
            self.selectPackageFrame.Hide()
            self.downloadPackageFrame = DownloadPackageFrame(self)
            self.downloadPackageFrame.Show()
            self.downloadPackageFrame.checkBaseFile()
    
    def OnCancelDownload(self, event):
        if self.downloadPackageFrame.thread:
            self.downloadPackageFrame.thread.stop()
        self.downloadPackageFrame.Destroy()
        self.selectPackageFrame.Show()
        
    def GoPrepareInstall(self):
        self.downloadPackageFrame.Destroy()
        self.prepareInstallFrame = PrepareInstallFrame(self)
        self.prepareInstallFrame.Show()
        
    def OnInstall(self, event):
        self.prepareInstallFrame.Destroy()
        self.installFrame = InstallFrame(self)
        self.installFrame.Show()
        disk.installPackage(kPackages, checkedItems, self.FinishInstall, self.installFrame.updateGauge)
    
    def FinishInstall(self):
        self.installFrame.Destroy()
        self.finishFrame = FinishFrame(self)
        self.finishFrame.Show()
        
def checkFreeSpace():
    mountDisks = disk.getMountList()
    #if len(mountDisks) > 1:
    #    print "More than one disks"
    #elif len(mountDisks) > 0:
    #    return disk.getFreeSpace(mountDisks[0])

    if len(mountDisks) > 0:
        return disk.getFreeSpace(mountDisks[0])
    
    return None, None


def showMessage(msg):
    wx.MessageBox(msg, 'Info', wx.OK | wx.ICON_INFORMATION)

def checkingVersionDialog(parent, showError=True):
    dlg = wx.ProgressDialog("WikiReader Updater",
                               "Checking sdcard ...",
                               maximum = 100,
                               parent=parent,
                               style = 0
                                | wx.PD_APP_MODAL
                                | wx.PD_CAN_ABORT
                                #| wx.PD_CAN_SKIP
                                #| wx.PD_ELAPSED_TIME
                                #| wx.PD_ESTIMATED_TIME
                                #| wx.PD_REMAINING_TIME
                                #| wx.PD_AUTO_HIDE
                                )
        
    version = disk.getVersion()
    if not version:
        version = '0000.00.00'
    
    #print version
    if len(disk.getMountList()) < 1 and showError:
        wx.MessageBox('We don\'t detected any sdcard on your computer', 'Info', wx.OK | wx.ICON_INFORMATION)
    
    dlg.Destroy()
    
    return version


if __name__ == "__main__":
    app = WikiReaderUpdater()
    app.MainLoop()
