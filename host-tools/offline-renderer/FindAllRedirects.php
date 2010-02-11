<?php
// COPYRIGHT:   Openmoko Inc. 2010
// LICENSE:     GPL Version 3 or later
// DESCRIPTION: Compute a list of all possible redirect words
//              by processing the MediaWiki language files
// AUTHORS:     Christopher Hall <hsw@openmoko.com>

// check that some files are given
if (sizeof($argv) < 2) {
  echo "usage: $argv[0] list_of_files\n";
  exit(1);
}

// remove the program name from the list of files
$list = array_slice($argv, 1);

// key of this array will be the redirect word
$redirects = array();

// Read each of the language files and extract: $magicWords['redirect']
foreach ($list as $file) {
    if ('file' === filetype($file)) {
        //echo "Processing: $file\n";
        $magicWords = array();
        $magicWords['redirect'] = array();
        include $file;
        if (is_array($magicWords)) {
            $m = $magicWords['redirect'];
            if (is_array($m)) {
                foreach ($m as $value) {
                    if ("0" !== $value && 0 !== $value) {
                        $value = mb_strtolower($value, 'UTF-8');
                        $redirects[$value] = $value;
                        //echo $value, "\n";
                    }
                }
            } elseif (NULL !== $m) {
                echo '$magicWords[\'redirect\'] is not an array for: ', $file, "\n";
                echo '$magicWords[\'redirect\'] is: ', gettype($m), "\n";
            }
        } else {
            echo '$magicWords is not an array for: ', $file, "\n";
        }
    }
}

// output the values
sort($redirects);
?>
#! /usr/bin/env python
# -*- coding: utf-8 -*-
#
# *** WARNING: Generated file do not modify
#
# Generated on: <?php echo date('Y-m-d H:i:s'), ' by ', $argv[0]; ?>


import os, sys
import re

redirect_string = r'('
<?php

$flag = false;
foreach ($redirects as $value) {
    echo 'redirect_string += \'';
    if ($flag) {
        echo '|';
    } else {
        $flag = true;
    }
    echo $value, "'\n";
}

?>
redirect_string += r')'

start_string = r'\s*'
end_string = r'[^\[]*\[\[(.*?)([#|].*?)?\]\]'

regex = re.compile(start_string + redirect_string + end_string, re.IGNORECASE)

def main():
    tests = [
        r'#redirect[[Just Testing]]',
        r'#айдау[[Just Testing]]',
        r'＃リダイレクト[[Just Testing]]',
        r'＃転送[[Just Testing]]',
        r'#転送[[Just Testing]]',
    ]

    for s in tests:
        m = regex.match(s)
        if m:
            print('matched: {0:s} : {1:s} => {3:s}'.format(s, m.group(1), m.group(2)))
        else:
            print('no match: {0:s}'.format(s))


# run the program
if __name__ == "__main__":
    main()
