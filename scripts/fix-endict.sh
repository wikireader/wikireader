#!/bin/sh

# fix a duplicate entry in dictionary

for file in enwiktionary-*-pages-articles.xml
do
  dst="${file}-FIXED"
  if [ -e "${dst}" ]
  then
    echo already fixed: ${file}
  else
    echo -n fixing: ${file} ...
    # there is a zero width or 1 pixel width space just before '</title>'
    # so replace it with '-DUP'
    sed 's@<title>ឃើញ​</title>@<title>ឃើញ-DUP</title>@' < "${file}" > "${dst}"
    echo ' 'wrote: ${dst}
  fi
done
