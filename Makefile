all: clean subdirs

SUBDIRS= ./source/client ./source/server

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: clean rmlogs install
clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done


rmlogs:
	rm -rf ./logs/*

install:
	mkdir -p ./bin/
	find ./bin/ ! -name '*.txt' ! -name '*.md' -type f -exec rm -f {} +
	cp ./build/* ./bin

client: clean ./source/client
	$(MAKE) -C ./source/client -lssl -lcrypto -I /usr/include/mysql/

server: clean ./source/server
	$(MAKE) -C ./source/server -lssl -lcrypto -I /usr/include/mysql/
