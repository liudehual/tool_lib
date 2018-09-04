TARGET = libCxImage.a

OBJECTS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

$(TARGET): $(OBJECTS)
	$(AR) cru $@ $?
	$(RANLIB) $@
	install -m 644 -D $(TARGET) ../build-linux/$(TARGET)
     
$(OBJECTS): %.o: %.cpp
	$(CXX) -c -I../jasper/include $(CXXFLAGS) $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

include ../Defs.make
