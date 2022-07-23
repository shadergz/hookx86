all: Hook_IA32

Hook_IA32: Hook_IA32.cc
	$(CXX) -g -m32 -msse -o $@ $<

clean:
	rm -f Hook_IA32

