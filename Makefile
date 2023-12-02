SUBDIRS= ./source/client ./source/server

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: clean  rmlogs install
clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

rmlogs:	
	rm -rf ./logs/*

install:
	mkdir -p ./install/
	rm -rf ./install/*
	cp ./build/* ./install