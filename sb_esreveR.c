/*
 * Copyright © 2009 Tyler Hayes
 * ALL RIGHTS RESERVED
 * [This program is licensed under the GPL version 3 or later.]
 * Please see the file COPYING in the source
 * distribution of this software for license terms.
 *
 * This LADSPA plugin reverses sections of the given sound.
 *
 * NOTE: even though the official name of this plugin is "esreveR", the name
 * 'Reverse' is used throughout this code for readability.
 */


//----------------
//-- INCLUSIONS --
//----------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ladspa.h"


//-----------------------
//-- DEFINED CONSTANTS --
//-----------------------
/*
 * These are the port numbers for the plugin
 */
#define REVERSE_INPUT 0
#define REVERSE_OUTPUT 1

/*
 * Other constants
 */
// the plugin's unique ID given by Richard Furse (ladspa@muse.demon.co.uk)
#define UNIQUE_ID 4302
// the number of ports involved
#define PORT_COUNT 2


//--------------------------------
//-- STRUCT FOR PORT CONNECTION --
//--------------------------------

typedef struct {
	// data locations for the input & output audio ports
	LADSPA_Data * Input;
	LADSPA_Data * Output;
} Reverse;


//---------------
//-- FUNCTIONS --
//---------------

/*
 * Creates a Reverse plugin instance by allocating space for a plugin handle.
 * This function returns a LADSPA_Handle (which is a void * -- a pointer to
 * anything).
 */
LADSPA_Handle instantiate_Reverse(const LADSPA_Descriptor * descriptor, unsigned long loc_sample_rate)
{
	Reverse * reverse;	// for a Reverse struct instance
	
	// allocate space for a Reverse struct instance
	reverse = (Reverse *) malloc(sizeof(Reverse));
	
	// send the LADSPA_Handle to the host. If malloc failed, NULL is returned.
	return reverse;
}

//-----------------------------------------------------------------------------

/*
 * Make a connection between a specified port and it's corresponding data location.
 * For example, the output port should be "connected" to the place in memory where
 * that sound data to be played is located.
 */
void connect_port_to_Reverse(LADSPA_Handle instance, unsigned long Port, LADSPA_Data * data_location)
{
	Reverse * reverse;
	
	// cast the (void *) instance to (Revolution *) and set it to local pointer
	reverse = (Reverse *) instance;
	
	// direct the appropriate data pointer to the appropriate data location
	if (Port == REVERSE_INPUT)
		reverse->Input = data_location;
	else if (Port == REVERSE_OUTPUT)
		reverse->Output = data_location;
}

//-----------------------------------------------------------------------------

/*
 * Here is where the actual audio manipulation is done.
 */
void run_Reverse(LADSPA_Handle instance, unsigned long sample_count)
{
	// set local pointer to plugin instance
	Reverse * reverse = (Reverse *) instance;

	// set local pointers to appropriate sample buffers
	LADSPA_Data * input = reverse->Input;
	LADSPA_Data * output = reverse->Output;
	
	// set an index into the input buffer starting with the last sample
	unsigned long index = sample_count - 1;

	// set the ouput samples to the reverse order of the input samples
	while (index >= 0)
	{
		*(output++) = input[index];
		--index;
	}
}

//-----------------------------------------------------------------------------

/*
 * Frees the dynamic memory allocated for the Reverse structure instance.
 */
void cleanup_Reverse(LADSPA_Handle instance)
{
	if (instance)
		free(instance);
}

//-----------------------------------------------------------------------------

// global LADSPA_Descriptor used by _init(), _fini(), and ladpsa_descriptor()
LADSPA_Descriptor * reverse_descriptor = NULL;

//-----------------------------------------------------------------------------

/*
 * The _init() function is called whenever this plugin is first loaded
 * by the host using it.
 */
