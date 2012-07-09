#! /usr/bin/env python
# -*- coding: utf-8 -*-
# COPYRIGHT: Tom Bachmann
# LICENSE: GPL Version 3 or later
# DESCRIPTION: ...
# AUTHORS: Tom Bachmann <e_mc_h2@web.de>

from __future__ import with_statement
import xml.dom.minidom
import sys
import getopt
import os
import re
import shutil
import struct
import subprocess
import codecs

verbose = False
coll_shift = 28
article_count = 0
all_articles = {}
search_articles = []
index_matrix = {}
authors = {}

# this _must_ be in ascending ASCII sequence
KEYPAD_KEYS = """ !#$%&'()*+,-.0123456789=?@abcdefghijklmnopqrstuvwxyz"""

def usage(message):
    if None != message:
        print 'error:', message
    print 'usage: %s <options> directory' % os.path.basename(__file__)
    print '       --help                  This message'
    print '       --verbose               Enable verbose output'
    print '       --article-index=file    Article index database output [articles.db]'
    print '       --prefix=name           Device file name portion for .fnd/.pfx [pedia]'
    print '       --workdir=dir           Work directory [/tmp]'
    print '       --coll-number=number    Collection number'
    print '       --xhtml=file            XHTML output [all_articles.html]'
    exit(1)

def get_item(dom, name):
    return dom.getElementsByTagName(name)[0].childNodes[0].data

def prepend(item, what):
    if item.childNodes.length == 0:
        item.appendChild(what)
    else:
        item.insertBefore(what, item.childNodes[0])

def split_at_id(dom, name):
    l1 = [dom]
    result = None

# traverse the tree breadth-first, looking for the node
    while (not result) and (l1 != []):
        l2 = []
        for element in l1:
            for node in element.childNodes:
                if node.nodeType == node.ELEMENT_NODE:
                    l2.append(node)
                    if node.attributes.getNamedItem('id') and node.getAttribute('id') == name:
                        result = node
            l1 = l2

    if not result:
        raise(NameError('id not found!!!')) # do properly

# now split the tree
    parent  = result.parentNode
    before  = None
    after   = result

    while True:
        b = parent.cloneNode(False)
        a = parent
        fnd = False
        #print a
        #print b
        #print

        rema = []
        for n in a.childNodes:
            if n == after:
                fnd = True
                if before: # None on first while loop entry
                    b.appendChild(before)
                continue
            if not fnd:
                rema.append(n)

        for n in rema:
            b.appendChild(n) # automatically removes from a

        parent = a.parentNode
        before = b
        after  = a

        if after == dom.documentElement:
            break

    impl = dom.implementation
    newdoc = impl.createDocument(None, "irrelevant", None)
    newdoc.childNodes[0] = before

    return (newdoc, dom)

def create_link(dom, name, whereto):
    res = dom.createElement('a')
    res.setAttribute('href', whereto)
    res.appendChild(dom.createTextNode(name))
    return res

def build_toc(outf, domlist, title, author, rtitle):
    outf.write('<html><title>' + title + '</title><body>\n'
               + '<h1>' + title + '</h1>\n'
               + '<h2> by ' + author + '</h2>\n')
    i = 0
    for (name, d) in domlist:
        outf.write('<h3><a href="' + rtitle(i) +'">' + name + '</a></h3>\n')
        i += 1
    outf.write('</body></html>')

