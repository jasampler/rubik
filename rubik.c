/**
 * rubik - Console program to play with the Rubik's cube and save any position.
 *
 * Copyright 2022 Carlos Rica (jasampler)
 * This file is the main file of the jasampler's rubik project.
 * rubik is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * rubik is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with the rubik.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h> /* NULL, EOF, stderr, printf, fprintf, getchar */
#include <string.h> /* strchr, strcmp */
#include <ctype.h> /* isgraph */

#define NFACES 6
#define EMPTY_COLOR (-1)
#define NMINICUBES 20
#define NORIENTS 24
#define NROTATIONS NFACES
#define NVALUES_CUBE 48
#define NMINICUBES_PER_FACE 8

char ORIGCOLORS[NMINICUBES][NFACES];
char ORIENTATIONS[NORIENTS][NFACES];
char TRANSFORMATIONS[NROTATIONS][NORIENTS];

/* Faces of the cube and each minicube, used also as the colors of the faces:
       |z
       |___4___
      /.      /|
     / . 0   / |
    /_______/  |
    |  . . .| .|___
   2| .     |3 /  x
    |.  1   | /
    |_______|/
   /     5
  /y
*/

/** Adjacent 4 faces of each face from 0 to 5, in clockwise order from minor.
 * Also, shows the successive positions of the faces when rotating a minicube,
 * for example, rotating the minicube from the face in 0, face in 1 moves to 2,
 * face in 2 moves to 4, face in 4 moves to 3 and face in 3 moves to 1. */
char ADJACENTS[NFACES][4] = {
	{1, 2, 4, 3},
	{0, 3, 5, 2},
	{0, 1, 5, 4},
	{0, 4, 5, 1},
	{0, 2, 5, 3},
	{1, 3, 4, 2}
};
char OPPOSITES[NFACES] = {5, 4, 3, 2, 1, 0};

/** Ordered indexes of the non-empty faces of each minicube in x,y,z order. */
char CUBE_FACES_INDEXES[NVALUES_CUBE][2] = {
	{0, 2}, {0, 4}, {0, 5},
	{1, 4}, {1, 5},
	{2, 3}, {2, 4}, {2, 5},
	{3, 2}, {3, 5},
	{4, 3}, {4, 5},
	{5, 1}, {5, 2}, {5, 5},
	{6, 1}, {6, 5},
	{7, 1}, {7, 3}, {7, 5},
	{8, 2}, {8, 4},
	{9, 3}, {9, 4},
	{10, 1}, {10, 2},
	{11, 1}, {11, 3},
	{12, 0}, {12, 2}, {12, 4},
	{13, 0}, {13, 4},
	{14, 0}, {14, 3}, {14, 4},
	{15, 0}, {15, 2},
	{16, 0}, {16, 3},
	{17, 0}, {17, 1}, {17, 2},
	{18, 0}, {18, 1},
	{19, 0}, {19, 1}, {19, 3},
};

/* Standard positions of the minicubes in the cube, shown in vertical planes:
      |z
      |
      |12_13_14
     /15    16/
    /17_18_19/
      |8_____9_
     /        /
    /10____11/
      |0__1__2_____
     /3     4 /    x
    /5__6__7_/
   /
  /y
*/

/** Positions of the minicubes of each face, corners first and edges second,
 * in clockwise order each group, minor value first. */
char POSITIONS_BY_FACE[NFACES][NMINICUBES_PER_FACE] = {
	{12, 14, 19, 17,   13, 16, 18, 15},
	{ 5, 17, 19,  7,    6, 10, 18, 11},
	{ 0, 12, 17,  5,    3,  8, 15, 10},
	{ 2,  7, 19, 14,    4, 11, 16,  9},
	{ 0,  2, 14, 12,    1,  9, 13,  8},
	{ 0,  5,  7,  2,    1,  3,  6,  4}
};

/** Color of the given face of the minicube located in the given position. */
char init_color(int face, int x, int y, int z) {
	switch (face) {
		case 2: if (x == 0) { return face; } break;
		case 4: if (y == 0) { return face; } break;
		case 5: if (z == 0) { return face; } break;
		case 3: if (x == 2) { return face; } break;
		case 1: if (y == 2) { return face; } break;
		case 0: if (z == 2) { return face; } break;
	}
	return EMPTY_COLOR;
}

/** External characters for the colors, internally are from 0 to 5. */
char *COLOR_CHARS = "123456";
#define EMPTY_CHAR ' '

