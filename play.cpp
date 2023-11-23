/**
 * Version: 0.2.3-linux
 * Description & Changelog: https://www.luogu.com.cn/paste/oqnjnuc3
 */
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include <unistd.h>

#include "json.hpp"

#include "player1.h"
#include "player2.h"

using nlohmann::json;

#define player1_ch "()"
#define player2_ch "[]"

void setcolor(int b, int f) 
{
	printf("\033[%d;%dm", b, f);
}
void setxy(int x, int y) 
{ 
	printf("\033[%d;%dH", x + 1, y + 1); 
}
int rand(int l, int r) { return rand() % (r - l + 1) + l; }

int win_piece, player1_c, player2_c, text_c, line_c, board_c, background_c;

int row, column, speed, first_player;
int sd1, sd2;
int board[1000][1000];

void quit(bool ok, const char *title, const char *text) { if(!ok) fprintf(stderr, "%s\n%s\n", title, text), exit(-1); }
void read_config() {
	try {
		std::ifstream config_file("config.json");
		json config;
		config_file >> config;
		if(config["behavior"]["row"] == 0) row = rand(config["behavior"]["row_bound"][0], config["behavior"]["row_bound"][1]);
		else row = config["behavior"]["row"];
		quit((row == 0) || (5 <= row && row <= 999), "ERROR in config.json", "row should be either equal to 0 or between 5 and 999.");
		if(config["behavior"]["column"] == 0) column = rand(config["behavior"]["column_bound"][0], config["behavior"]["column_bound"][1]);
		else column = config["behavior"]["column"];
		quit((column == 0) || (5 <= column && column <= 999), "ERROR in config.json", "column should be either equal to 0 or between 5 and 999.");
		if(config["behavior"]["first"] == 0) first_player = rand(1, 2);
		else first_player = config["behavior"]["first"];
		quit((first_player == 0 || first_player == 1 || first_player == 2), "ERROR in config.json", "first should be between 0 and 2.");
		speed = config["behavior"]["speed"];
		quit((speed >= 0), "ERROR in config.json", "speed shouldn't be negative.");
		win_piece = config["behavior"]["win"];
		quit((5 <= win_piece && win_piece <= 999), "ERROR in config.json", "win should be between 5 and 999.");
		sd1 = config["behavior"]["player1_seed"];
		if(sd1 == 0) sd1 = std::random_device{}();
		sd2 = config["behavior"]["player2_seed"];
		if(sd2 == 0) sd2 = std::random_device{}();
		player1_c = config["theme"]["player1"];
		player2_c = config["theme"]["player2"];
		text_c = config["theme"]["text"];
		line_c = config["theme"]["line"];
		board_c = config["theme"]["board"];
		background_c = config["theme"]["background"];
	} catch(nlohmann::detail::exception &e) {
		puts("An ERROR occured unexpectedly, please report the following information to the auther to solve it.\n");
		puts(e.what());
		exit(-1);
	}
}

void init() {
	setcolor(board_c, line_c);
	printf("┌");
	for(int i = 1; i <= column - 2; i++) printf("─┬");
	printf("─┐");
	puts("");
	for(int i = 1; i <= row - 2; i++) {
		printf("├");
		for(int j = 1; j <= column - 2; j++) printf("─┼");
		printf("─┤");
		puts("");
	}
	printf("└");
	for(int i = 1; i <= column - 2; i++) printf("─┴");
	printf("─┘");

	setxy(3, 2 * column + 4);
	setcolor(board_c, text_c);
	printf("Size: %d×%d", row, column);

	setxy(4, 2 * column + 4);
	setcolor(board_c, text_c);
	printf("Player 1: ");
	setcolor(board_c, player1_c);
	printf("%s", player1_ch);
	setcolor(board_c, text_c);
	printf("   Seed：%d", sd1);

	setxy(5, 2 * column + 4);
	setcolor(board_c, text_c);
	printf("Player 2: ");
	setcolor(board_c, player2_c);
	printf("%s", player2_ch);
	setcolor(board_c, text_c);
	printf("   Seed：%d", sd2);

	setxy(6, 2 * column + 4);
	setcolor(board_c, text_c);
	printf("First mover: %d", first_player);
}

bool put_piece(int r, int c, int t) {
	if(r < 1 || r > row || c < 1 || c > column || board[r][c]) return false;
	setxy(r - 1, c * 2 - 2);
	setcolor(board_c, t == 1 ? player1_c : player2_c);
	printf("%s", t == 1 ? player1_ch : player2_ch);
	board[r][c] = t;
	return true;
}
void prompt(const char *text) {
	setxy(7, 2 * column + 4);
	setcolor(board_c, text_c);
	if(!text) printf("                              ");
	else printf("Info: %s", text);
}
void change_prompt(char c) {
	setxy(7, 2 * column + 26);
	setcolor(board_c, text_c);
	putchar(c);
}
const int dir[8][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
int check_win(std::pair<int, int> piece, int player) {
	for(int d = 0; d < 4; d++) {
		int cnt = 0;
		int x = piece.first, y = piece.second;
		while(true) {
			if((x >= 1 && x <= row && y >= 1 && y <= column) && (board[x][y] == player)) cnt++;
			else break;
			x += dir[d][0], y += dir[d][1];
		}
		x = piece.first, y = piece.second;
		while(true) {
			if((x >= 1 && x <= row && y >= 1 && y <= column) && (board[x][y] == player)) cnt++;
			else break;
			x -= dir[d][0], y -= dir[d][1];
		}
		if(cnt >= win_piece + 1) return player;
	}
	return 0;
}

int main() {
	setvbuf(stdout, nullptr, _IONBF, 0);

	printf("\033c");
	srand(time(0));
	read_config();
	init();
	int winner = 0, crash = 0, piece_count = 0;
	Player1::init(row, column, first_player == 1, sd1);
	Player2::init(row, column, first_player == 2, sd2);
	prompt("Now it's player 2's turn");
	std::pair<int, int> last = {-1, -1}, now;
	if(first_player == 2) {
		now = Player2::put_piece(last);
		if(!put_piece(now.first, now.second, 2)) { crash = 1, winner = 1; goto End; }
		winner = check_win(now, 2);
		piece_count++;
		last = now;
		if(winner || piece_count == row * column) goto End;
		usleep(speed);
	}
	while(true) {
		change_prompt('1');
		now = Player1::put_piece(last);
		if(!put_piece(now.first, now.second, 1)) { crash = 1, winner = 2; goto End; }
		winner = check_win(now, 1);
		piece_count++;
		last = now;
		if(winner || piece_count == row * column) goto End;
		usleep(speed);
		change_prompt('2');
		now = Player2::put_piece(last);
		if(!put_piece(now.first, now.second, 2)) { crash = 1, winner = 1; goto End; }
		winner = check_win(now, 2);
		piece_count++;
		last = now;
		if(winner || piece_count == row * column) goto End;
		usleep(speed);
	}
End:
	prompt(NULL);
	if(winner == 0) {
		setcolor(board_c, text_c);
		setxy(row + 1, 0);
		printf("Draw");
	} else {
		setcolor(board_c, text_c);
		setxy(row + 1, 0);
		if(crash) printf("Player %d tries to place a piece at (%d, %d). ", 3 - winner, now.first, now.second);
		setxy(row + 2, 0);
		printf("Player %d wins!", winner);
	}
	return 0;
}

// vim:noet:ts=8:sw=8
