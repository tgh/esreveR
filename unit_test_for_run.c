/*
 * Tyler Hayes - CS300 - HW2 - Unit Test Driver
 *
 * This is a driver for unit testing the run() function of my esreveR LADSPA
 * plugin.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ladspa.h"

//---------------------------
//-- FUNCTION DECLARATIONS --
//---------------------------

void run_Reverse(LADSPA_Handle instance, unsigned long sample_count);


//------------------------
//-- STRUCT DEFINITIONS --
//------------------------

typedef struct {
	LADSPA_Data sample_rate;
	
	// data locations for the input & output audio ports
	LADSPA_Data * Input;
	LADSPA_Data * Output;
} Reverse;


//----------------------
//-- GLOBAL VARIABLES --
//----------------------

// to hold the filename passed in by the user through the command line
char * filename;


//----------
//-- MAIN --
//----------
/*
 * takes three arguments, the sample rate, the number of samples, and
 * a file name to which the test data will be stored in.
 */
int main(int argc, char * argv[])
{
	// exit if run without 3 arguments
	if (argc != 4)
	{
		printf("\nNeed 3 arguments: sample rate, number of samples, and filename");
		printf(" for test results (in that order).\n");
		exit(-1);
	}
	
	// get the file name
	filename = malloc(sizeof(char) * strlen(argv[3]));
	if (!filename)
		exit(-1);
	strcpy(filename, argv[3]);

	// set the appropriate constants from the command line arguments
	const LADSPA_Data SAMPLE_RATE = (const LADSPA_Data) atof(argv[1]);
	const unsigned long BUFFER_SIZE = (unsigned long) atol(argv[2]);
	
	// create a pseudo Reverse instance
	Reverse * reverse = malloc(sizeof(Reverse));
	if (!reverse)
	{
		free(filename);
		exit(-1);
	}
	
	reverse->sample_rate = SAMPLE_RATE;
	
	// create a psuedo input buffer of audio samples.
	// the sample values are arbitrary, but they are sequential in order
	// to read the output easier to verify that each block was reversed.
	reverse->Input = malloc(sizeof(LADSPA_Data) * BUFFER_SIZE);
	if (!reverse->Input)
	{
		free(filename);
		free(reverse);
		exit(-1);
	}
	int i = 0;
	LADSPA_Data sample_val = 0.0f;
	for (i = 0; i < BUFFER_SIZE; ++i)
	{
		reverse->Input[i] = sample_val;
		sample_val += 1.0f;
	}
	
	// create the output buffer and initialize it to all zeroes
	reverse->Output = malloc(sizeof(LADSPA_Data) * BUFFER_SIZE);
	if (!reverse->Output)
	{
		free(filename);
		free(reverse->Input);
		free(reverse);
		exit(-1);
	}
	for (i = 0; i < BUFFER_SIZE; ++i)
		reverse->Output[i] = 0.0f;
	
	run_Reverse(reverse, BUFFER_SIZE);
	
	free(reverse->Input);
	free(reverse->Output);
	free(reverse);
	free(filename);
	
	return 0;
}

//--------------------------
//-- run_Reverse function --
//--------------------------
/*
 * This is the function that is being unit tested.  It is used in esreveR.c.
 * Portions of code used for file output for test data storage is sectioned
 * off for readablilty.  These sections, of course, are not used in esreveR.c.
 */
