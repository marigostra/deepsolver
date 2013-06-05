

all: rpmtrans

clean:
	/bin/rm -f rpmtrans *.o

rpmtrans: rpmtrans.cpp
	g++ -Wall -pedantic -orpmtrans rpmtrans.cpp -lrpm
