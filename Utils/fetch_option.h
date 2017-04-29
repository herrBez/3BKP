/**
 * This header provides the optionFlag data structure that is used to 
 * store the configuration of a particular instantiation of the program
 * @file fetch_option.h
 * @author Mirko Bez
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

/**
 * optionFlag the configuration data structure
 */
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
	/** Bool that says whether the standard CPLEX model and solution should be printed in two files */
	bool output_required;
	/** 
	 *  In the MultiKnapsack problem it says whether the fixed of the
	 *  knapsacks should be considered in the objective function or not
	 */
	bool optimizeKnapsackCost;
	/**
	 * Boolean variables that states if the balancing constraints should be considered
	 * or not.
	 */
	bool extended;
	/**
	 * Set the timeout for finding the CPLEX solution. In this case we are not guarantees that
	 * the solution is optimal (but at least is feasible)
	 */
	double timeout;
	/**
	 * Boolean vector with 3 entries corresponding to x-, y- and z-dimension and if at least one of them is
	 * true then the slave problem is considered.
	 */
	bool optimize[3];
	/**
	 * Contains the input file name
	 */
	char filename[128];
	/**
	 * Contains the output file name
	 */
	char output_filename[128];
	/**
	 * Number of threads to be used in the CPLEX problem. 
	 * Default = 0, i.e. CPLEX will decide on its own how many threads to use.
	 */
	int threads;

};


/**
 * Function that reads command line parameter options and put the configuration in an optionFlag data stracture.
 * @param argc the command line argument counter
 * @param argv the command line arguments
 * @return an optionFlag containing the configuration of that particular instantiation of the program.
 */
optionFlag get_option(int argc,  char * argv[]);

#endif /* __OPTION_UTILITIES__H__ */

