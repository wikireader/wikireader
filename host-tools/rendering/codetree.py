"""
 Helper to create a Code Tree and write it out to storage

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

import struct

class CodeTree:
    class Node:
        def __init__(self, value, index):
            self.index = index
            self.left = None
            self.right = None
            self.value = value

        def __str__(self):
            return "Node: <%d,%s>" % (self.index, self.value)

    def __init__(self):
        self.nodes = []
        self.root = self.Node(None, len(self.nodes))
        self.nodes.append(self.root)

    def addCodeWord(self, codeword, value):
        """Add the codeword to the tree"""

        node = self.root
        for char in codeword:
            if char == '0':
                if not node.left:
                    node.left = self.Node(None, len(self.nodes))
                    self.nodes.append(node.left)
            
                node = node.left
            elif char == '1':
                if not node.right:
                    node.right = self.Node(None, len(self.nodes))
                    self.nodes.append(node.right)

                node = node.right
            else:
                assert False
        node.value = value

    def writeTo(self, file):
        """Write things in IN-ORDER into an array...."""
        for node in self.nodes:
            l_index = -1
            r_index = -1

            if node.left:
                l_index = node.left.index
            if node.right:
                r_index = node.right.index
            print "<%s, L:%d, R:%d>" % (node.value, l_index, r_index)