char color_to_char(char face) {
	return face != EMPTY_COLOR ? COLOR_CHARS[(int) face] : EMPTY_CHAR;
}

char char_to_color(char c) {
	char *p = strchr(COLOR_CHARS, c);
	return p != NULL ? p - COLOR_CHARS : EMPTY_COLOR;
}

/** Number of coloured faces in the minicube located in the given position. */
int count_minicube_colors(int x, int y, int z) {
	int count, f;
	count = 0;
	for (f = 0; f < NFACES; f++) {
		if (init_color(f, x, y, z) != EMPTY_COLOR) {
			count++;
		}
	}
	return count;
}

/** Colors of the faces of all external minicubes except the central ones,
 * in x,y,z order (modifying first the x, then the y and then the z). */
void init_original_minicube_colors() {
	int i, x, y, z, f;
	char *mc;
	i = 0;
	for (z = 0; z < 3; z++) {
		for (y = 0; y < 3; y++) {
			for (x = 0; x < 3; x++) {
				if (count_minicube_colors(x, y, z) > 1) {
					mc = ORIGCOLORS[i];
					for (f = 0; f < NFACES; f++) {
						mc[f] = init_color(f, x, y, z);
					}
					i++;
				}
			}
		}
	}
}

/** Given the current colors of the faces of a minicube returns its position
 * (currently searches the minicube colors in the initial state of the cube).
 * This function allows to calculate the positions using the orientations. */
int find_minicube_position(char minicubecolors[NFACES]) {
	int i, f, found;
	char *origminicubecolors;
	for (i = 0; i < NMINICUBES; i++) {
		origminicubecolors = ORIGCOLORS[i];
		found = 1;
		for (f = 0; f < NFACES; f++) {
			if (origminicubecolors[f] == EMPTY_COLOR) {
				if (minicubecolors[f] != EMPTY_COLOR) {
					found = 0;
					break;
				}
			} else if (minicubecolors[f] == EMPTY_COLOR) {
				found = 0;
				break;
			}
		}
		if (found) {
			return i;
		}
	}
	return -1;
}

void print_original_cube_colors() {
	int i, j;
	char c;
	for (i = 0; i < NMINICUBES; i++) {
		printf("%2d: ", i);
		for (j = 0; j < NFACES; j++) {
			c = ORIGCOLORS[i][j];
			if (c == EMPTY_COLOR) {
				printf(" .");
			} else {
				printf(" %d", c);
			}
		}
		printf("  (%d)\n", find_minicube_position(ORIGCOLORS[i]));
	}
}

/** Each orientation of a minicube is defined by a new distribution of faces. */
void init_minicube_orientations() {
	int i, f, a;
	char *orient, *adjacents;
	i = 0;
	for (f = 0; f < NFACES; f++) {
		adjacents = ADJACENTS[f];
		for (a = 0; a < 4; a++) {
			orient = ORIENTATIONS[i++];
			orient[0] = f;
			orient[1] = adjacents[a];
			orient[2] = adjacents[(a + 1) % 4];
			orient[3] = adjacents[(a + 3) % 4];
			orient[4] = adjacents[(a + 2) % 4];
			orient[5] = OPPOSITES[f];
		}
	}
}

#define LINE_BUFFER_SIZE 100L
/** Prints the lines replacing the one-char keys found with the given values. */
void print_template(char *linesarr, size_t nrows, size_t ncols,
		char *keys, char *values, char *indent) {
	int i, j, k;
	size_t maxlen;
	char *line, ch, key, value, tmpline[LINE_BUFFER_SIZE];
	maxlen = (ncols > LINE_BUFFER_SIZE ? LINE_BUFFER_SIZE : ncols) - 1L;
	for (i = 0; i < nrows; i++) {
		line = linesarr + (i * ncols);
		for (j = 0; j < maxlen && (ch = line[j]) != '\0'; j++) {
			for (k = 0; (key = keys[k]) != '\0'; k++) {
				if (ch == key) {
					value = values[k];
					break;
				}
			}
			tmpline[j] = (ch == key ? value : ch);
		}
		tmpline[j] = '\0';
		printf("%s%s\n", indent, tmpline);
	}
}

#define NROWS_ORIENT_TPL 9
#define NCOLS_ORIENT_TPL 14
char LINES_ORIENT_TPL[NROWS_ORIENT_TPL][NCOLS_ORIENT_TPL] = {
	   " ____E___",
	"   /.      /|",
	"  / . A   / |",
	" /_______/  |",
	" |  . . .| .|",
	"C| .     |D /",
	" |.  B   | /",
	" |_______|/",
	"      F"
};

