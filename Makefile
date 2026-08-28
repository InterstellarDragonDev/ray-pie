ray-pie: main.cpp
	g++ main.cpp backend.h backend.cpp file_managing.cpp json.hpp nanosvg.h nanosvgrast.h images.h raygui.h gui.cpp -o build/ray-pie -w -lraylib  -lGL -lm -lpthread -ldl -lrt -lX11
	./build/ray-pie
