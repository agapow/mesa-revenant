PACKAGE = mesa
VERSION = 1.9.5
TAR_NAME = $(PACKAGE)
DIST_DIR = $(TAR_NAME)-$(VERSION)

all clean install $(PACKAGE):
	$(MAKE) -C src $@

doc docs:
	$(MAKE) -C doc

dist: $(DIST_DIR).tar.gz

$(DIST_DIR).tar.gz: FORCE $(DIST_DIR)
	tar chof - $(DIST_DIR) |\
	 gzip -9 -c >$(DIST_DIR).tar.gz
	rm -rf $(DIST_DIR)

$(DIST_DIR):
	mkdir -p $(DIST_DIR)/src
	cp Makefile $(DIST_DIR)
	cp Makefile $(DIST_DIR)
	cp src/Makefile $(DIST_DIR)/src
	cp src/*.cpp src/*.h $(DIST_DIR)/src

FORCE:
	-rm $(DIST_DIR).tar.gz &> /dev/null
	-rm -rf $(DIST_DIR) &> /dev/null

.PHONY: all clean dist
