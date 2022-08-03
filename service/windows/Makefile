CC=x86_64-w64-mingw32-g++
WINEPREFIX=$(HOME)/.wine

all : service.exe controls

controls:
	$(CC) -o controls.exe servicecontrols.c -static-libstdc++ -static-libgcc -mwindows

service.exe: service.o
	$(CC) -o service.exe main.cpp service.o -static-libstdc++ -static-libgcc -static

service.o :
	$(CC) -o service.o -c service.cpp -lKernel32.dll -lAdvapi32.dll

clean:
	rm -f *.o *.exe

