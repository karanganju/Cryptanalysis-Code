#include <stdio.h>
#include <math.h>
#include <stdlib.h>
using namespace std;

int s, b;
int *Sbox;
int *Perm;

int **bias;

int pow2s[31];

#define num_Sbox pow(2, s/b);

//Returns true if all on bits in y are also on in x
bool contains(int x, int y) {
	return (x & y == y);
}

void calc_bias() {
	for (int i = 0; i < num_Sbox; i++) for (int j = 0; j < num_Sbox; j++) {
		int den_count = 0, num_count = 0;
		for (int k = 0; k < num_Sbox; k++) if contains(k, i) {
			den_count++;
			if contains(Sbox[k], j) num_count++;
		}
		while(den_count != num_Sbox) {
			den_count *= 2;
			num_count *= 2;
		}
		bias[i][j] = num_Sbox/2 - num_count;
	}
}

int main() {

	pow2s[0] = 1;
	for (int i = 1; i < 31; i++) pow2s[i] = pow2s[i - 1] * 2;

	printf("Input number of stages: ");
	scanf("%d", &s);
	printf("Input block size: ");
	scanf("%d", &b);

	if (s % b) {
		printf("s should be divisible by b!\n");
		exit(0);
	}
	//Assuming a single S-Box used for all stages and all sub-blocks
	//Assuming user gives permutation as well

	Sbox = (int *)malloc(num_Sbox * sizeof(int));
	printf("Enter all outputs of Sbox serially sith spaces(in decimal format): \n");
	for (int i = 0; i < num_Sbox; i++) scanf("%d", &Sbox[i]);

	Perm = (int *)malloc(s * sizeof(int));
	printf("Enter permutation serially for each stage(in decimal format): \n");
	for (int i = 0; i < s; i++) scanf("%d", &Perm[i]);

	bias = (int **)malloc(num_Sbox * num_Sbox * sizeof(int));
	calc_bias();


}