void print_orientation_3d(char *orient) {
	int f;
	char values[NFACES];
	for (f = 0; f < NFACES; f++) {
		values[f] = color_to_char(orient[f]);
	}
	print_template((char *) LINES_ORIENT_TPL,
		NROWS_ORIENT_TPL, NCOLS_ORIENT_TPL, "ABCDEF", values, "   ");
}

void print_orientations_3d() {
	int i;
	for (i = 0; i < NORIENTS; i++) {
		if (i < 10) {
			printf("%d: ", i);
		} else {
			printf("%d:", i);
		}
		print_orientation_3d(ORIENTATIONS[i]);
	}
}

/** Rotates a minicube clockwise from the given face. */
void rotate_minicube(char face, char colors[NFACES]) {
	char *adjacents = ADJACENTS[(int) face], aux;
	aux = colors[(int) adjacents[0]];
	colors[(int) adjacents[0]] = colors[(int) adjacents[3]];
	colors[(int) adjacents[3]] = colors[(int) adjacents[2]];
	colors[(int) adjacents[2]] = colors[(int) adjacents[1]];
	colors[(int) adjacents[1]] = aux;
}

void copy_orientation(char dest[NFACES], char orig[NFACES]) {
	int f;
	for (f = 0; f < NFACES; f++) {
		dest[f] = orig[f];
	}
}

/* Finds in the global table the index of an orientation given by its colors.
 * Requires ORIENTATIONS[NORIENTS][NFACES]. */
char find_orientation(char searchedcolors[NFACES]) {
	int i, f;
	char *colors, found;
	for (i = 0; i < NORIENTS; i++) {
		colors = ORIENTATIONS[i];
		found = 1;
		for (f = 0; f < NFACES; f++) {
			if (colors[f] != searchedcolors[f]) {
				found = 0;
				break;
			}
		}
		if (found) {
			return i;
		}
	}
	return -1;
}

/** The transformations table knows the new orientation of a minicube
 * given the rotation applied to it and its previous orientation. Note that the
 * rotations are clockwise, for counter-clockwise, rotate by its opposite face.
 * Requires ORIENTATIONS[NORIENTS][NFACES]. */
void init_minicube_transformations() {
	int r, i;
	char *neworients, colors[NFACES];
	for (r = 0; r < NROTATIONS; r++) {
		neworients = TRANSFORMATIONS[r];
		for (i = 0; i < NORIENTS; i++) {
			copy_orientation(colors, ORIENTATIONS[i]);
			rotate_minicube(r, colors);
			neworients[i] = find_orientation(colors);
		}
	}
}

void print_transformations() {
	int r, i;
	char *neworients;
	printf("ORIENT|");
	for (i = 0; i < NORIENTS; i++) {
		printf(" %2d", i);
	}
	printf("\n------|");
	for (i = 0; i < NORIENTS; i++) {
		printf("---");
	}
	printf("\n");
	for (r = 0; r < NROTATIONS; r++) {
		printf("MOVE %d|", r);
		neworients = TRANSFORMATIONS[r];
		for (i = 0; i < NORIENTS; i++) {
			printf(" %2d", neworients[i]);
		}
		printf("\n");
	}
}

/** Assigns the colors of the minicube in the order of the given orientation. */
void apply_orientation(char newminicubecolors[NFACES],
			char origminicubecolors[NFACES], char orient[NFACES]) {
	int f;
	for (f = 0; f < NFACES; f++) {
		newminicubecolors[f] = origminicubecolors[(int) orient[f]];
	}
}

/** Saves the initial orientations and positions of the minicubes of the cube,
returning false if the given cube point has minicubes in the same position. */
char init_cube_point(char currentorients[NMINICUBES],
			char minicubesbypos[NMINICUBES], char *initialpoint) {
	int i;
	char minicubecolors[NFACES], pos;
	for (i = 0; i < NMINICUBES; i++) {
		currentorients[i] = initialpoint[i] - 'A';
		minicubesbypos[i] = -1;
	}
	for (i = 0; i < NMINICUBES; i++) {
		apply_orientation(minicubecolors, ORIGCOLORS[i],
				ORIENTATIONS[(int) currentorients[i]]);
		pos = find_minicube_position(minicubecolors);
		if (minicubesbypos[(int) pos] != -1) {
			return 0;
		}
		minicubesbypos[(int) pos] = i;
	}
	return 1;
}

