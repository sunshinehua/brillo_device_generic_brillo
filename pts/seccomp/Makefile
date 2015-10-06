TEST_PROGS := seccomp_bpf
CFLAGS += -Wl,-no-as-needed -Wall
LDFLAGS += -pthread

all: $(TEST_PROGS)

clean:
	$(RM) $(TEST_PROGS)

seccomp_bpf: seccomp_bpf.c test_harness.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@
