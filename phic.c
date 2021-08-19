#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <float.h>

#include "toolbox.h"

#define CMAX 256

static double ln_factorial (double n) {
	if (n == 0) return 0;
	double f = (0.5 * log(2 * 3.1415926))
		+ ((n + 0.5) * log((double)n))
		- n
		+ 1 / (12 * n)
		- 1 / (360 * pow((double)n, (double)3));
	return f;
}

double poisson[CMAX][CMAX];
static void init_poisson (void) {
	for (int m = 0; m < CMAX; m++) {
		for (int n = 0; n < CMAX; n++) {
			poisson[m][n] = (n * log((double)m)) - m - ln_factorial((int)n);
		}
	}
}

static char * usage = "\
usage: phic <file> <x> <c1> <c2> [... <cn>]\n\
  file = text file with one integer per line\n\
  x    = switch cost    [int]\n\
  c1   = count class 1  [int]\n\
  c2   = count class 2  [int]\n\
  cn   = count class n  [int]\n\
example: phic values.txt 10 1 5 15\n\
";

int main (int argc, char ** argv) {

	gkn_ivec class = gkn_ivec_new();     // HMM states
	gkn_ivec value = gkn_ivec_new();     // sequence of ints

	// cmdline
	if (argc < 5) gkn_exit(usage);
	int x = atoi(argv[2]);
	for (int i = 3; i < argc; i++) {
		int j = atoi(argv[i]);
		if (j >= CMAX) gkn_exit("exceeded maximim class value");
		gkn_ivec_push(class, j);
	}

	// read file
	FILE    *fp;
	char    line[32];
	if ((fp = fopen(argv[1], "r")) == NULL) gkn_exit("file open error");
	while (fgets(line, sizeof(line), fp) != NULL) {
		int i;
		if (sscanf(line, "%d", &i) != 1) gkn_exit("file read error");
		if (i >= CMAX) gkn_exit("exceeded maximum value");
		gkn_ivec_push(value, i);
	}
	
	// simplifications
	init_poisson();
	int states = class->size;
	int *state  = class->elem;
	int len = value->size;
	int *seq = value->elem;
	
	// create data structures
	double **score = malloc(states * sizeof(double*));
	int    **trace = malloc(states * sizeof(double*));
	for (int i = 0; i < states; i++) {
		score[i] = calloc(sizeof(double), len);
		trace[i] = calloc(sizeof(int)   , len);
	}
	
	// init
	for (int i = 0; i < states; i++) {
		score[i][0] = poisson[state[i]][seq[0]];
		trace[i][0] = i;
	}
	
	// fill
	for (int i = 1; i < len; i++) {
		for (int j = 0; j < states; j++) { // current state
			int max_state = -1;
			double max_score = -DBL_MAX;
			double emit = poisson[state[j]][seq[i]];
			for (int k = 0; k < states; k++) { // previous state
				double s = emit + score[k][i-1];
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
	double max_score = -DBL_MAX;
	int max_state = -1;
	for (int i = 0; i < states; i++) {
		if (score[i][len-1] > max_score) {
			max_score = score[i][len-1];
			max_state = i;
		}
	}
	int j = len;
	int k = len;
	for (int i = len-1; i > 0; i--) {
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