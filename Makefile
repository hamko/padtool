CFLAGS = -g -Wall --std=c++11 -O3
LFLAGS = -lm
all: a.out

a.out: inv_pad_damage.cpp
	g++ $(CFLAGS) $^ $(LFLAGS)
