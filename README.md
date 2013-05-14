zimcheck
========

Written by : Kiran Mathew Koshy
(kiranmathewkoshy@gmail.com)

Please note that this program is under heavy development and all versions may not be stable.


1 - Internal checksum: launch internal checksum verification

2 - Dead internal urls: check all ZIM internal urls an verify if the target
exists. That means css/javascript loading urls, images src and url href.... an
probably a few others

3 - Checks that urls in CSS files are not external, and internal urls are valid

4 - Veryfy that there is not online dependencies (images, javascript/css
loading, ....) in HTML code

5 - Check if the following metadata entries are there: title, creator,
publisher, date, description language. Check if date and language are in the
correct format.http://openzim.org/wiki/Metadata 

6 - Verify that the favicon is there 

7 - Verify the main page header entry is defined and point to a valid content.

8 - Check duplicate content: be sure that the same content is not available
under two different url. For example two times the same picture.

9 - Verify that internal urls are not absolute
