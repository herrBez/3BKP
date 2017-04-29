/**
 * @author Mirko Bez
 * @file main.cpp
 * 
 * Usage: ./main <filename.dat>
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include "../Utils/cpxmacro.h"
#include <getopt.h>
#include <unistd.h>
#include "../Utils/Instance3BKP.h"
#include <assert.h>     /* assert */
#include "utils.h"
#include "MasterProblem.h"
#include <algorithm>




char OUTPUTFILENAME[128];


using namespace std;

int status;
char errmsg[BUF_SIZE];		








/**
 * solve the model write the result in a file and returns the return value of the function
 * @param env the CPLEX environment
 * @param lp the CPLEX problem
 * @param N the number of nodes of the TSP
 * @param mapY a map containing the (CPLEX) index of the y variables of the problem
 * used in order to retrieve the values of the y variables and print them on the screen.
 * @return objval the optimal solution
 */
double solve( CEnv env, Prob lp, Instance3BKP instance, optionFlag oFlag) {
	clock_t t1,t2;
    t1 = clock();
    struct timeval  tv1, tv2;
    gettimeofday(&tv1, NULL);
	CHECKED_CPX_CALL( CPXmipopt, env, lp );
	t2 = clock();
    gettimeofday(&tv2, NULL);
	double objval = 0.0;
	CHECKED_CPX_CALL( CPXgetobjval, env, lp, &objval );
	
	double user_time = (double)(tv2.tv_sec+tv2.tv_usec*1e-6 - (tv1.tv_sec+tv1.tv_usec*1e-6));
	double cpu_time = (double)(t2-t1) / CLOCKS_PER_SEC;
	
	
	if(oFlag.output_required){	
		CHECKED_CPX_CALL( CPXsolwrite, env, lp, "/tmp/Model.sol");
	}
	
	printf("User time:\t %.4lf seconds ~ %.6lf minutes\n", user_time, (user_time/60));
	printf("CPU time:\t %.4lf seconds ~ %.6lf minutes \n", cpu_time, (cpu_time/60));
	printf("On average %.4lf (virtual) CPUs were used\n", cpu_time/user_time);
	if(oFlag.threads != 0){
		printf("(%d threads)", oFlag.threads);
	}
	cout << "Objval: " << objval << endl;
	
	return objval;
}



VarVal fetchVariables(CEnv env, Prob lp, Instance3BKP instance, mapVar map){
	
	int N = instance.N;
	int K = instance.K;
	
	VarVal variable_values(K, N, 6);
	
	{
		vector<double> tmp(K);
		int begin = map.Z[0];
		int end = map.Z[K-1];
		CHECKED_CPX_CALL (CPXgetx, env, lp, &tmp[0], begin, end);
		std::transform(tmp.begin(), tmp.end(), variable_values.z.begin(), [](double val){ return val < 0.5? 0:1; });
	}
	
	
	for(int k = 0; k < K; k++){
		int begin = map.Sigma[k][0];
		int end = map.Sigma[k][2];
		CHECKED_CPX_CALL( CPXgetx, env, lp, &variable_values.sigma[k][0], begin, end);
	}
	
	
	for(int k = 0; k < K; k++){
		vector<double> tmp(N);
		int begin = map.T[k][0];
		int end = map.T[k][N - 1];
		CHECKED_CPX_CALL( CPXgetx, env, lp, &tmp[0], begin, end);
		std::transform(tmp.begin(), tmp.end(), variable_values.t[k].begin(), [](double val){ return val < 0.5? 0:1; });
	}
	
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			int begin = map.Chi[k][i][0];
			int end = map.Chi[k][i][2];
			CHECKED_CPX_CALL( CPXgetx, env, lp, &variable_values.chi[k][i][0], begin, end);
		}
	}
	
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int j = 0; j < N; j++){
				vector<double> tmp(3);
				int begin = map.B[k][i][j][0];
				int end = map.B[k][i][j][2];
				CHECKED_CPX_CALL( CPXgetx, env, lp, &tmp[0], begin, end);
				std::transform(tmp.begin(), tmp.end(), variable_values.b[k][i][j].begin(), [](double val){ return val < 0.5? 0:1; });
			}
		}
	}
	
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			vector<double> tmp(6);
			int begin = map.Rho[k][i][0];
			int end = map.Rho[k][i][5];
			CHECKED_CPX_CALL( CPXgetx, env, lp, &tmp[0], begin, end);
			std::transform(tmp.begin(), tmp.end(), variable_values.rho[k][i].begin(), [](double val){ return val < 0.5? 0:1; });
		}
	}
	
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int r = 0; r < 6; r++){
				if(variable_values.rho[k][i][r] >= 0.9) { //Taking into account round errors due to the double representation
					for(int delta = 0; delta < 3; delta++){
						variable_values.rotation[i][delta] = instance.R[r][delta];
					}
					break;
				}
			}
		}
	}
	return variable_values;
}

/**
 * Print a human readable output in the file named output.txt
 * @param env
 * @param lp
 * @param instance
 * @param map
 */
 
