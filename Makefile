CXX=g++

all: hookcisc

hookcisc: HookCISC_32.cpp
	$(CXX) -g -m32 -msse -o $@ $<

run: hookcisc
	./$<

clean:
	rm -f hookcisc

