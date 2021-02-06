#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <float.h>

#include "toolbox.h"

#define CMAX 256

static double ln_factorial (double n) {
	double f;
	if (n == 0) return 0;
	f = (0.5 * log(2 * 3.1415926))
		+ ((n + 0.5) * log((double)n))
		- n
		+ 1 / (12 * n)
		- 1 / (360 * pow((double)n, (double)3));
	return f;
}

double poisson[CMAX][CMAX];
static void init_poisson (void) {
	int m, n;
	for (m = 0; m < CMAX; m++) {
		for (n = 0; n < CMAX; n++) {
			poisson[m][n] = (n * log((double)m)) - m - ln_factorial((int)n);
		}
	}
}

static void yikes (const char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

static char * usage = "\
usage: nclassifier <file> <x> <c1> <c2> [... <cn>]\n\
  file = text file with one integer per line\n\
  x    = switch cost    [int]\n\
  c1   = count class 1  [int]\n\
  c2   = count class 2  [int]\n\
  cn   = count class n  [int]\n\
example: nclassifier values.txt 10 1 5 15\n\
";

int main (int argc, char ** argv) {
	int     i, j, k;
	int     x;                         // switch cost
	int     len, *seq, states, *state; // aliases
	ik_ivec class = ik_ivec_new();     // HMM states
	ik_ivec value = ik_ivec_new();     // sequence of ints
	FILE    *fp;
	char    line[16];
	double  **score;                   // score matrix
	int     **trace;                   // trace matrix
	int     max_state;
	double  max_score, s, emit;

	// cmdline
	if (argc < 5) yikes(usage);
	x = atoi(argv[2]);
	for (i = 3; i < argc; i++) {
		j = atoi(argv[i]);
		if (j >= CMAX) yikes("exceeded maximim class value");
		ik_ivec_push(class, j);
	}

	// read file
	if ((fp = fopen(argv[1], "r")) == NULL) yikes("file open error");
	while (fgets(line, sizeof(line), fp) != NULL) {
		if (sscanf(line, "%d", &i) != 1) yikes("file read error");
		if (i >= CMAX) yikes("exceeded maximum value");
		ik_ivec_push(value, i);
	}
	
	// simplifications
	init_poisson();
	states = class->size;
	state  = class->elem;
	len    = value->size;
	seq    = value->elem;
	
	// create data structures
	score = malloc(states * sizeof(double*));
	trace = malloc(states * sizeof(double*));
	for (i = 0; i < states; i++) {
		score[i] = calloc(sizeof(double), len);
		trace[i] = calloc(sizeof(int)   , len);
	}
	
	// init
	for (i = 0; i < states; i++) {
		score[i][0] = poisson[state[i]][seq[0]];
		trace[i][0] = i;
	}
	
	// fill
	for (i = 1; i < len; i++) {
		for (j = 0; j < states; j++) { // current state
			max_state = -1;
			max_score = -DBL_MAX;
			emit = poisson[state[j]][seq[i]];
			for (k = 0; k < states; k++) { // previous state
				s = emit + score[k][i-1];
				if (j != k) s -= x; // switch cost
				if (s > max_score) {
					max_score = s;
					max_state = k;
				}
			}
			score[j][i] = max_score;
			trace[j][i] = max_state;
		}
	}
	
	// traceback 
	max_score = -DBL_MAX;
	max_state = -1;
	for (i = 0; i < states; i++) {
		if (score[i][len-1] > max_score) {
			max_score = score[i][len-1];
			max_state = i;
		}
	}
	j = len;
	k = len;
	for (i = len-1; i > 0; i--) {
		if (trace[max_state][i] == max_state) {
			j = i;
		} else {
			printf("%d\t%d\t%d\n", j, k, state[max_state]);
			j = i;
			k = i;
			max_state = trace[max_state][i];
		}
	}
	printf("%d\t%d\t%d\n", j, k, state[max_state]);
	
	return 0;
}