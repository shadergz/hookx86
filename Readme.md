# HookIA32

It's a hook function for x86 based architectures

## How this works?

![Image](https://miro.medium.com/max/1254/0*P2ntxJ78gmDvEtgX.PNG)

## The program output

![Image](hook.png)

## Details

- Used method: splicing (Trampoline)
- Works on x64 machines (with compatibility mode)
- Using mmap/mumap for allocate and deallocate pages for the trampolines
- Not portable for Windows or a non IEEE-1003 ([POSIX](https://standards.ieee.org/ieee/1003.1/7101/)) compatible

## More information

- [Hooking](https://en.wikipedia.org/wiki/Hooking)

## How to compile?

~~~bash
make
~~~

## How to run?

~~~bash
make run
~~~

## How to clean?

~~~bash
make clean
~~~
