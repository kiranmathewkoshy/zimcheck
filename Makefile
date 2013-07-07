zimcheck: zimcheck.cpp
	g++ -o zimcheck zimcheck.cpp  /usr/local/lib/libzim.a -llzma -std=c++0x -W
