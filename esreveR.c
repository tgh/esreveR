/*
 * Copyright © 2009 Tyler Hayes
 * ALL RIGHTS RESERVED
 * [This program is licensed under the GPL version 3 or later.]
 * Please see the file COPYING in the source
 * distribution of this software for license terms.
 *
 * This LADSPA plugin simple reverses a given sound.
 *
 * NOTE: even though the official name of this plugin is "esreveR", the name
 * 'Reverse' is used throughout this code for readability.
 */

//----------------
//-- INCLUSIONS --
//----------------
#include <stdio.h>
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

//--------------------------------
//-- STRUCT FOR PORT CONNECTION --
//--------------------------------

typedef struct {
	//buffer pointers the host has connected to the plugin
	LADSPA_Data * Input;
	LADSPA_Data * Output;
} Reverse;

//---------------
//-- FUNCTIONS --
//---------------

/*
 * Creates a Reverse plugin instance
 */
LADSPA_HANDLE instantiate_Reverse(const LADSPA_Descriptor * descriptor, unsigned long sample_rate)
{
	
}

/*
 *
 */
void connect_port_to_Reverse(LADSPA_Handle instance, unsigned long Port, LADSPA_Data * data_location)
{
	
}

/*
 *
 */
void activate_Reverse(LADSPA_Handle instance)
{
	
}

/*
 *
 */
void run_Reverse(LADSPA_Handle instance, unsigned long sample_count)
{
	
}

/*
 *
 */
void deactivate_Reverse(LADSPA_Handle instance)
{
	
}

/*
 *
 */
void cleanup_Reverse(LADSPA_Handle instance)
{
	
}

LADSPA_Descriptor * reverse_descriptor = NULL;

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
		// assign the unique ID of the plugin given by ?
		reverse_descriptor->UniqueID = 0;	// This will be changed later when I get an offical ID <------
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
		/* assign the copyright info of the plugin (NOTE: use "None" for no copyright
		 * as per ladspa.h)
		 */
		reverse_descriptor->Copyright = strdup("None");	//might be changed later <-------
		/*
		 * assign the number of ports for the plugin.  since there are no control
		 * features for Reverse, there are only 2 ports: audio input and output.
		 */
		reverse_descriptor->PortCount = 2;
		
	}
	
}

/*
 * Returns a descriptor of the requested plugin type (there is only one plugin
 * type in this library).
 */
const LADSPA_Descriptor * get_ladpsa_descriptor(unsigned long index)
{
	if (index == 0)
		return reverse_descriptor;
	else
		return NULL;
}

/*
 *
 */
void _fini()
{
	
}
