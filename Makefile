all: nadir

nadir: force_look driverlib
	@cd src; make all

driverlib: force_look
	@cd driverlib; make

force_look:
	@	true

clean:
	@cd src; make clean
	@cd driverlib; make clean
	@rm -f nadir.bin nadir.axf

install: all
	@cd src; make install

clean_inst: clean all install

