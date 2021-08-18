STARK_GIT = "https://github.com/SideChannelMarvels/Stark.git"
SUBDIRS = simu Stark

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

simu:
		$(MAKE) -C $@

Stark:
	@if [ ! -d $@ ] ;\
	then \
	  echo "Stark does not exist, fetching"; \
	  git clone $(STARK_GIT); \
	fi
	$(MAKE) -C $@

clean:
	@rm -vf tracefile_r10 tracefile_r11 tracefile_r10_convert
	$(MAKE) clean -C simu
	@rm -rvf Stark
