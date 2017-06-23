/*!
 * @author Mirko Bez
 * @file MasterProblem.h
 * 
 */
#ifndef __SLAVE__PROBLEM__H__
#define __SLAVE__PROBLEM__H__

#include "../Utils/fetch_option.h"
#include "../Utils/Instance3BKP.h"
#include "../Utils/cpxmacro.h"
#include "utils.h"

/**
 * setup slave problem
 * @param env
 * @param lp 
 * @param instance an instance of 3KP
 * @param map contains the index position of the variables in the problem
 * @param fetched_variables contains the values of the variables of the solution of the master problem 
 */
void setupSP(CEnv env, Prob lp, Instance3BKP instance, mapVar map, VarVal fetched_variables, optionFlag oFlag);

#endif /* SLAVE_PROBLEM */





