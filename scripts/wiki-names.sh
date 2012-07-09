
#!/bin/sh
# conversion of short names to readable names or file names
# to use: source "$(dirname "$0")/wiki-names.sh"

# type : name : filename
# Note: no spaces allowed in any line or shell quoting breaks
ContentList='
appro:Appropedia:Appropedia-pages-articles.xml:
books:wikibooks:wikibooks-pages-articles.xml:
dict:wiktionary:wiktionary-pages-articles.xml:
guten:Project-Gutenberg:gutenberg:
how:Wikihow:wikihow-pages-articles.xml:
pedia:wikipedia:wiki-pages-articles.xml:
quote:wikiquote:wikiquote-pages-articles.xml:
starw:Wookieepedia:Wookieepedia-pages-articles.xml:
fgrlm:forgottenrealms:forgottenrealms-pages-articles.xml:
dand:DandD:dand-pages-articles.xml:
trav:wikitravel:wikitravel-pages-articles.xml:
'

ListOfAllContentTypes=
ListOfAllFilePrefixes=
for item in ${ContentList}
do
  t="${item%%:*}"
  item="${item#*:}"
  n="${item%%:*}"
  item="${item#*:}"
  f="${item%%:*}"

  ListOfAllContentTypes="${ListOfAllContentTypes} ${t}"
  f="${f%%-*}"
  ListOfAllFilePrefixes="${ListOfAllFilePrefixes} ${f}"
done
ListOfAllContentTypes="${ListOfAllContentTypes# }"
ListOfAllFilePrefixes="${ListOfAllFilePrefixes# }"


# convert to readable name
TypeToName()
{
  local type="$1"; shift

  for item in ${ContentList}
  do
    t="${item%%:*}"
    item="${item#*:}"
    n="${item%%:*}"
    if [ X"${type}" = X"${t}" ]
    then
      echo ${n}
      return 0
    fi
  done
  return 1
}


# convert to filename
TypeToFilename()
{
  local type="$1"; shift

  for item in ${ContentList}
  do
    t="${item%%:*}"
    item="${item#*:}"
    item="${item#*:}"
    fn="${item%%:*}"
    if [ X"${type}" = X"${t}" ]
    then
      echo ${fn}
      return 0
    fi
  done
  return 1
}
