all:
	g++ src/main.cpp -o build/esthrower `pkg-config gtkmm-3.0 --cflags --libs` -laria2 -std=c++17