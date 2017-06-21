/*!
 * @author Mirko Bez
 * @file MasterProblem.h
 * 
 */
#ifndef __MASTER__PROBLEM__H__
#define __MASTER__PROBLEM__H__
#include "../Utils/fetch_option.h"
#include "../Utils/Instance3BKP.h"
#include "../Utils/cpxmacro.h"
#include "utils.h"
#include <algorithm>


/**
 * set up the model for CPLEX
 * @param env the cplex enviroment
 * @param lp the cplex problem
 * @param instance the 3(B)KP instance of the problem
 * @param oFlag an object containing the configuration information
 * @return an object containing the indixes of the variables inside the CPLEX problem 
 */	
mapVar setupLP(CEnv env, Prob lp, Instance3BKP instance, optionFlag oFlag);

/**
 * fetches the variables' values after the solution is found
 * @param env the CPLEX environment
 * @param lp the CPLEX problem
 * @param instance the problem instance
 * @param map a map containing the CPLEX's indices of the variables
 * @return a VarVal object that contains the values fetched from CPLEX
 */
VarVal fetchVariables(CEnv env, Prob lp, Instance3BKP instance, mapVar map);

/**
 * Print a human readable output in the file named output.txt
 * @param env
 * @param lp
 * @param instance
 * @param map
 */
void output(CEnv env, Prob lp, Instance3BKP instance, 
			double objval, VarVal v, char * filename, optionFlag oFlag
);


#endif /* MASTER_PROBLEM */




