zimcheck
========

Written by : Kiran Mathew Koshy
(kiranmathewkoshy@gmail.com)


This is a tool which can be used to check the quality of a ZIM file.
Here's a list of the checks that  are done on a ZIM file:

1 - Internal checksum: launch internal checksum verification

2 - Metadata Entries: checks if all metadata entries are present in the ZIM file.

3 - Favicon: Checks if the favicon is present in the ZIM file.

4 - Main Page entry: Checks wether the main page is present, and that it points to a valid article.

5 - Redundant data check: Checks if there are any redundant articles.

6 - Internal URL check: checks all interla URLs to make sure that they are valid URLs.

7 - MIME type check: checks the validity of MIME type of all articles in the ZIM file.

Usage: 

./zimcheckusage: ./zimcheck [options] zimfile

options:
  -A        run all tests. Default if no flags are given.
  -C        Internal CheckSum Test
  -M        MetaData Entries
  -F        Favicon
  -P       Main page
  -R        Redundant data check
  -U        URL checks
  -E       MIME checks

examples:
  ./zimcheck -A wikipedia.zim
  ./zimcheck -C wikipedia.zim
  ./zimcheck -F -R wikipedia.zim
  ./zimcheck -MI wikipedia.zim
  ./zimcheck -U wikipedia.zim
  ./zimcheck -R -U wikipedia.zim
  ./zimcheck -R -U -MI wikipedia.zim

