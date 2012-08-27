#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2012
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Create Article Indices
# AUTHORS: Sean Moss-Pultz <sean@openmoko.com>
#          Christopher Hall <hsw@openmoko.com>
#          Hank Wang <hank@openmoko.com>

"""
This API only for MSW(Windows) version.
"""

import wmi

c = wmi.WMI()

def getRemovableDisk():
    tmp = []
    disk = []
    for drive in c.Win32_LogicalDisk(DriveType=2):
        tmp.append(str(drive.Caption))

    for physical_disk in c.Win32_DiskDrive():
        if physical_disk.size:
            for partition in physical_disk.associators ("Win32_DiskDriveToDiskPartition"):
                for logical_disk in partition.associators ("Win32_LogicalDiskToPartition"):
                    if logical_disk.Caption in tmp:
                        disk.append(str(logical_disk.Caption))

    return disk

def getDiskSize(DeviceID="G:"):
    disk = c.Win32_LogicalDisk(DeviceID=DeviceID)[0]
    return disk.size, disk.Freespace