void output(CEnv env, Prob lp, Instance3BKP instance, double objval, VarVal v, char * filename, optionFlag oFlag){
	int N = instance.N;
	int K = instance.K;
	
	
	
	ofstream outfile(filename);
	if(!outfile.good()) {
		cerr << filename << endl;
		throw std::runtime_error("Cannot open the file ");
	}
	outfile << "#Valore della funzione obiettivo " << objval << endl; 
	outfile << "#Vengono inclusi solo gli oggetti messi nello zaino" << endl;
	for(int k = 0; k < K; k++){
		if(v.z[k] == 1) {
			outfile << "#Dimensione max. del " << k << "-mo Zaino " << instance.S[k][0] << " " << instance.S[k][1] <<" "<< instance.S[k][2] << endl;
		}
	} 
	
	for(int k = 0; k < K; k++){
		if(v.z[k] == 0)
			continue;
		outfile << "#Knapsack " << k << "-mo" << endl;
		outfile << v.sigma[k][0] << " " << v.sigma[k][1] << " " << v.sigma[k][2] << endl;
		if(oFlag.extended){
			outfile << "L " << instance.L[k][0] << " " << instance.L[k][1] << " " << instance.L[k][2] << endl;
			outfile << "U " << instance.U[k][0] << " " << instance.U[k][1] << " " << instance.U[k][2] << endl;
		}
	
		for(int i = 0; i < N; i++){
			if(v.t[k][i] == 0)
				continue;
			//Print only inserted objects 
			outfile << i << "\t";
			
			for(int delta = 0; delta < 3; delta++){
				outfile << v.chi[k][i][delta];
				outfile << " ";
			}
			outfile << "\t";
			for(int delta = 0; delta < 3; delta++){
				outfile << instance.s[i][v.rotation[i][delta]] << " ";
			}
			
			outfile << endl;
		}
			
	}
	
	outfile.close();
}

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
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int r = 0; r < 6; r++){
				char bound = 'B';
				double value = fetched_variables.rho[k][i][r];
				CHECKED_CPX_CALL(CPXchgbds, env, lp, 1, &map.Rho[k][i][r], &bound, &value);
			}
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

/**
 * the main function
 * @param argc
 * @param argv
 * @return EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 */
int main (int argc, char *argv[])
{
	bool exc_arised = false;	
	try { 

		optionFlag oFlag = get_option(argc, argv);	
		Instance3BKP instance(oFlag.filename, oFlag.extended);
		DECL_ENV( env );
		DECL_PROB( env, lp );
		
		mapVar map = setupLP(env, lp, instance, oFlag);
		
		/* Set up timeout */
		CHECKED_CPX_CALL(CPXsetdblparam, env,  CPX_PARAM_TILIM, oFlag.timeout);
		CHECKED_CPX_CALL(CPXsetintparam, env,  CPX_PARAM_THREADS, oFlag.threads);
		
		// find the solution
		double objvalMP = solve(env, lp, instance, oFlag);
		
		printf("Solved Main Problem\n");
		
		
		// fetch variables from the solved model
		VarVal fetched_variables = fetchVariables(env, lp, instance, map);
		
		printf("Fetched variables Successfully\n");
		
		//sprintf(OUTPUTFILENAME, "output_%s%c", oFlag.output_filename, '\0');
		output(env, lp, instance, objvalMP, fetched_variables,  oFlag.output_filename, oFlag);
		
		
		printf("Written solution for Main Problem in %s\n", oFlag.output_filename);
		
		//We want to optimize at least in one direction
		if(oFlag.optimize[0] || oFlag.optimize[1] || oFlag.optimize[2]){
			double start_chi_value = 0.0;
			double end_chi_value = 0.0;
			
			
			for(int k = 0; k < instance.K; k++){
				for(int i = 0; i < instance.N; i++){
					
						for(int delta = 0; delta < 3; delta++){
							if(oFlag.optimize[delta])
							start_chi_value += fetched_variables.chi[k][i][delta];
						}
					
				}
			}
			cout << "start chi values " << start_chi_value << endl;
			
			
			// Setup Slave Problem
			setupSP(env, lp, instance, map, fetched_variables, oFlag);
			
			printf("Set up problem Successfully\n");
			
			
			CHECKED_CPX_CALL( CPXwriteprob, env, lp, "/tmp/Model2.lp", NULL ); 
			CHECKED_CPX_CALL(CPXchgprobtype, env, lp, CPXPROB_MILP);
			
			
			// Solve Slave Problem
			double objvalSP = solve(env, lp, instance, oFlag); 
			
			
			
			VarVal slave_variables = fetchVariables(env, lp, instance, map);
			
		
			
			
			
			end_chi_value = 0;
			for(int k = 0; k < instance.K; k++){
				for(int i = 0; i < instance.N; i++){
					
						for(int delta = 0; delta < 3; delta++){
							if(oFlag.optimize[delta])
							end_chi_value += slave_variables.chi[k][i][delta];
						}
					
				}
			}
			cout << "end chi values " << end_chi_value << endl;
			
			sprintf(OUTPUTFILENAME, "%s2", oFlag.output_filename);

			output(env, lp, instance, objvalSP,  slave_variables,  OUTPUTFILENAME, oFlag);
			printf("Written solution for Extra Problem in %s\n", OUTPUTFILENAME);
			
		}
		// free-allocated resources
		CPXfreeprob(env, &lp);
		CPXcloseCPLEX(&env);
	} catch (exception& e){
		cout << ">>Exception in processing " << argv[1] << ": " << e.what() << endl;
		exc_arised = true;
	}
	return exc_arised?EXIT_FAILURE:EXIT_SUCCESS;
}
