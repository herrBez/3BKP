
#include "SlaveProblem.h"

/**
 * setup slave problem
 * @param env
 * @param lp 
 * @param instance an instance of 3KP
 * @param map contains the index position of the variables in the problem
 * @param fetched_variables contains the values of the variables of the solution of the master problem 
 */
void setupSP(CEnv env, Prob lp, Instance3BKP instance, mapVar map, VarVal fetched_variables, optionFlag oFlag){
	int N = instance.N;
	int K = instance.K;
	//fix z variables with the known values
	for(int k = 0; k < K; k++){
		char bound = 'B'; //Set upper and lower bound
		double value = fetched_variables.z[k];
		CHECKED_CPX_CALL(CPXchgbds, env, lp, 1, &map.Z[k], &bound, &value);
	}
	//fix t variables with the known values
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			char bound = 'B';
			double value = fetched_variables.t[k][i];
			CHECKED_CPX_CALL(CPXchgbds, env, lp, 1, &map.T[k][i], &bound, &value);
		}
	}
	//fix b variables that are not used
	for(int k = 0; k < K; k++){
		
			for(int i = 0; i < N; i++){
				if(fetched_variables.t[k][i] == 0){
				for(int j = 0; j < N; j++){
					for(int delta = 0; delta < 3; delta++){
					char bound = 'B';
					double value = 0.0;
					CHECKED_CPX_CALL(CPXchgbds, env, lp, 1, &map.B[k][i][j][delta], &bound, &value);
					}
				}
			
			}	
		}
	}
	
	//fix sigma variables
	for(int k = 0; k < K; k++){
		for(int delta = 0; delta < 3; delta++){
			char bound = 'B';
			double value = fetched_variables.sigma[k][delta];
			CHECKED_CPX_CALL(CPXchgbds, env, lp, 1, &map.Sigma[k][delta], &bound, &value);
		}
	}
	
	
	
	
	//fix rho variabes with known values
	for(int i = 0; i < N; i++){
		for(int r = 0; r < 6; r++){
			char bound = 'B';
			double value = fetched_variables.rho[i][r];
			CHECKED_CPX_CALL(CPXchgbds, env, lp, 1, &map.Rho[i][r], &bound, &value);
				}
	}

		
	//fix chi variables that are not used (s.t. t[k][i] == 0)
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			if(fetched_variables.t[k][i] == 0){
				char bound[3] = {'B', 'B', 'B'};
				double value[3] = {0.0, 0.0,0.0};
				CHECKED_CPX_CALL(CPXchgbds, env, lp, 3, &map.Chi[k][i][0], &bound[0], &value[0]);
			}
		}
	}
	
	/* Change objective function */
	
	//Change objective coefficient of variabe z_k
	for(int k = 0; k < K; k++){
		double coeff = 0.0;
		CHECKED_CPX_CALL(CPXprechgobj, env, lp, 1, &map.Z[k], &coeff);
	}
	
	//Change objective coefficient of variabe t_k
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			double coeff = 0.0;
			CHECKED_CPX_CALL(CPXprechgobj, env, lp, 1, &map.T[k][i], &coeff);
		}
	}
	//Change objective coefficients of variable chi_ki only if t_ki
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int delta = 0; delta < 3; delta++){
				double coeff = 0.0;
				if(oFlag.optimize[delta] && fetched_variables.t[k][i] == 1) //Optimize only with respect to objects that are included
					coeff = 1.0;
				
				CHECKED_CPX_CALL(CPXprechgobj, env, lp, 1, &map.Chi[k][i][delta], &coeff);
			}
		}
	}
	
	/* Set problem to minimum */
	CHECKED_CPX_CALL( CPXchgobjsen, env, lp, CPX_MIN); 
	
	CPXchgprobname (env, lp, "slave problem");
	
	
	
}	


