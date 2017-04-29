/**
 * This file contains some data structure that are used in the problem
 */
#ifndef __OPTION_UTILITIES__H__
#define __OPTION_UTILITIES__H__
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <vector>
#include <getopt.h>
#include <string.h>

using namespace std;


struct optionFlag {
	
	optionFlag(){
		 output_required = true;
		 optimizeKnapsackCost = true;
		 extended = false;
		 timeout = 0.0;
		 optimize[0] = true;
		 optimize[1] = true;
		 optimize[2] = true;
	 }
	
	public:
	
	bool output_required;
	bool optimizeKnapsackCost;
	bool extended;
	double timeout;
	bool optimize[3];
	char filename[128];
	char output_filename[128];
	int threads;

};



optionFlag get_option(int argc,  char * argv[]);

#endif /* __OPTION_UTILITIES__H__ */

