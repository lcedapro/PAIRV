# application/freertos/embedded_proto/test/Makefile
CC = gcc
CFLAGS = -Wall -Wextra -g -I../inc -DTEST_X86
LDFLAGS =

TESTS = test_ringbuf test_frame_builder test_neuron_map test_uart_frame test_config_frame

all: $(TESTS)

test_ringbuf: test_ringbuf.c ../src/ringbuf.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_frame_builder: test_frame_builder.c ../src/frame_builder.c ../src/noc.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_neuron_map: test_neuron_map.c ../src/neuron_map.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_uart_frame: test_uart_frame.c ../src/uart_frame.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_config_frame: test_config_frame.c ../data/config_meta.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(TESTS)
	@for t in $(TESTS); do echo "=== $$t ==="; ./$$t || exit 1; done
	@echo "ALL TESTS PASSED"

clean:
	rm -f $(TESTS) *.o

.PHONY: all run clean
