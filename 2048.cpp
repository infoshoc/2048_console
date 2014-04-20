#include <cstdio>
#include <cstdlib>
#include <conio.h>
#include <Windows.h>

bool try_again(){
	return true;
}

const int N = 4;
const short FINISH_TILE = 2048;

long long score;
short field[N][N];

inline bool check_coord ( int i, int j ) { return i >= 0 && i < N && j >= 0 && j < N; }

enum game_result{
	CONTINUE,
	WIN, 
	LOOSE
} get_game_result(){
	bool continue_flag = true;
	for ( int i = 0; i < N; ++i ) {
		for ( int j = 0; j < N; ++j ) {
			if ( field[i][j] == FINISH_TILE ) {
				return WIN;
			}
			if ( field[i][j] == 0 ) {
				continue_flag = true;
				continue;
			}
			static const int D = 4;
			static const int DI[D] = { -1,  0,  0, +1 };
			static const int DJ[D] = {  0, -1, +1,  0 };

			for ( int d = 0; d < D; ++d ) {
				if ( check_coord ( i + DI[d], j + DJ[d] ) && field[i][j] == field[i + DI[d]][j + DJ[d]] ) {
					continue_flag = true;
				}
			}
		}
	}
	return continue_flag ? CONTINUE : LOOSE;
}

void magnet( const int mult_i, const int mult_j, const int d_i, const int d_j ){

	static bool merged[N][N];

	memset ( merged, false, sizeof ( bool ) * N * N );

	for ( int di = 1; di <= N; ++di ) {
		for ( int dj = 1; dj <= N; ++dj ) {
			int i = ( N + di * mult_i ) % N;
			int j = ( N + dj * mult_j ) % N;
			
			if ( field[i][j] == 0 ) {
				continue;
			}

			int new_i = i;
			int new_j = j;

			do{
				new_i += d_i;
				new_j += d_j;
			} while ( check_coord ( new_i, new_j ) && field[new_i][new_j] == 0 );

			if ( check_coord ( new_i, new_j ) && field[i][j] == field[new_i][new_j] && !merged[new_i][new_j] ) {
				field[new_i][new_j] += field[i][j];
				score += field[new_i][new_j];
				field[i][j] = 0;
				merged[new_i][new_j] = true;
				continue;
			}
			new_i -= d_i;
			new_j -= d_j;

			short tile = field[i][j];
			field[i][j] = 0;
			field[new_i][new_j] = tile;
		}
	}
}
void magnet_left  (){magnet ( +1, +1,  0, -1 );}
void magnet_right (){magnet ( +1, -1,  0, +1 );}
void magnet_top   (){magnet ( +1, +1, -1,  0 );}
void magnet_bottom(){magnet ( -1, +1, +1,  0 );}

void put_random(){
	static int empty_cells[N*N][2];
	int empty_cells_size = 0;

	for ( int i = 0; i < N; ++i ) {
		for ( int j = 0; j < N; ++j ) {
			if ( field[i][j] == 0 ) {
				empty_cells[empty_cells_size][0] = i;
				empty_cells[empty_cells_size][1] = j;
				++empty_cells_size;
			}
		}
	}

	if ( empty_cells_size ) {
		int idx = rand() % empty_cells_size;
		field[empty_cells[idx][0]][empty_cells[idx][1]] = 1 << ( rand() % 2 + 1 );
	}
}

enum command{
	LEFT, RIGHT, TOP, BOTTOM
} read_command(){
	while ( true ) {
		static bool arrow_key = false;
		int c = _getch();

		if ( arrow_key ) {
			arrow_key = false;
			switch ( c ) {
				case 75:
					return LEFT;
				case 77:
					return RIGHT;
				case 72:
					return TOP;
				case 80:
					return BOTTOM;
			}
			continue;
		}

		switch ( c ) {
			case 0xE0:
			case 0:
				arrow_key = true;
				break;
			case VK_ESCAPE:
			case 'Q':
			case 'q':
				exit ( 0 );
		}
	}
}

void print (){
	system ( "cls" );

	printf ( "Score:%lld\n", score );
	for ( int i = 0; i < N; ++i ) {
		for ( int j = 0; j < N; ++j ) {
			printf ( "%5hd", field[i][j] );
		}
		putchar ( '\n' );
	}
}

void play(){	
	static game_result result;

	memset ( field, 0, sizeof ( short ) * N * N );

	while ( ( result = get_game_result() ) == CONTINUE ) {
		put_random();
		print();
		switch ( read_command() ) {
			case LEFT:
				magnet_left();
				break;
			case RIGHT:
				magnet_right();
				break;
			case TOP:
				magnet_top();
				break;
			case BOTTOM:
				magnet_bottom();
				break;
		}
	}

	switch ( result ) {
	case WIN:
		printf ( "You Win\nScores:%lld\n", score );
	case LOOSE:
		printf ( "Game Over\nScores:%lld\n", score );
	}
}

int main(){
	
	do {
		play();
	} while ( try_again() ); 

	return 0;
}