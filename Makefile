CXXFLAGS=-g -Wall -O2

play: play.cpp player1.h player2.h json.hpp
	$(CXX) $< -o $@ $(CXXFLAGS)