void print_cube_3d_template(char cubecolors[NMINICUBES][NFACES],
		char *lines, size_t nrows, size_t ncols, char *keys) {
	int i, f;
	char *idx;
	char values[NVALUES_CUBE + 2];
	for (i = 0; i < NVALUES_CUBE; i++) {
		idx = CUBE_FACES_INDEXES[i];
		values[i] =
			color_to_char(cubecolors[(int) idx[0]][(int) idx[1]]);
	}
	values[i++] = '\\';
	for (f = 0; f < NFACES; f++) {
		values[i++] = color_to_char(f);
	}
	values[i] = '\0';
	print_template(lines, nrows, ncols, keys, values, "        ");
}

#define NROWS_CUBE_TPL 17
#define NCOLS_CUBE_TPL 63
char LINES_CUBE_TPL[NROWS_CUBE_TPL][NCOLS_CUBE_TPL] = {
	"         ___p_____q_____r__ (n      __C_____B_____A__ (E",
	"       Y/  A  /  B  /  C  /|       |     |     |     |",
	"       /_____/_____/_____/ |r     g|  r  |  q  |  p  |*A",
	"     Z/  D  /  E  /  F  /|g|       |_____|_____|_____|Y*",
	"     /_____/_____/_____/ |/|       |     |     |     | |*D",
	"   [/  G  /  H  /  I  /|h/ |o     d|  o  |  n  |  m  |*|Z*",
	"W) /_____/_____/_____/ |/|d|       |_____|_____|_____|V* |*G",
	"   |     |     |     |i/ |/|       |     |     |     | |*|[*",
	"  [|  P  |  Q  |  R  |/|e/ |l     a|  l  |  k  |  j  |*|W* |P",
	"   |_____|_____|_____| |/|a/       |_____|_____|_____|S* |*|",
	"   |     |     |     |f/ |/u    e) *  u  *  t  *  s  * |*|X|",
	"  X|  M  |  N  |  O  |/|b/         a*_____*_____*_____*|T* |M",
	"   |_____|_____|_____| |/x           *  x  *  w  *  v  * |*|",
	"   |     |     |     |c/             b*_____*_____*_____*|U|",
	"  U|  J  |  K  |  L  |/{               *  {  *  z  *  y  * |J",
	"   |_____|_____|_____|                 c*_____*_____*_____*|",
	"      y     z     {   (w                   L     K     J    (N"
};

/** The order of filling minicubes is z=0:stuvxyz{, z=1:moMO, z=2:ABCDFGHI */
#define KEYS_CUBE_TPL "SjsktaluTvbxJUyKzLc{VmdoMXOfAYpBqCgrDZFhGP[HQIRi*ENWenw"

/** Uses the minicube orientations to sort the colors, and then prints them.
 * Requires ORIGCOLORS[NMINICUBES][NFACES],ORIENTATIONS[NORIENTS][NFACES].*/
void print_cube_3d(char currentorients[NMINICUBES],
		char minicubesbypos[NMINICUBES]) {
	int i, mcidx;
	char newcubecolors[NMINICUBES][NFACES];
	for (i = 0; i < NMINICUBES; i++) {
		mcidx = minicubesbypos[i];
		apply_orientation(newcubecolors[i], ORIGCOLORS[mcidx],
				ORIENTATIONS[(int) currentorients[mcidx]]);
	}
	print_cube_3d_template(newcubecolors, (char *) LINES_CUBE_TPL,
			NROWS_CUBE_TPL, NCOLS_CUBE_TPL, KEYS_CUBE_TPL);
	printf("\n");
}

void print_positions_by_face() {
	int f, m;
	printf("FACE| ---EDGES--- --CORNERS--\n");
	for (f = 0; f < NFACES; f++) {
		printf("  %d |", f);
		for (m = 0; m < NMINICUBES_PER_FACE; m++) {
			printf("%3d", POSITIONS_BY_FACE[f][m]);
		}
		printf("\n");
	}
}

void move_minicubes(char mcbypos[NMINICUBES], char posarr[4]) {
	int tmp = mcbypos[(int) posarr[3]];
	mcbypos[(int) posarr[3]] = mcbypos[(int) posarr[2]];
	mcbypos[(int) posarr[2]] = mcbypos[(int) posarr[1]];
	mcbypos[(int) posarr[1]] = mcbypos[(int) posarr[0]];
	mcbypos[(int) posarr[0]] = tmp;
}

