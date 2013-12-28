CFLAGS = -g -Wall --std=c++0x -O3
LFLAGS = -lm
all: a.out

a.out: inv_pad_damage.cpp
	g++ $(CFLAGS) $^ $(LFLAGS)
