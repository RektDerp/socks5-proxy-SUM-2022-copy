sources = proxy_test.cpp
bindir = ./bin
programName = asio777

#TODO: переделать всё на CMake

all: linux windows

linux : $(sources) $(bindir)
	gcc $(sources) -o $(bindir)/$(programName) -lstdc++

windows : $(sources) $(bindir)
	x86_64-w64-mingw32-gcc $(sources) -o $(bindir)/$(programName).exe -static -lstdc++ -lws2_32 -lwsock32

$(bindir) :
	mkdir -p $(bindir)
clean :
	rm -rf *.o $(bindir)
format : 
	clang-format -i $(sources)