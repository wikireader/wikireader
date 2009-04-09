#!/usr/bin/env python
"""
 Decode the file generate by generate_code

 Copyright (C) 2009 Holger Hans Peter Freyther <zecke@openmoko.org>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

class HuffTree:
    class Node:
        # For our tree
        def __init__(self):
            self.left = None
            self.right = None
            self.value = None

    def __init__(self):
        self.root = self.Node()

    def read(self, file):
        """Read a serialized huffman tree"""
        pass


class HuffTreeMatcher:
    """Match a code word..."""
    def __init__(self, tree):
        self.tree = tree
        self.reset()

    def reset(self):
        self.node = self.tree.root

    def add_char(self, char):
        if char == '0':
            self.node = self.node.left
        elif char == '1':
            self.node = self.node.right
        else:
            assert False

        return self.node.value

class BitReader:
    """Read stuff bit by bit..."""
    def __init__(self, data):
        self.data = data
        self.index = 0
        self.bit = 9

    def pull_bit(self):
        return None


def create_huff_tree(file):
    """
    Read the header and
    """
