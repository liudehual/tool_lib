TARGET = libpng.a

OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

$(TARGET): $(OBJECTS)
	$(AR) cru $@ $?
	$(RANLIB) $@
	install -m 644 -D $(TARGET) ../build-linux/$(TARGET)
     
$(OBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

include ../Defs.make
