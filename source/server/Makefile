SRC=main.c ftpServer.c ../common/common.c
CC=gcc
LIB=-lpthread
DATE=$(shell date +%Y-%m-%d-%H-%M-%S)
BUILDDIR=../../build
LOGDIR=../../logs/$(DATE)
OUTFILE=$(BUILDDIR)/ftpServer
LOGFILE=$(LOGDIR)/serverbuild.log

$(OUTFILE):$(SRC) | $(BUILDDIR) $(LOGDIR)
	$(CC) -o $@ $^ 2>&1 | tee $(LOGFILE)
	cp $@ $(LOGDIR)/

$(BUILDDIR) $(LOGDIR):
	mkdir -p $@

.PHONY:clean rmlogs install
clean:
	rm -rf $(BUILDDIR)/*

rmlogs:	
	rm -rf ../../logs/*

install: $(OUTFILE)
	mkdir -p ../../install/
	cp $(OUTFILE) ../../install/
