// The Computer Language Benchmarks Game
// http://benchmarksgame.alioth.debian.org/
//
// contributed by Jeremy Zerfas
// rewritten by Аноним Легионов, inspired by fasta Rust #2 program
// use two OpenMP locks instead of one critical section
// decouples IO activity from random number generation
//
// modified by Josh Goldfoot, adding use of a buffer for fasta_repeat


// This controls the width of lines that are output by this program.
#define MAXIMUM_LINE_WIDTH	60

// This program will generate the random nucleotide sequences in parallel which
// are worked on in blocks of lines. The number of lines in those blocks is
// controlled by this setting.
#define LINES_PER_BLOCK 1024

#define CHARACTERS_PER_BLOCK (MAXIMUM_LINE_WIDTH*LINES_PER_BLOCK)

#define THREADS_TO_USE 4

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _OPENMP
#include <omp.h>
#endif

// intptr_t should be the native integer type on most sane systems.
typedef intptr_t intnative_t;

typedef struct
{
	char letter;
	float probability;
} nucleotide_info;


// Repeatedly print string_To_Repeat until it has printed
// number_Of_Characters_To_Create. The output is also wrapped to
// MAXIMUM_LINE_WIDTH columns.
static char* make_Sequence_Buffer(const char string_To_Repeat[])
{
	const intnative_t string_To_Repeat_Length = strlen(string_To_Repeat);

	// JG: Changed to that this writes a sequence to a buffer, which is used
	intnative_t number_Of_Characters_To_Create = string_To_Repeat_Length * MAXIMUM_LINE_WIDTH;
	char* buffer = (char*) malloc(number_Of_Characters_To_Create + 
		number_Of_Characters_To_Create / MAXIMUM_LINE_WIDTH + 1);
	if (buffer == NULL)
		exit(-1);
	char* bufferOffset = buffer;

	// Create an extended_String_To_Repeat which is a copy of string_To_Repeat
	// but extended with another copy of the first MAXIMUM_LINE_WIDTH characters
	// of string_To_Repeat appended to the end. Later on this allows us to
	// generate a line of output just by doing simple memory copies using an
	// appropriate offset into extended_String_To_Repeat.
	char extended_String_To_Repeat[string_To_Repeat_Length + MAXIMUM_LINE_WIDTH];
	for (intnative_t column = 0; column<string_To_Repeat_Length + MAXIMUM_LINE_WIDTH;
		column++)
		extended_String_To_Repeat[column] =
		string_To_Repeat[column%string_To_Repeat_Length];
	intnative_t offset = 0;

	char line[MAXIMUM_LINE_WIDTH + 1];
	line[MAXIMUM_LINE_WIDTH] = '\n';

	for (intnative_t current_Number_Of_Characters_To_Create =
		number_Of_Characters_To_Create;
		current_Number_Of_Characters_To_Create>0;)
	{
		// Figure out the length of the line we need to write. If it's less than
		// MAXIMUM_LINE_WIDTH then we also need to add a line feed in the right
		// spot too.
		intnative_t line_Length = MAXIMUM_LINE_WIDTH;
		if (current_Number_Of_Characters_To_Create<MAXIMUM_LINE_WIDTH)
		{
			line_Length = current_Number_Of_Characters_To_Create;
			line[line_Length] = '\n';
		}

		memcpy(line, extended_String_To_Repeat + offset, line_Length);

		// Update the offset, reducing it by string_To_Repeat_Length if
		// necessary.
		offset += line_Length;
		if (offset>string_To_Repeat_Length)
			offset -= string_To_Repeat_Length;

		// "Output" that line to our buffer and update the
		// current_Number_Of_Characters_To_Create.
		memcpy(bufferOffset, line, line_Length + 1); //  JG: used to be fwrite(line, line_Length + 1, 1, stdout);
		bufferOffset += line_Length + 1;
		current_Number_Of_Characters_To_Create -= line_Length;
	}
	*bufferOffset = 0;
	return buffer;
}

