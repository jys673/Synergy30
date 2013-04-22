#+
#   Global Synergy Makefile
#-
MAKE      = make
all : build clean

build :
	./configure
	echo "`pwd`"
	cd iolib ; $(MAKE) ;
	cd cnf;  $(MAKE);
	cd dac; touch *.c; $(MAKE);
	cd ssh; touch *.c; $(MAKE);
	cd cid; touch *.c; $(MAKE);
	cd pmd; touch *.c; $(MAKE);
	cd tsh; touch *.c; $(MAKE);
	cd fah; touch *.c; $(MAKE);
	cd libtest; touch *.c; $(MAKE);
	cd utils; touch *.c; $(MAKE);
	cd fdd; touch *.c; $(MAKE);
	cd api_lib; touch *.c; $(MAKE);
	cd ntsh; touch *.c; $(MAKE);
	cd bin; chmod 755 *; 
	cd obj; chmod 755 *;
clean :
	echo "done."
