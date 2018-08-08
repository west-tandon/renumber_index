
all:
	g++ -std=c++14 -Wall -Wextra -g -fcilkplus -O3 -march=native -o reorder main.cpp