void repeat_And_Wrap_String(const char string_To_Repeat[], intnative_t number_Of_Characters_To_Create) 
{
	/* JG: fasta_repeat repeats every len(alu) * line-length = 287 * 61 = 17507 characters.
	       So, calculate this once, then just print that buffer over and over. */
	char* sequence = make_Sequence_Buffer(string_To_Repeat);
	intnative_t sequenceLen = (intnative_t) strlen(sequence);
	intnative_t outputBytes = number_Of_Characters_To_Create + number_Of_Characters_To_Create / 60;
	while (outputBytes >= sequenceLen) {
		fwrite(sequence, sequenceLen, 1, stdout);
		outputBytes -= sequenceLen;
	}
	if (outputBytes > 0) {
		fwrite(sequence, outputBytes, 1, stdout);
		printf("\n"); 
	}
	free(sequence);
}


// Generate a pseudorandom number from 0 to max using a linear
// congruential generator.
#define IM		139968
#define IA		3877
#define IC		29573
uint32_t seed = 42;

int rng_tid; //Thread ID
int rng_tnum = 1; //Thread number
intnative_t rng_cnt = 0;

#ifdef _OPENMP
omp_lock_t rng_lock;
#define RNG_LOCK_INIT() omp_init_lock(&rng_lock)
#define RNG_LOCK() omp_set_lock(&rng_lock)
#define RNG_FREE() omp_unset_lock(&rng_lock)
#else
#define RNG_LOCK_INIT() do{}while(0)
#define RNG_LOCK() do{}while(0)
#define RNG_FREE() do{}while(0)
#endif

static void rng_init(void)
{
	RNG_LOCK_INIT();
	rng_tid = 0;
}

static intnative_t rng_gen_blk(uint32_t * buf, intnative_t len, int curr_tid)
{
	intnative_t gen_cnt = -1;//Error by default
	RNG_LOCK();

	if (rng_tid == curr_tid)
	{
		if (++rng_tid >= rng_tnum)
		{
			rng_tid = 0;
		}

		gen_cnt = (len<rng_cnt) ? len : rng_cnt;
		rng_cnt -= gen_cnt;

		len = gen_cnt;

		while (0 != len--)
		{
			seed = (seed*IA + IC) % IM;
			*(buf++) = seed;//This is stupid actually!
		}
	}

	RNG_FREE();
	return gen_cnt;
}

int out_tid; //Thread ID
int out_tnum = 1; //Thread number
#ifdef _OPENMP
omp_lock_t out_lock;
#define OUT_LOCK_INIT() omp_init_lock(&out_lock)
#define OUT_LOCK() omp_set_lock(&out_lock)
#define OUT_FREE() omp_unset_lock(&out_lock)
#else
#define OUT_LOCK_INIT() do{}while(0)
#define OUT_LOCK() do{}while(0)
#define OUT_FREE() do{}while(0)
#endif

static void out_init(void)
{
	OUT_LOCK_INIT();
	rng_tid = 0;
}

static intnative_t out_write(char * buf, intnative_t len, int curr_tid)
{
	intnative_t wr_cnt = -1;//Error by default
	OUT_LOCK();

	if (out_tid == curr_tid)
	{
		if (++out_tid >= out_tnum)
		{
			out_tid = 0;
		}
		wr_cnt = fwrite(buf, len, 1, stdout);
	}

	OUT_FREE();
	return wr_cnt; //-1 - thread error, 0 - IO error, 1 - ОК
}

