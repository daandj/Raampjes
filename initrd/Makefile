TOPTARGETS = all clean
SUBDIRS = $(filter-out programs/,$(wildcard */))
PROGS = $(join $(SUBDIRS),$(subst /,,$(SUBDIRS)))

all: initrd.tar

initrd.tar: $(PROGS)
	tar -cf $@ programs

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(PROGS): $(SUBDIRS)
	-mkdir programs/
	cp $@ programs/

clean: $(SUBDIRS)
	-rm initrd.tar
	-rm -rf programs/

.PHONY: all clean $(SUBDIRS)