def handle_ebook(num, out_file):
    global article_count, all_articles, search_articles

    bookdir = num
    content = os.path.join(bookdir, 'content.opf')

    dom = xml.dom.minidom.parse(content)
    metadata = dom.getElementsByTagName('metadata')[0]
    manifest = dom.getElementsByTagName('manifest')[0]
    spine    = dom.getElementsByTagName('spine')[0]

    '''
    print
    print num
    print '  Title:  %s' % get_item(metadata, 'dc:title')
    print '  Author: %s' % get_item(metadata, 'dc:creator')
    '''
    title  = get_item(metadata, 'dc:title')
    author = get_item(metadata, 'dc:creator')
    file_as = metadata.getElementsByTagName('dc:creator')[0].getAttribute('opf:file-as')

    # build table of files
    files = {}
    for f in manifest.getElementsByTagName('item'):
        files[f.getAttribute('id')] = f.getAttribute('href')

    '''
    # build linear reading order
        linear = []
        for r in spine.getElementsByTagName('itemref'):
            f = files[r.getAttribute('idref')]
            if r.getAttribute('linear') != 'no':
                linear.append(f)
    '''

    # find contents file
    tocfile = files[spine.getAttribute('toc')]
    toc     = xml.dom.minidom.parse(os.path.join(num, tocfile))

    # build navpoints list
    doms      = {}
    navpoints = []
    navrex = re.compile('^([^#]*)#?(.*)$')
    for np in toc.getElementsByTagName('navMap')[0].getElementsByTagName('navPoint'):
        label = np.getElementsByTagName('navLabel')[0]
        content = np.getElementsByTagName('content')[0]
        filestr = content.getAttribute('src')
        m = navrex.match(filestr)
        navpoints.append((int(np.getAttribute('playOrder')),
                          get_item(label, 'text'),
                          m.group(1), m.group(2)))
        if not m.group(1) in doms:
            doms[m.group(1)] = xml.dom.minidom.parse(os.path.join(num, m.group(1)))

    npmap = {}
    dommap = {}
    navpoints.sort()
    for np in navpoints:
        if np[3] == "": # direct reference to a file, not part in a file
            npmap[np[0]] = doms[np[2]]
            assert not doms[np[2]] in dommap
            dommap[doms[np[2]]] = np[0]
            continue

        # reference to part in file, need to split
        (before, after) = split_at_id(doms[np[2]], np[3])
        doms[np[2]] = after
        npmap[np[0]] = after
        if after in dommap:
            npmap[dommap[after]] = before
        dommap[after] = np[0]

    # TODO
    # rejoin ridiculously short articles
    # add untouched files
    # add per-author files
    # handle multi-level navmaps properly?

    def rtitle(n):
        return title + ' - ' + str(n)

    domlist = []
    for (label, name, p, q) in navpoints:
        domlist.append((name, npmap[label]))

    i = 0
    toclink = create_link(dom, 'toc', title)
    for (n, d) in domlist:
        # add titles
        html   = d.getElementsByTagName('html')[0]
        titlelist = html.getElementsByTagName('title')
        dtitle = None
        if titlelist.length != 0:
            dtitle = titlelist[0]
        else:
            dtitle = dom.createElement('title')
            text   = dom.createTextNode('none')
            dtitle.appendChild(text)
            prepend(html, dtitle)
        dtitle.childNodes[0].data = rtitle(i)

        # add navigation links
        body = html.getElementsByTagName('body')[0]
        prev = create_link(dom, 'prev', rtitle(i-1))
        next_ = create_link(dom, 'next', rtitle(i+1))
        prepend(body, dom.createElement('br'))
        if i+1 != len(domlist):
            prepend(body, next_)
            prepend(body, dom.createTextNode(' '))
        prepend(body, toclink.cloneNode(True))
        if i != 0:
            prepend(body, dom.createTextNode(' '))
            prepend(body, prev)
        body.appendChild(dom.createElement('br'))
        if i != 0:
            body.appendChild(prev.cloneNode(True))
            body.appendChild(dom.createTextNode(' '))
        body.appendChild(toclink.cloneNode(True))
        if i+1 != len(domlist):
            body.appendChild(dom.createTextNode(' '))
            body.appendChild(next_.cloneNode(True))

        d.writexml(out_file)

        article_count = article_count + 1
        all_articles[rtitle(i)] = (article_count, -1)
        i += 1

    article_count += 1
    all_articles[title] = (article_count, -1)
    search_articles.append(title)
    build_toc(out_file, domlist, title, author, rtitle)

    if file_as in authors:
        authors[file_as].append(title)
    else:
        authors[file_as]=[title]

def main():
    global verbose
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hvi:p:n:w:x:C:',
                                   ['help', 'verbose',
                                    'article-index=',
                                    'prefix=',
                                    'coll-number=',
                                    'workdir=',
                                    'xhtml='])
    except getopt.GetoptError, err:
        usage(err)

    art_name = "articles.db"
    coll_number = 0
    prefix   = 'pedia'
    workdir  = '/tmp'
    out_name = 'all_articles.html'

    for opt, arg in opts:
        if opt in ('-v', '--verbose'):
            verbose = True
        elif opt in ('-h', '--help'):
            usage(None)
        elif opt in ('-i', '--article-index'):
            art_name = arg
        elif opt in ('-n', '--coll-number'):
            coll_number = int(arg)
        elif opt in ('-p', '--prefix'):
            prefix = arg
        elif opt in ('-w', '--workdir'):
            workdir = arg
        elif opt in ('-x', '--xhtml'):
            out_name = arg
        else:
            usage('unhandled option: ' + opt)

    directory = args[0]
    directory = os.path.abspath(directory)
    art_name  = os.path.abspath(art_name)
    out_name  = os.path.abspath(out_name)

    fnd_name = os.path.abspath(prefix) + str(coll_number) + '.fnd'
    pfx_name = os.path.abspath(prefix) + str(coll_number) + '.pfx'

    os.chdir(workdir)

    if verbose:
        print 'workdir:   %s' % workdir
        print 'directory: %s' % directory

    # create list of all epub files
    epubs = []
    rex = re.compile('.*\.epub$')
    for f in os.listdir(directory):
        m = rex.match(f)
        if m:
            epubs.append((0, os.path.join(directory, f))) # 0 a placholder
        elif verbose:
            print 'ignoring %s' % f

    # unzip all epubs into the work directory
    rootrex = re.compile('^(.*)\/content\.opf')
    for i in range(len(epubs)):
        (n, f) = epubs[i]
        os.system('unzip -q -o ' + f)
        dom  = xml.dom.minidom.parse(os.path.join('META-INF', 'container.xml'))
        root = dom.getElementsByTagName('ns0:rootfile')[0].getAttribute('full-path')
        m    = rootrex.match(root)
        epubs[i] = (m.group(1), f)
    shutil.rmtree('META-INF')
    os.remove('mimetype')

    # remove leftover files
    art_import = art_name + '.import'
    for f in [art_name, art_import, out_name, pfx_name, fnd_name]:
        if os.path.exists(f):
            os.remove(f)

    # open output file
    out_file    = codecs.open(out_name, 'w', 'utf-8')

    # process all ebooks
    for (n, f) in epubs:
        handle_ebook(n, out_file)

    print authors

    # close output file
    out_file.close()

    output_fnd(fnd_name, coll_number)
    output_pfx(pfx_name)

    write_db(art_name)