void _init()
{
	/*
	 * allocate memory for reverse_descriptor (it's just a pointer at this point).
	 * in other words create an actual LADSPA_Descriptor struct instance that
	 * reverse_descriptor will point to.
	 */
	reverse_descriptor = (LADSPA_Descriptor *) malloc(sizeof(LADSPA_Descriptor));
	
	// make sure malloc worked properly before initializing the struct fields
	if (reverse_descriptor)
	{
		// assign the unique ID of the plugin
		reverse_descriptor->UniqueID = UNIQUE_ID;
		
		/*
		 * assign the label of the plugin. since there are no control features for this
		 * plugin, "reverse" is fine. (NOTE: it must not have white spaces as per ladspa.h).
		 * NOTE: in case you were wondering, strdup() from the string library makes a duplicate
		 * string of the argument and returns the duplicate's pointer (a char *).
		 */
		reverse_descriptor->Label = strdup("reverse");
		
		/*
		 * assign the special property of the plugin, which is any of the three
       * defined in ladspa.h: LADSPA_PROPERTY_REALTIME, LADSPA_PROPERTY_INPLACE_BROKEN,
		 * and LADSPA_PROPERTY_HARD_RT_CAPABLE.  They are just ints (1, 2, and 4,
		 * respectively).  See ladpsa.h for what they actually mean.
		 */
		reverse_descriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
		
		// assign the plugin name
		reverse_descriptor->Name = strdup("esreveR");
		
		// assign the author of the plugin
		reverse_descriptor->Maker = strdup("Tyler Hayes");
		
		/*
		 * assign the copyright info of the plugin (NOTE: use "None" for no copyright
		 * as per ladspa.h)
		 */
		reverse_descriptor->Copyright = strdup("GPL");
		
		/*
		 * assign the number of ports for the plugin.  since there are no control
		 * features for Reverse, there are only 2 ports: audio input and output.
		 */
		reverse_descriptor->PortCount = PORT_COUNT;
		
		LADSPA_PortDescriptor * temp_descriptor_array;	// used for allocating and initailizing a
																		// LADSPA_PortDescriptor array (which is
																		// an array of ints) since reverse_descriptor->
																		// PortDescriptors is a const *.
		
		// allocate space for the temporary array with a length of the number of ports (PortCount)
		temp_descriptor_array = (LADSPA_PortDescriptor *) calloc(2, sizeof(LADSPA_PortDescriptor));
		
		/*
		 * set the port properties by ORing specific bit masks defined in ladspa.h.
		 *
		 * this first one gives the first port the properties that tell the host that
		 * this port takes input and is an audio port (not a control port).
		 */
		temp_descriptor_array[REVERSE_INPUT] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		
		/*
		 * this gives the second port the properties that tell the host that this port is
		 * an output port and that it is an audio port (I don't see any situation where
		 * one might be an output port, but not an audio port...).
		 */
		temp_descriptor_array[REVERSE_OUTPUT] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		
		/*
		 * set the instance LADSPA_PortDescriptor array (PortDescriptors) pointer to
		 * the location temp_descriptor_array is pointing at.
		 */
		reverse_descriptor->PortDescriptors = (const LADSPA_PortDescriptor *) temp_descriptor_array;
		
		/*
		 * set temp_descriptor_array to NULL for housekeeping--we don't need that local
		 * variable anymore.
		 */
		temp_descriptor_array = NULL;
		
		char ** temp_port_names;	// temporary local variable (which is a pointer to an array
											// of arrays of characters) for the names of the ports since
											// reverse_descriptor->PortNames is a const char * const *.

		// allocate the space for two port names
		temp_port_names = (char **) calloc(PORT_COUNT, sizeof(char *));
		
		// set the name of the input port
		temp_port_names[REVERSE_INPUT] = strdup("Input");
		
		// set the name of the ouput port
		temp_port_names[REVERSE_OUTPUT] = strdup("Output");
		
		/*
		 * set the instance PortNames array pointer to the location temp_port_names
		 * is pointing at.
		 */
		reverse_descriptor->PortNames = (const char **) temp_port_names;
		
		// reset temp variable to NULL for housekeeping
		temp_port_names = NULL;
		
		LADSPA_PortRangeHint * temp_hints;	// temporary local variable (pointer to a
														// PortRangeHint struct) since reverse_descriptor->
														// PortRangeHints is a const *.
		
		// allocate space for two port hints (see ladspa.h for info on 'hints')									
		temp_hints = (LADSPA_PortRangeHint *) calloc(2, sizeof(LADSPA_PortRangeHint));
		
		/*
		 * set the port hint descriptors (which are ints). Since this is a simple
		 * reverse effect, input and ouput don't need any range hints.
		 */
		temp_hints[REVERSE_INPUT].HintDescriptor = 0;
		temp_hints[REVERSE_OUTPUT].HintDescriptor = 0;
		
		/* set the instance PortRangeHints pointer to the location temp_hints
		 * is pointed at.
		 */
		reverse_descriptor->PortRangeHints = (const LADSPA_PortRangeHint *) temp_hints;
		
		// reset temp variable to NULL for housekeeping
		temp_hints = NULL;
		
		// set the instance's function pointers to appropriate functions
		reverse_descriptor->instantiate = instantiate_Reverse;
		reverse_descriptor->connect_port = connect_port_to_Reverse;
		reverse_descriptor->activate = NULL;
		reverse_descriptor->run = run_Reverse;
		reverse_descriptor->run_adding = NULL;
		reverse_descriptor->set_run_adding_gain = NULL;
		reverse_descriptor->deactivate = NULL;
		reverse_descriptor->cleanup = cleanup_Reverse;
	}
	
}

//-----------------------------------------------------------------------------

/*
 * Returns a descriptor of the requested plugin type (there is only one plugin
 * type in this library).
 */
const LADSPA_Descriptor * ladspa_descriptor(unsigned long index)
{
	if (index == 0)
		return reverse_descriptor;
	else
		return NULL;
}

//-----------------------------------------------------------------------------

/*
 * This function is automatically called when the host is done with the plugin
 * (when the dynamic library is unloaded).  If frees all dynamic memory allocated
 * for the LADSPA_Descriptor structure instantiated by reverse_descriptor.
 */
void _fini()
{
	if (reverse_descriptor)
	{
		free((char *) reverse_descriptor->Label);
		free((char *) reverse_descriptor->Name);
		free((char *) reverse_descriptor->Maker);
		free((char *) reverse_descriptor->Copyright);
		
		int i = 0;
		for (i = 0; i < reverse_descriptor->PortCount; ++i)
			free((char *) reverse_descriptor->PortNames[i]);
		
		free((char **) reverse_descriptor->PortNames);
		free((LADSPA_PortRangeHint *) reverse_descriptor->PortRangeHints);
		
		free(reverse_descriptor);
	}
}

// EOF