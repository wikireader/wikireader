#!/usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Openmoko Inc. 2009
# LICENSE: GPL Version 3 or later
# DESCRIPTION: Simple FLASH programmer
# AUTHOR: Christopher Hall <hsw@openmoko.com>

import sys
import time

import Dump


def msg(s):
    sys.stdout.write(s)
    sys.stdout.flush()

spin_count = 0

def spin_reset():
    global spin_count
    spin_count = 0


def spin():
    global spin_count
    if 1 == spin_count:
        msg('\x08-')
        spin_count = 2
    elif 2 == spin_count:
        msg('\x08\\')
        spin_count = 3
    elif 3 == spin_count:
        msg('\x08|')
        spin_count = 0
    else:
        msg('/')
        spin_count = 1


class FLASHROM:

    def __init__(self, name = 'FLASH', program_block_size = 128, verify_block_size = 128):
        self.name = name
        self.program_block_size = program_block_size
        self. verify_block_size =  verify_block_size
        self.erased = set()


    def program(self, buffer, offset):
        length = len(buffer)
        pre_bytes = (self.program_block_size - offset) % self.program_block_size
        block_bytes = length - pre_bytes
        post_bytes = block_bytes % self.program_block_size
        blocks = block_bytes / self.program_block_size

        #msg('pre = %d bytes\nblocks = %d * %d bytes\npost = %d bytes\n' % (pre_bytes, blocks, self.program_block_size, post_bytes))
        self.set_block_protection()

        # erase all 4k blocks we will need
        first_block = offset >> 12
        last_block = (offset + length) >> 12

        for block in range(first_block, last_block + 1):
            self.write_enable(True)
            self.erase_block(block)

        msg('%s: write %d bytes @0x%x ' % (self.name, length, offset))
        spin_reset()

        index = 0
        rc = self.write_block(buffer[0:pre_bytes], offset)
        if rc:
            spin()
        else:
            spin_reset()
            msg('F')

        index += pre_bytes
        offset += pre_bytes

        if rc:
            for b in range(0, blocks):
                rc = self.write_block(buffer[index:index + self.program_block_size], offset)
                if rc:
                    spin()
                else:
                    spin_reset()
                    msg('F')
                    break
                index += self.program_block_size
                offset += self.program_block_size

            if rc:
                rc = self.write_block(buffer[index:], offset)
                if rc:
                    spin()
                else:
                    spin_reset()
                    msg('F')

        self.wait_ready()
        self.write_enable(False)

        msg('\n')
        return rc


    def verify(self, buffer, offset):
        length = len(buffer)
        msg('%s: verifying %d bytes @0x%x ' % (self.name, length, offset))

        verify_buffer = ''

        a = 0
        spin_reset()
        while a < length:
            if (length - a) < self.verify_block_size:
                read_length = length - a
            else:
                read_length = self.verify_block_size

            verify_buffer += self.read_block(a + offset, read_length)

            spin()
            if buffer[a:a + read_length] != verify_buffer[a:a + read_length]:
                spin_reset()
                msg('\n')
                msg('%s: verify failed @0x%x, length = %d\n' % (self.name, a, read_length))
                msg('Read[expected]:')
                Dump.hexcmp(verify_buffer[a:a + read_length], buffer[a:a + read_length])
                msg('\n')
                return False
            a += read_length
        msg('\n')

        return True

    # Child clas must override some of these
    # at least read/write/erase block functions must be given

    def write_enable(enable):
        pass

    def wait_ready(s):
        pass

    def erase_block(s, block):
        assert True, 'Virtual %s called' % __name__

    def set_block_protection():
        pass

    def write_block(self, buffer, offset):
        assert True, 'Virtual %s called' % __name__

    def read_block(self, address, read_length):
        assert True, 'Virtual %s called' % __name__



class PM25LV512(FLASHROM):

    def __init__(self, spi, name = 'PM25LV512', program_block_size = 256, verify_block_size = 256):
        FLASHROM.__init__(self, name, program_block_size, verify_block_size)
        self.spi = spi


    def write_enable(self, enable):
        if enable:
            e = 0x06
        else:
            e = 0x04
        self.spi.command(e)


    def wait_ready(self):
        status = 0x01
        while 0x01 == (status & 0x01):
            status = ord(self.spi.transact(0x05, 1, 1)[0])


    def erase_block(self, block):
        a = block << 12
        block &= (1 << 12) - 1
        if block in self.erased:
            return
        msg('erase 4k sector @0x%08x\n' % a)
        self.write_enable(True)
        self.wait_ready()
        self.spi.command([0xd7, (a >> 16), (a >> 8), (a & 0xff)])
        self.wait_ready()
        self.erased.add(block)


    def set_block_protection(self):
        self.spi.command([0xf1, 0x00])
        self.spi.command([0x01, 0x02])


    def write_block(self, buffer, address):
        if 0 == len(buffer):
            return True

        self.wait_ready()
        self.write_enable(True)
        self.spi.command([0x02, (address >> 16), (address >> 8), address, buffer])
        return True


    def read_block(self, address, read_length):
        return self.spi.transact([0x03, (address >> 16), (address >> 8), address], 1, read_length)


