CFLAGS += -Wall -Wextra -fpic
LDFLAGS += -shared
LDLIBS += -ladm_iosched -lslurm

libslurmadmcli.so: slurmadmcli.o
	$(LINK.o) $^ $(LDLIBS) -o $@

clean:
	${RM} libslurmadmcli.so slurmadmcli.o