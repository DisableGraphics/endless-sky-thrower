all: linux windows

linux:
	g++ src/main.cpp -o build/linux/esthrower `pkg-config gtkmm-3.0 --cflags --libs` `curl-config --cflags --libs` -std=c++17

windows:
	i686-w64-mingw32.static-g++ src/main.cpp -o build/windows/esthrower.exe `i686-w64-mingw32.static-pkg-config gtkmm-3.0 --cflags --libs` `i686-w64-mingw32.static-curl-config --cflags --libs` -std=c++17

clean:
	rm build/esthrower