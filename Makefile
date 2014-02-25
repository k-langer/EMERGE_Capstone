all: emerge qt
emerge:
	cd src && $(MAKE)
qt:
	make -f Makefile.qt
install:
	cd src && make install
clean:
	cd src && make clean
	make clean -f Makefile.qt
