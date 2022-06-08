
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

#hash1: hash1.o
#	$(CC) -o hash1 hash1.o

CFLAGS = -g
default: hash1

clean:
	rm -f *.o
