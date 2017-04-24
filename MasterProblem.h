#ifndef __MASTER__PROBLEM__H__

#include "utils.h"
#include "Instance3BKP.h"
#include "cpxmacro.h"

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




