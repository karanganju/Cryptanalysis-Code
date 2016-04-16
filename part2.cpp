#include <stdio.h>
#include <math.h>
#include <stdlib.h>
using namespace std;

#define LL long long int

LL s, b;
LL *Sbox;
LL *Perm;
LL *rPerm;

LL *bias;

LL pow2s[63];

#define num_Sbox pow2s[b/s]

void calc_bias() {
	for (LL i = 0; i < num_Sbox; i++) for (LL j = 0; j < num_Sbox; j++) {
			LL num_count = 0;
			for (LL k = 0; k < num_Sbox; k++) {
				LL ti = i;
				LL tj = j;
				LL tx = k;
				LL ty = Sbox[k];
				LL out = 0;
				for (LL iter = 0; iter < b / s; iter++) {
					out ^= ((ti % 2) & (tx % 2)) ^ ((tj % 2) & (ty % 2));
					ti /= 2;
					tj /= 2;
					tx /= 2;
					ty /= 2;
				}
				if (out) num_count++;
			}
			bias[i * num_Sbox + j] = num_Sbox / 2 - num_count;
		}
	printf("Bias table: \n");
	for (LL i = 0; i < num_Sbox; i++) {
		for (LL j = 0; j < num_Sbox; j++) printf("%lld ", bias[i * num_Sbox + j]);
		printf("\n");
	}
}

void permute(LL &c) {
	LL k = c;
	c = 0;
	for (LL i = 0; i < b; i++) {
		c |= (((c & (1 << i)) >> i) << Perm[i]);
	}
	return;
}

void rpermute(LL &c) {
	LL k = c;
	c = 0;
	for (LL i = 0; i < b; i++) {
		c |= (((k & (1 << i)) >> i) << rPerm[i]);
	}
	return;
}

void substitute(LL &c, LL &bias_num, LL subs_vec) {
	for (LL i = 0; i < s; i++) {
		bias_num *= 2 * bias[((c & (LL)(pow2s[(i + 1) * (b / s)] - 1)) >> (i * b / s)) * num_Sbox + (subs_vec & (LL)(pow2s[(i + 1) * (b / s)] - 1)) >> (i * b / s)];
	}
	c = subs_vec;
}

LL rev(LL c, LL bits) {
	LL k = 0;
	for (int i = 0; i < bits; i++) {
		k |= ((c % 2) << (bits - i - 1));
		c /= 2;
	}
	return k;
}

//Assuming each stage has a round key operation
//This method should print the best combination using the bias array given above and the Perm array
void calc_best_paths() {
	LL best_shot = 0;

	LL max_bias = 0;

	for (LL iter = 0; iter < pow2s[b * (s + 1)]; iter++) {
		LL itercopy = iter;
		LL bias_num = 1;
		bool faulty = false;
		for (LL i = 0; i < s; i++) {
			LL round_inp = itercopy % pow2s[b];
			LL next_inp =  (itercopy >> b) % pow2s[b];
			if (round_inp == 0 || next_inp == 0) {
				faulty = true;
				break;
			}
			if (i < s - 1) rpermute(next_inp);
			for (LL j = 0; j < s; j++) {
				bias_num *= bias[rev((round_inp % pow2s[b / s]), b / s) * num_Sbox + rev((next_inp % pow2s[b / s]), b / s)] / 2;
				round_inp = round_inp >> b / s;
				next_inp = next_inp >> b / s;
			}
			itercopy = itercopy >> b;
		}
		if (faulty) continue;
		if ((bias_num * bias_num) > (max_bias * max_bias)) {
			best_shot = iter;
			max_bias = bias_num;
		}
	}

	printf("Best bias : %lld\n", max_bias * 2);
	for (LL i = 0; i < s + 1; i++) {
		LL round_bits = best_shot % pow2s[b];
		LL next_round_bits = 0;
		best_shot = best_shot >> b;
		if (i != s) next_round_bits = best_shot % pow2s[b];
		int bits[b];
		if (i == 0) printf("Plaintext / Round 0 Key bits : \n");
		else if (i == s) printf("Ciphertext / Round %lld Key bits : \n", s);
		else printf("Round %lld Key bits : \n", i);
		LL round_bits_copy = round_bits;
		for (LL j = 0; j < b; j++) {
			bits[b - 1 - j] = round_bits_copy % 2;
			round_bits_copy = round_bits_copy >> 1;
		}
		for (LL j = 0; j < b; j++) printf("%d  ", bits[j]);
		if (i < s)printf("\nBiases : ");

		if (i < s) for (LL j = 0; j < s; j++) {
				printf("%lld ( %lld , %lld ) ", bias[rev((round_bits % pow2s[b / s]), b / s) * num_Sbox + rev((next_round_bits % pow2s[b / s]), b / s)], rev((round_bits % pow2s[b / s]), b / s), rev((next_round_bits % pow2s[b / s]), b / s));
				round_bits = round_bits >> b / s;
				next_round_bits = next_round_bits >> b / s;
			}
		printf("\n");
	}
}

int main() {
	pow2s[0] = 1;
	for (LL i = 1; i < 31; i++) pow2s[i] = pow2s[i - 1] * 2;

	printf("Input number of stages: ");
	scanf("%d", &s);
	printf("Input block size (<=32): ");
	scanf("%d", &b);

	if (b % s) {
		printf("b should be divisible by s!\n");
		exit(0);
	}

	//Assuming a single S-Box used for all stages and all sub-blocks
	//Assuming user gives permutation as well

	Sbox = (LL *)malloc(num_Sbox * sizeof(LL));
	printf("Enter all outputs of Sbox serially with spaces(in decimal format): \n");
	for (LL i = 0; i < num_Sbox; i++) scanf("%d", &Sbox[i]);

	Perm = (LL *)malloc(b * sizeof(LL));
	rPerm = (LL *)malloc(b * sizeof(LL));
	printf("Enter permutation serially for each stage(in decimal format starting from 0): \n");
	for (LL i = 0; i < b; i++) {
		scanf("%d", &Perm[i]);
		rPerm[Perm[i]] = i;
	}


	bias = (LL *)malloc(num_Sbox * num_Sbox * sizeof(LL));
	calc_bias();

	// Assuming that S-box and P-box are the same across all stages
	calc_best_paths();

}