static void generate_And_Wrap_Pseudorandom_DNA_Sequence(
	const nucleotide_info nucl_info[],
	const intnative_t nucl_num,
	const intnative_t char_num)
{
	uint32_t cumul_p[nucl_num];
	float cumul_acc = 0.0;

	for (intnative_t i = 0; i<nucl_num; i++)
	{
		cumul_acc += nucl_info[i].probability;
		cumul_p[i] = 1ul + (uint32_t)(cumul_acc*(float)IM); //Compensate rounding errors on test file
	}

#ifdef _OPENMP
	intnative_t tnum = omp_get_num_procs();
	if (tnum>THREADS_TO_USE) tnum = THREADS_TO_USE;
	omp_set_num_threads(tnum);
	rng_tnum = tnum;
	out_tnum = tnum;
#endif
	rng_tid = 0;
	out_tid = 0;
	rng_cnt = char_num;

#pragma omp parallel
	{
		char block[CHARACTERS_PER_BLOCK + LINES_PER_BLOCK];
		char * line;
		uint32_t rnd[CHARACTERS_PER_BLOCK], r;
		intnative_t cnt, col, prid, nid, ncnt;
		int cur_tid;
#ifdef _OPENMP
		cur_tid = omp_get_thread_num();
#else
		cur_tid = 0;
#endif
		while (1)
		{
			do
			{
				cnt = rng_gen_blk(rnd, CHARACTERS_PER_BLOCK, cur_tid);
			} while (-1 == cnt);

			if (0 == cnt)
			{
				break;//Work finished!
			}

			line = block;

			for (col = 0, prid = 0; prid < cnt; prid++)
			{
				r = rnd[prid];

				ncnt = 0;
				for (nid = 0; nid < nucl_num; nid++)
				{
					if (cumul_p[nid] <= r)
					{
						ncnt++;
					}
				}

				*line++ = nucl_info[ncnt].letter;
				if (++col >= MAXIMUM_LINE_WIDTH)
				{
					col = 0;
					*line++ = '\n';
				}
			}
			//Check if we need to end the line
			if (0 != col)
			{
				//Last iteration didn't end the line, so finish the job.
				*line++ = '\n';
			}
			//Print results
			do
			{
				cnt = out_write(block, line - block, cur_tid);
			} while (-1 == cnt);
			//Check  fot IO error
			if (0 == cnt)
			{
				exit(1);
			}
		}
	}
}


int main(int argc, char ** argv)
{
	const intnative_t n = atoi(argv[1]);

	fputs(">ONE Homo sapiens alu\n", stdout);
	const char homo_Sapiens_Alu[] =
		"GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTC"
		"AGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCG"
		"TGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGG"
		"AGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA";
	repeat_And_Wrap_String(homo_Sapiens_Alu, 2 * n);

	rng_init();
	out_init();

	fputs(">TWO IUB ambiguity codes\n", stdout);
	nucleotide_info iub_Nucleotides_Information[] =
	{
		{ 'a', 0.27 },{ 'c', 0.12 },{ 'g', 0.12 },{ 't', 0.27 },{ 'B', 0.02 },
		{ 'D', 0.02 },{ 'H', 0.02 },{ 'K', 0.02 },{ 'M', 0.02 },{ 'N', 0.02 },
		{ 'R', 0.02 },{ 'S', 0.02 },{ 'V', 0.02 },{ 'W', 0.02 },{ 'Y', 0.02 }
	};
	generate_And_Wrap_Pseudorandom_DNA_Sequence(iub_Nucleotides_Information,
		sizeof(iub_Nucleotides_Information) / sizeof(nucleotide_info), 3 * n);

	fputs(">THREE Homo sapiens frequency\n", stdout);
	nucleotide_info homo_Sapien_Nucleotides_Information[] =
	{
		{ 'a', 0.3029549426680 },{ 'c', 0.1979883004921 },
		{ 'g', 0.1975473066391 },{ 't', 0.3015094502008 }
	};
	generate_And_Wrap_Pseudorandom_DNA_Sequence(homo_Sapien_Nucleotides_Information,
		sizeof(homo_Sapien_Nucleotides_Information) / sizeof(nucleotide_info), 5 * n);

	return 0;
}
