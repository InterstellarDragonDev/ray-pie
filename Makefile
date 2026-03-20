ray-pie: main.cpp
	g++ main.cpp backend.h backend.cpp file_managing.cpp toml.hpp json.hpp -o build/ray-pie -w -lraylib  -lGL -lm -lpthread -ldl -lrt -lX11
	./build/ray-pie
