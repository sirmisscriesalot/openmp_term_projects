default:
	g++ -o ./build/histeq -fopenmp ./src/histogram_equalization.cpp -I/usr/include/python3.8 -lpython3.8
