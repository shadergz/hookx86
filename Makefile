CXX=g++

all: HookIA

HookIA: HookIA32.cpp
	$(CXX) -g -m32 -msse -o $@ $<

run: HookIA
	./$<

clean:
	rm -f HookIA