void move_minicubes_reverse(char mcbypos[NMINICUBES], char posarr[4]) {
	int tmp = mcbypos[(int) posarr[0]];
	mcbypos[(int) posarr[0]] = mcbypos[(int) posarr[1]];
	mcbypos[(int) posarr[1]] = mcbypos[(int) posarr[2]];
	mcbypos[(int) posarr[2]] = mcbypos[(int) posarr[3]];
	mcbypos[(int) posarr[3]] = tmp;
}

/** Reduces any number of times that a face must be rotated to: -2,-1,0,1,2 */
int reduce_times(int times) {
	times %= 4;
	if (times == 3) {
		return -1;
	}
	if (times == -3) {
		return 1;
	}
	return times;
}

/** Rotates a face a given number of times, negative to do it in reverse.
 * First changes the positions of the minicubes and then their orientations. */
void rotate_cube_face(char face, int times, char currentorients[NMINICUBES],
		char minicubesbypos[NMINICUBES]) {
	int t, n, p, i;
	char *posarr, *transform;
	if (! times) {
		return;
	}
	posarr = POSITIONS_BY_FACE[(int) face];
	if (times > 0) {
		n = t = times;
		do {
			move_minicubes(minicubesbypos, posarr);
			move_minicubes(minicubesbypos, posarr + 4);
		} while (--n);
		transform = TRANSFORMATIONS[(int) face];
	} else {
		n = t = -times;
		do {
			move_minicubes_reverse(minicubesbypos, posarr);
			move_minicubes_reverse(minicubesbypos, posarr + 4);
		} while (--n);
		transform = TRANSFORMATIONS[(int) OPPOSITES[(int) face]];
	}
	for (p = 0; p < NMINICUBES_PER_FACE; p++) {
		i = minicubesbypos[(int) posarr[p]];
		n = t;
		do {
			currentorients[i] = transform[(int) currentorients[i]];
		} while (--n);
	}
}

void print_cube_point(char currentorients[NMINICUBES]) {
	int i;
	char chars[NMINICUBES + 1];
	for (i = 0; i < NMINICUBES; i++) {
		chars[i] = currentorients[i] + 'A';
	}
	chars[i] = '\0';
	printf("        %s\n", chars);
}

/** Returns true if the argument has 20 uppercase letters in the range A-X. */
int valid_cube_point_chars(char *arg) {
	int i;
	char c;
	for (i = 0; (c = arg[i]) != '\0'; i++) {
		if (c < 'A' || c > 'A' + NORIENTS - 1) {
			return 0;
		}
	}
	if (i != 20) {
		return 0;
	}
	return 1;
}

#define MINUS '-'
#define APOS '\''

/** Returns true if the argument has 6 printable ASCII characters not - or '. */
char valid_color_chars(char *arg) {
	int i;
	char c;
	for (i = 0; (c = arg[i]) != '\0'; i++) {
		if (! isgraph(c) || c == APOS || c == MINUS
				|| strchr(arg, c) - arg < i) {
			return 0;
		}
	}
	if (i != 6) {
		return 0;
	}
	return 1;
}

void println(char *str) {
	fprintf(stderr, "%s\n", str);
}

void print_help() {
println("Use: rubik [-s|--silent] [-c|--chars UFLRBD] [-i|--initial POSITION]");
println("Shows a 3D representation of the Rubik's Cube in ASCII and");
println("allows to turn its faces by default entering the digits 1-6.");
println("");
println("  -c,--chars UFLRBD     6 characters to represent the colors");
println("  -i,--initial POSITION the 20 uppercase letters (A-X) printed after");
println("                        each move to recover again the same position");
println("  -s,--silent           prints only the POSITION and not the ASCII");
println("");
println("Entering the character shown in the center of a face turns");
println("that face clockwise one-quarter turn, and entering -N or N'");
println("turns the face N anticlockwise one-quarter turn. Applying");
println("the minus or the apostrophe again to the same number will");
println("have no effect. Any other unrecognized symbol is ignored.");
println("");
}

struct rubik_move_st {
	int times;
	char face;
};

/** Static variable to maintain the state while reading the program's input. */
struct rubik_input_state_st {
	int pendingsign, times;
	char lastchr, lastfacechr, pendingface, savedface;
	struct rubik_move_st result;
} ST;

/** Initializes the variable with the state of the program's input. */
void init_input_state(void) {
	ST.pendingface = ST.savedface = EMPTY_COLOR;
	ST.lastchr = ST.lastfacechr = '\0';
	ST.pendingsign = 1;
	ST.times = 0;
}

