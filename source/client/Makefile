SRC=main.c ftpClient.c ../common/common.c
CC=gcc
DATE=$(shell date +%Y-%m-%d-%H-%M-%S)
BUILDDIR=../../build
LOGDIR=../../logs/$(DATE)
OUTFILE=$(BUILDDIR)/ClientFTP
LOGFILE=$(LOGDIR)/clientbuild.log

$(OUTFILE):$(SRC) | $(BUILDDIR) $(LOGDIR)
	$(CC) -o $@ $^ -lmysqlclient 2>&1 | tee $(LOGFILE)
	cp $@ $(LOGDIR)/

$(BUILDDIR) $(LOGDIR):
	mkdir -p $@

.PHONY:clean rmlogs install
clean:
	rm -rf $(BUILDDIR)/*

rmlogs:	
	rm -rf ../../logs/*

install: $(OUTFILE)
	mkdir -p ../../bin/
	cp $(OUTFILE) ../../bin/