def write_db(db_name):
    import_name = db_name + '.import'
    if os.path.exists(import_name):
         os.remove(import_name)

    with open(import_name, 'w') as f:
        for title in all_articles:
            (article_number, fnd_offset) = all_articles[title]
            f.write('~' + title.encode('utf-8'))    # force string
            f.write('\t%d\t%d\t%d\n' % (article_number, fnd_offset, 0))

    p = subprocess.Popen('sqlite3 > /dev/null 2>&1 ' + db_name, shell=True, stdin=subprocess.PIPE)
    p.stdin.write("""
create table articles (
    title varchar primary key,
    article_number integer,
    fnd_offset integer,
    restricted varchar
);

pragma synchronous = 0;
pragma temp_store = 2;
pragma locking_mode = exclusive;
pragma cache_size = 20000000;
pragma default_cache_size = 20000000;
pragma journal_mode = memory;

.mode tabs
.import %s articles
.exit
""" % import_name)
    p.stdin.close()
    p.wait()


def output_pfx(filename):
    """output the pfx matrix"""
    global index_matrix

    out_f = open(filename, 'w')
    list = '\0' + KEYPAD_KEYS
    for k1 in list:
        for k2 in list:
            for k3 in list:
                key = k1+k2+k3
                if key in index_matrix:
                    offset = index_matrix[key]
                else:
                    offset = 0
                out_f.write(struct.pack('I', offset))

    out_f.close()


import unicodedata
def strip_accents(s):
    if type(s) == str:
        s = unicode(s, 'utf-8')
    return ''.join((c for c in unicodedata.normalize('NFD', s) if unicodedata.category(c) != 'Mn'))


# fake bigram encoding
def bigram_encode(title):
    result = ''
    title = strip_accents(title)

    while len(title) >= 1:
        if title[0].lower() in KEYPAD_KEYS:
            result += chr(ord(title[0:1]))
        title = title[1:]

    return result


def output_fnd(filename, coll_number):
    """create bigram table"""
    global index_matrix
    global all_articles, search_articles

    out_f = open(filename, 'w')

    # create fake bigram table
    i = 0
    while i < 128:
        out_f.write('zz')
        i += 1

    # create pfx matrix and write encoded titles

    def sort_key(key):
        global KEYPAD_KEYS
        return ''.join(c for c in strip_accents(key).lower() if c in KEYPAD_KEYS)

    # first write those articles that should appear in search
    article_list = [ (sort_key(title), title) for title
                     in search_articles ]
    article_list.sort()

    index_matrix = {}
    index_matrix['\0\0\0'] = out_f.tell()
    for stripped_title, title in article_list:
        offset = out_f.tell()
        key3 = (title[0:3] + '\0\0\0')[0:3].lower()
        key2 = key3[0:2] + '\0'
        key1 = key3[0:1] + '\0\0'
        if key1 not in index_matrix:
            index_matrix[key1] = offset
        if key2 not in index_matrix:
            index_matrix[key2] = offset
        if key3 not in index_matrix:
            index_matrix[key3] = offset
        (article_number, dummy) = all_articles[title]
        all_articles[title] = (article_number, offset)
        out_f.write(struct.pack('Ib', article_number | (coll_number << coll_shift), 0)
                    + bigram_encode(title) + '\0' + title.encode('utf-8') + '\0')

    # fake entry so as to make sure the remaining articles don't show up accidentally
    # search.c recognizes that
    # TODO this is still a hack, but it is more or less ok.
    out_f.write(struct.pack('Ib', article_number | (coll_number << coll_shift), 0)
            + bigram_encode('@@@@@@@@@@@@@@@@@') + '\0' + '@@@@@@@@@@@@@@@@@' + '\0')

    # now write the other articles
    for title in all_articles:
        if title in search_articles:
            continue
        offset = out_f.tell()
        (article_number, dummy) = all_articles[title]
        all_articles[title] = (article_number, offset)
        out_f.write(struct.pack('Ib', article_number | (coll_number << coll_shift), 0)
                    + bigram_encode(title) + '\0' + title.encode('utf-8') + '\0')

    out_f.close()

if __name__ == "__main__":
    main()
