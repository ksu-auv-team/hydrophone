src = $(wildcard *.c)
obj = $(src:.c=.o)

LDFLAGS = -lfftw3 -lsndfile -lm

fft_test: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) fft_test
