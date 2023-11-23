// You can include everything you want.
#include <utility>
#include <random>
#include <cstdlib>
// Please write the codes in this namespace except for the include part, or the program may crash.
namespace PlayerExample /* This name will be either Player1 or Player2, depending on which player you are. */ {
	using namespace std;
	// This is a special "Five Piece Game". (both row and column count from 1)
	// You are player X, and your enemy is player O.
	// Your task is to write an AI to compete with your enemy.
	// You can do anything you want in this namespace, including defining your own functions.
	// Good luck!

	// Here you can have some varibles or functions.
	int r, c;
	mt19937 mt;
	int bigrand(int l, int r) { uniform_int_distribution<int> distri(l, r); return distri(mt); }

	// This function will be only called once before the game starts.
	/// @param row the number of rows in the board.
	/// @param colum the number of colums in the board.
	/// @param first whether you play the first or the second. (if you play the first, it will be true. Otherwise, it will be false.)
	/// @param seed the seed which you can use to initilize your random device. (it will be displayed in the GUI, it's not necessary to use it if you don't want to debug by the seed)
	void init(int row, int colum, bool first, int seed) {
		r = row, c = colum;
		mt = mt19937(seed);
	}
	// This function will be called every time when it's your turn to put the piece.
	/// @param enemy the piece's position your enemy put last time. (if this is the first piece in this game, enemy will be {-1, -1}.)
	/// @return the piece's position you want to put.
	pair<int, int> put_piece(pair<int, int> enemy) {
		return make_pair(bigrand(1, r), bigrand(1, c));
	}
}