CC=x86_64-w64-mingw32-g++
WINEPREFIX=$(HOME)/.wine

all : main

main: service.o
	$(CC) -g main.cpp service.o -o service.exe -static-libstdc++ -static-libgcc --static
	cp service.exe $(WINEPREFIX)/drive_c

service.o :
	$(CC) -c service.cpp -o service.o -lKernel32.dll -lAdvapi32.dll

clean:
	rm -f *.o *.exe

