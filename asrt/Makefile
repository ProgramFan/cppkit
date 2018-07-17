#
# Simple Makefile
#

default: build

build: libasrt.a

libasrt.a: asrt.o
	ar rcs -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

install: libasrt.a
	$(INSTALL) $(PREFIX)/lib/$^ $^

clean:
	rm -f *.o libasrt.a
