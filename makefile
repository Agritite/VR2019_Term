all:
	g++ -std=c++17 -O3 -c *.cpp `pkg-config --cflags --libs opencv`
	g++ -std=c++17 -O3 -o watercolor *.o `pkg-config --cflags --libs opencv`
	rm *.o