void run_Reverse(LADSPA_Handle instance, unsigned long total_sample_count)
{
	// set local pointer to plugin instance
	Reverse * reverse = (Reverse *) instance;

	// set local pointers to appropriate sample buffers
	LADSPA_Data * input_reader = reverse->Input;
	LADSPA_Data * output_writer = reverse->Output;
	
	// set the minimum amount of samples to reverse (0.2 seconds of sound)
	const unsigned long MIN_SAMPLES = 0.2 * reverse->sample_rate;
	// set the maximum amount of samples to reverse (1.5 seconds of sound)
	const unsigned long MAX_SAMPLES = 1.5 * reverse->sample_rate;
	
	// set a starting position index for the input buffer. This holds the
	// position at which the next sub-block of samples starts so that
	// the input index knows where to stop since it traverses backwards through
	// the input buffer.
	unsigned long start_position = 0;

	// set an index for the input reader and output writer
	unsigned long in_index = 0;
	unsigned long out_index = 0;

//----------------FILE INITIATION FOR TEST RESULTS-----------------------------
	FILE * write_file = NULL;
	write_file = fopen(filename, "w");
	if (!write_file)
	{
		printf("\n**Error: fail to create file 'test_results.txt'\n");
		return;
	}

	fprintf(write_file, "Sample Rate: %f", reverse->sample_rate);
	fprintf(write_file, "\nSample Count: %ld", total_sample_count);
//-----------------------------------------------------------------------------
	
	// break loop when the output index has reached the end of the output buffer
	while (out_index < total_sample_count)
	{
		// set a random number lower bound
		unsigned long rand_num_lower_bound = start_position + MIN_SAMPLES;
		// set a random number upper bound
		unsigned long rand_num_upper_bound = start_position + MAX_SAMPLES;
		// a random number to be set later
		unsigned long random_num = 0;
		
		// set the input index to the end of the buffer if the lower bound is
		// within MIN_SAMPLES of the end of the buffer or if it is beyond the
		// end of the buffer (this catches the special case where the whole
		// block passed in by the host is shorter than MIN_SAMPLES).
		if (MIN_SAMPLES >= total_sample_count
			 || rand_num_lower_bound >= total_sample_count - MIN_SAMPLES)
			in_index = total_sample_count - 1;
		
		else 
		{	
			// set the random number upper bound to the point where the number
			// of samples left in the buffer is MIN_SAMPLES if the upper bound is
			// within minimum samples away from the end of the buffer
			if (rand_num_upper_bound > total_sample_count - MIN_SAMPLES)
				rand_num_upper_bound = total_sample_count - MIN_SAMPLES;
			
			/*
			 * get a random number from a random number generator.
			 *
			 * The purpose of the random number here is to pick a random point in
			 * the input buffer (between the bounds) to which the input reader
			 * will start reading backwards in order to get random sizes of blocks
			 * to reverse.
			 *
			 * Seeding code referenced to Guy Rutenberg
			 * (http://www.guyrutenberg.com/2007/09/03/seeding-srand/)
			 */
			// seed the C library's random number generator with the current time
			struct timeval current_time;
			gettimeofday(&current_time, NULL);
			srand48 ((unsigned long)(current_time.tv_usec * current_time.tv_sec));
			// get a random number between lower bound and upper bound
			random_num = rand_num_lower_bound +
					(lrand48() % (rand_num_upper_bound - rand_num_lower_bound + 1));
			
			// set the input index to one less than the random number, because the
			// start position will become the point at random_num.
			in_index = random_num - 1;
		}

//-----------------WRITE NUMBER OF SAMPLES REVERSED----------------------------
		fprintf(write_file, "\n\nSub-block length: %ld samples", in_index - start_position + 1);
		fprintf(write_file, "\nValues:\n");
//-----------------------------------------------------------------------------
		
		// reverse the block.
		// NOTE: the test condition 'in_index != 0xFFFFFFFFFFFFFFFF' is for when
		// the start position is at zero (the beginning of the input buffer).
		// When 0 is decremented as an unsigned long it becomes that huge
		// positive number represented in hex as 0xFFFFFFFFFFFFFFFF, which of
		// course is always going to be greater than 0.	
		while (in_index >= start_position && in_index != 0xFFFFFFFFFFFFFFFF)
		{
			// set the output buffer's value to the appropriate input buffer value
			output_writer[out_index] = input_reader[in_index];
			
//--------------WRITING RESULT TO TEST FILE------------------------------------
			fprintf(write_file, "\t%f\n", output_writer[out_index]);
//-----------------------------------------------------------------------------
			
			// move forward in the output buffer
			++out_index;
			// move backward in the input buffer
			--in_index;
		}
		
		// reset the start position to the end of the block just reversed
		start_position = random_num;
	}
	
//----------------CLOSE TEST FILE----------------------------------------------
	fclose(write_file);
//-----------------------------------------------------------------------------
}

// EOF
