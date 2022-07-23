all: Hook_IA32

Hook_IA32: Hook_IA32.cc
	$(CXX) -g -m32 -msse -o $@ $<

run: Hook_IA32
	./Hook_IA32

clean:
	rm -f Hook_IA32

