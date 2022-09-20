all: linux windows

linux:
	g++ src/main.cpp -o build/esthrower `pkg-config gtkmm-3.0 --cflags --libs` -laria2 -std=c++17

windows:
	x86_64-w64-mingw32-g++ src/main.cpp -o build/esthrower.exe `pkg-config gtkmm-3.0 --cflags --libs` -laria2 -std=c++17

clean:
	rm build/esthrower