#define SAVE_FOUND_FACE 1
#define SAVE_PENDING_FACE 2
#define ROTATE_SAVED_FACE 4

/** Updates the input state and returns true if there is a move to apply.
 * To process the char '\n', this function must be called at least twice. */
char process_input_char(char c) {
	int action = 0;
	char f, hasresult = 0;
	if ((f = char_to_color(c)) != EMPTY_COLOR) {
		if (ST.pendingface != EMPTY_COLOR) {
			if (ST.savedface != EMPTY_COLOR
			&& ST.savedface != ST.pendingface) {
				action |= ROTATE_SAVED_FACE;
			}
			action |= SAVE_PENDING_FACE;
		}
		action |= SAVE_FOUND_FACE;
		ST.lastfacechr = c;
	} else {
		if (c == APOS) {
			if (ST.lastchr == ST.lastfacechr) {
				ST.pendingsign = -1;
			}
		} else if (c == '\n') {
			if (ST.pendingface != EMPTY_COLOR) {
				if (ST.savedface != EMPTY_COLOR
				&& ST.savedface != ST.pendingface) {
					action |= ROTATE_SAVED_FACE;
				}
				action |= SAVE_PENDING_FACE;
			} else if (ST.savedface != EMPTY_COLOR) {
				action |= ROTATE_SAVED_FACE;
			}
		}
		ST.lastfacechr = '\0';
	}
	if (action) {
		if (action & ROTATE_SAVED_FACE) {
			ST.result.face = ST.savedface;
			ST.result.times = ST.times;
			hasresult = 1;
			ST.savedface = EMPTY_COLOR;
			ST.times = 0;
		}
		if (action & SAVE_PENDING_FACE) {
			if (ST.savedface != ST.pendingface) {
				ST.savedface = ST.pendingface;
			}
			ST.pendingface = EMPTY_COLOR;
			ST.times += ST.pendingsign;
			ST.pendingsign = 1;
		}
		if (action & SAVE_FOUND_FACE) {
			ST.pendingface = f;
			if (ST.lastchr == MINUS) {
				ST.pendingsign = -1;
			}
		}
	}
	ST.lastchr = c;
	return hasresult;
}

/*
#Repeat the sequence of moves 12 to find when the initial position is recovered:
gcc -ansi -Wall -pedantic -o rubik rubik.c
yes 12 | head -200 | ./rubik -s | nl | grep AAAAAAAAAAAAAAAAAAAA
*/
int main(int argc, char *argv[]) {
	int i, c;
	char silent = 0, *initialpoint = "AAAAAAAAAAAAAAAAAAAA";
	char currentorients[NMINICUBES], minicubesbypos[NMINICUBES];
	char stop;
	for (i = 1; i < argc; i++) {
		if (! strcmp(argv[i], "-s") || ! strcmp(argv[i], "--silent")) {
			silent = 1;
		} else if (i + 1 < argc && (! strcmp(argv[i], "-c")
					|| ! strcmp(argv[i], "--chars"))) {
			COLOR_CHARS = argv[++i];
			if (! valid_color_chars(COLOR_CHARS)) {
				fprintf(stderr,
					"Invalid color characters: %s\n\n",
					COLOR_CHARS);
				print_help();
				return -3;
			}
		} else if (i + 1 < argc && (! strcmp(argv[i], "-i")
					|| ! strcmp(argv[i], "--initial"))) {
			initialpoint = argv[++i];
			if (! valid_cube_point_chars(initialpoint)) {
				fprintf(stderr,
					"Invalid initial position: %s\n\n",
					initialpoint);
				print_help();
				return -2;
			}
		} else {
			print_help();
			return -1;
		}
	}
	init_original_minicube_colors();
	init_minicube_orientations();
	init_minicube_transformations();
	if (! init_cube_point(currentorients, minicubesbypos, initialpoint)) {
		fprintf(stderr, "Invalid initial position: %s\n\n",
			initialpoint);
		print_help();
		return -2;
	}
	init_input_state();
	c = '\n';
	while (c != EOF) {
		stop = 1;
		do {
			if (process_input_char(c)) {
				rotate_cube_face(ST.result.face,
						reduce_times(ST.result.times),
						currentorients, minicubesbypos);
			}
			if (c == '\n') {
				if (--stop == -1) {
					if (! silent) {
						print_cube_3d(currentorients,
								minicubesbypos);
					}
					print_cube_point(currentorients);
				}
			}
		} while (! stop);
		c = getchar();
	}
	return 1;
}

