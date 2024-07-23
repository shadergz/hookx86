CXX=g++

all: hookcisc

hookcisc: hook.cpp
	$(CXX) -g -m32 -msse -o $@ $<

run: hookcisc
	./$<

clean:
	rm -f hookcisc

