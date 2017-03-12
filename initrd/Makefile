TOPTARGETS = all clean
PROG_DIR = bin
SUBDIRS = $(filter-out $(PROG_DIR)/,$(wildcard */))
PROGS = $(join $(SUBDIRS),$(subst /,,$(SUBDIRS)))

all: initrd.tar

initrd.tar: $(PROGS)
	tar -cf $@ $(PROG_DIR)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(PROGS): $(SUBDIRS)
	-mkdir -p $(PROG_DIR)/
	cp $@ $(PROG_DIR)/

clean: $(SUBDIRS)
	-rm initrd.tar
	-rm -rf $(PROG_DIR)/

.PHONY: all clean $(SUBDIRS)
