/*!
 * @author Mirko Bez
 * @file MasterProblem.h
 * 
 */
#ifndef __MASTER__PROBLEM__H__

#include "../Utils/fetch_option.h"
#include "../Utils/Instance3BKP.h"
#include "../Utils/cpxmacro.h"
#include "utils.h"

/**
 * set up the model for CPLEX
 * @param env the cplex enviroment
 * @param lp the cplex problem
 * @param instance the 3(B)KP instance of the problem
 * @param oFlag an object containing the configuration information
 * @return an object containing the indixes of the variables inside the CPLEX problem 
 */	
mapVar setupLP(CEnv env, Prob lp, Instance3BKP instance, optionFlag oFlag);

#endif /* MASTER_PROBLEM */




