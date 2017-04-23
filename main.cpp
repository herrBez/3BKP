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
#include "cpxmacro.h"
#include <getopt.h>
#include <unistd.h>
#include "Instance3BKP.h"
#include <assert.h>     /* assert */
#include "utils.h"
#include <algorithm>




char OUTPUTFILENAME[128];


using namespace std;

// error status and messagge buffer
int status;
char errmsg[BUF_SIZE];		
const int NAME_SIZE = 512;
char name[NAME_SIZE];




/**
 * set up the variables
 * @param env
 * @param lp
 * @param instance
 * @param map the map that is filled up with the indexes
 * 
 */
void setupLPVariables(CEnv env, Prob lp, Instance3BKP instance, mapVar &map, optionFlag oFlag){
	int current_var_position = 0;
	int N = instance.N;
	int K = instance.K;
	
	/*
	 * Adding z variables
	 * z_k = 1 if the k-th knapsack is used, 0 otherwise
	 */
	 for(int k = 0; k < K; k++){
		char xtype = 'B';
		double obj = oFlag.optimizeKnapsackCost?-instance.fixedCost[k]:0.0;
		double lb = 0.0;
		double ub = 1.0;
		sprintf(name, "z %d", k);
		char* xname = (char*)(&name[0]);
		CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &xtype, &xname );
		map.Z[k] = current_var_position++;
	 }
	
	/*
	 * 
	 * adding \chi_ki^\delta variables
	 * the coordinate of the bottom-left-back point of item i along dimension \delta in knapsack k
	 * + constraint(17)
	 * 
	 */
	 for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int delta = 0; delta < 3; delta++){
				char xtype = 'C';
				double obj = 0.0;
				double lb = 0.0;
				double ub = CPX_INFBOUND;
				snprintf(name, NAME_SIZE, "chi %d %d %d", i, k, delta);
				char * xname = (char*)(&name[0]);
				CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &xtype, &xname );
				map.Chi[k][i][delta] = current_var_position++;
			}
		}
	 }

	/* 
	 * adding t variables
	 * t_kj := binary value {1 if j-th item is loaded in the KP 0 otherwise} 
	 * + constraint(18)
	 */ 
	 for(int k = 0; k < K; k++){
		for(int j = 0; j < N; j++){
		
			char xtype = 'B';
			double obj = instance.profit[j]; 
			double lb = 0.0;
			double ub = 1.0;
			sprintf(name, "t_%d %d", k, j);
			char* xname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &xtype, &xname );
			map.T[k][j] = current_var_position++;
		}
	}
	/* 
	 * adding b_kij^\delta variables
	 * b_{ijk}^\delta 1 if item i comes before item j in k-th knapsack in dimension \delta.
	 * + constraint(19)
	 */ 
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int j = 0; j < N; j++){
				for(int delta = 0; delta < 3; delta++){
					char xtype = 'B';
					double obj = 0;
					double lb = 0.0;
					double ub = 1.0;
					snprintf(name, NAME_SIZE, "b %d %d %d %d", k, i, j, delta);
					char* xname = (char*)(&name[0]);
					CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &xtype, &xname );
					map.B[k][i][j][delta] = current_var_position++;
				}
			}
		}
	}
	
	/* 
	 * adding rho_{ir} variables
	 * rho_{ir} {1 if item i is rotated with rotation r, 0 otherwise}.
	 * + constraint 20
	 */ 
	for(int i = 0; i < N; i++){
		for(int r = 0; r < 6; r++){
			char xtype = 'B';
			double obj = 0.0;
			double lb = 0.0;
			double ub = 1.0;
			snprintf(name, NAME_SIZE, "rho %d %d", i, r);
			char* xname = (char*)(&name[0]);
			CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &xtype, &xname );
			map.Rho[i][r] = current_var_position++;
		}
	}
	
	 cout << "Number of variables: " << current_var_position << endl;
}


/**
 * Set up the constraints 
 * @param env
 * @param lp
 * @param instance
 * @param map
 * 
 */
void setupLPConstraints(CEnv env, Prob lp, Instance3BKP instance, mapVar map, optionFlag oFlag){
	int N = instance.N;
	int K = instance.K;
	int R = 6; //Cardinality of the set R
	//int DELTA = 3; //Cardinality of the set \Delta
	int M = 1e6; //Used in constraint 9-10 
	
	unsigned int number_of_constraint = 0;
	
	
	//Constraint (6): sum_{j \in J} w_j*d_j*h_j*t_kj <= W_k D_k H_k
	for(int k = 0; k < K; k++){
		vector<double> coef(N);
		for(int j = 0; j < N; j++){
			coef[j] = instance.getVolume(j);
		}
		char sense = 'L';
		int matbeg = 0;
		double rhs = instance.getBoxVolume(k);
		snprintf(name, NAME_SIZE, "(6)");
		char* cname = (char*)(&name[0]);
		CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, map.T[k].size(), &rhs, &sense, &matbeg, &map.T[k][0], &coef[0], 0, &cname);
		number_of_constraint++;
	}
	
	
	//Constraint (7) : (\sum_{\delta \in \Delta} b_kij^\delta + b_kij^\delta) -t_ki -t_kj >= -1
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int j = 0; j < N; j++){
			
				if(i >= j)
					continue;
					
				vector < int > idVar(8);
				vector < double > coeff(8);
				unsigned int index = 0;
				for(int delta = 0; delta < 3; delta++){
					idVar[index] = map.B[k][i][j][delta];
					coeff[index] = +1.0;
					index++;
					idVar[index] = map.B[k][j][i][delta];
					coeff[index] = +1.0;
					index++;
				}
				idVar[index] = map.T[k][i];
				coeff[index] = -1.0;
				index++;
				idVar[index] = map.T[k][j];
				coeff[index] = -1.0;
				index++;
				char sense = 'G';
				int matbeg = 0;
				double rhs = -1.0;
				snprintf(name, NAME_SIZE, "(7) %d %d", i, j);
				char* cname = (char*)(&name[0]);
				
				CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coeff[0], 0, &cname);
				/* Checking that the vector is correctly initialized */
				assert(index == idVar.size());
				number_of_constraint++;
			}
		}
	}
		
	/* (8) */
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int delta = 0; delta < 3; delta++){
				vector< int > idVar(7);
				vector< double > coeff(7);
				unsigned int index = 0;
				idVar[index] = map.Chi[k][i][delta];
				coeff[index] = 1.0;
				index++;
				for(int r = 0; r < R; r++){
					idVar[index] = map.Rho[i][r];
					coeff[index] = instance.s[i][instance.R[r][delta]];
					index++;
				}
				char sense = 'L';
				int matbeg = 0;
				double rhs = instance.S[k][delta];
				snprintf(name, NAME_SIZE, "(8) %d %d",i,delta);
				char* cname = (char*)(&name[0]);
				CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coeff[0], 0, &cname);
				/* Checking that the vector is correctly initialized */
				assert(index == idVar.size());
				number_of_constraint++;
			}
		}
	}
	
	/* (9) */
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int j = 0; j < N; j++){
				if(i >= j)
					continue;
				for(int delta = 0; delta < 3; delta++){
					vector< int > idVar(9);
					vector< double > coeff(9);
					unsigned int index = 0;
					idVar[index] = map.Chi[k][i][delta];
					coeff[index] = 1.0;
					index++;
					for(int r = 0; r < R; r++){
						idVar[index] = map.Rho[i][r];
						coeff[index] = instance.s[i][instance.R[r][delta]];
						index++;
					}
					idVar[index] = map.Chi[k][j][delta];
					coeff[index] = -1.0;
					index++;
					idVar[index] = map.B[k][i][j][delta];
					coeff[index] = M;
					index++;
					char sense = 'L';
					int matbeg = 0;
					double rhs = M;
					snprintf(name, NAME_SIZE, "(9) %d %d %d %d", k, i, j, delta);
					char* cname = (char*)(&name[0]);
					CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coeff[0], 0, &cname);
				
					/* Checking that the vector is correctly initialized */
					assert(index == idVar.size());
					number_of_constraint++;
				}
			}
		}
	}
	
	
	/* (10) */
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int j = 0; j < N; j++){
				if(i >= j)
					continue;
				for(int delta = 0; delta < 3; delta++){
					vector< int > idVar(9);
					vector< double > coeff(9);
					
					unsigned int index = 0;
					idVar[index] = map.Chi[k][j][delta];
					coeff[index] = 1.0;
					index++;
					for(int r = 0; r < R; r++){
						/* Possible typo in the paper */
						//idVar[index] = map.Rho[i][r];
						idVar[index] = map.Rho[j][r];
						coeff[index] = instance.s[j][instance.R[r][delta]];
						index++;
					}
					idVar[index] = map.Chi[k][i][delta];
					coeff[index] = -1.0;
					index++;
					idVar[index] = map.B[k][j][i][delta];
					coeff[index] = M;
					index++;
					char sense = 'L';
					int matbeg = 0;
					double rhs = M;
					snprintf(name, NAME_SIZE, "(10) %d %d %d %d", k, i, j, delta);
					char* cname = (char*)(&name[0]);
					CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coeff[0], 0, &cname);
				
					/* Checking that the vector is correctly initialized */
					assert(index == idVar.size());
					number_of_constraint++;
				}
			}
		}
	}
		
		
	//Constraint (11) 
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int delta = 0; delta < 3; delta++){
				vector< int > idVar(2);
				vector< double > coeff(2);
				idVar[0] = map.Chi[k][i][delta];
				coeff[0] = 1.0;
				idVar[1] = map.T[k][i];
				coeff[1] = -instance.L;
				char sense = 'L';
				int matbeg = 0;
				double rhs = 0;
				snprintf(name, NAME_SIZE, "(11) %d %d",i,delta);
				char* cname = (char*)(&name[0]);
				CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coeff[0], 0, &cname);
				number_of_constraint++;
			}
		}
	}
	//Constraint (12): b_{kij}^\delta <= t_i ==> b_{ij}^\delta - t_i = 0
	
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int j = 0;  j < N; j++){
				for(int delta = 0; delta < 3; delta++){
					vector <int> idVar(2);
					vector <double> coef(2);
					idVar[0] = map.B[k][i][j][delta];
					coef[0] = 1.0;
					idVar[1] = map.T[k][i];
					coef[1] = -1.0;
					char sense = 'L';
					int matbeg = 0;
					double rhs = 0;
					snprintf(name, NAME_SIZE, "(12) %d %d %d", i, j, delta);
					char * cname = (char*) (&name[0]);
					CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coef[0], 0, &cname);
					number_of_constraint++;
				}
			}
		}
	}
	
	
	//Constraint (13): b_{ji}^\delta <= t_j ==> b_{ji}^\delta - t_j = 0
	/*for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int j = 0;  j < N; j++){
				for(int delta = 0; delta < 3; delta++){
					vector <int> idVar(2);
					vector <double> coeff(2);
					idVar[0] = map.B[k][j][i][delta];
					coeff[0] = 1.0;
					idVar[1] = map.T[k][j];
					coeff[1] = -1.0;
					char sense = 'L';
					int matbeg = 0;
					double rhs = 0;
					snprintf(name, NAME_SIZE, "(13) %d %d %d", i,j,delta);
					char * cname = (char*) (&name[0]);
					CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coeff[0], 0, &cname);
					number_of_constraint++;
				}
			}
		}
	}*/
	
	
	/* Constraint (16) */
	for(int i = 0; i < N; i++){
		vector < int > idVar(R + K);
		vector < double > coeff(R + K);
		
		for(int r = 0; r < R; r++){
			idVar[r] = map.Rho[i][r];
			coeff[r] = 1.0;
		}
		for(int k = 0; k < K; k++){
			idVar[R+k] = map.T[k][i];
			coeff[R+k] = -1.0;
		}
		double rhs = 0.0;
		char sense = 'E';
		int matbeg = 0;
		
		snprintf(name, NAME_SIZE, "(16) %d", i);
		char * cname = (char*) (&name[0]);
		CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coeff[0], 0, &cname);
		number_of_constraint++;
	}
	
	
	/* Constraint (Multi) */
	for(int k = 0; k < K; k++){
		for(int j = 0; j < N; j++){
			
			vector < int > idVar(2);
			vector < double > coeff(2);
			idVar[0] = map.T[k][j];
			coeff[0] = 1.0;
			idVar[1] = map.Z[k];
			coeff[1] = -1.0;
			
			double rhs = 0.0;
			char sense = 'L';
			int matbeg = 0;
			
			snprintf(name, NAME_SIZE, "Multi %d %d",k, j);
			char * cname = (char*) (&name[0]);
			CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coeff[0], 0, &cname);
			number_of_constraint++;
		}
	}
	
	/* */
	for(int j = 0; j < N; j++){
		vector < int > idVar(K);
		vector < double > coeff(K);
		for(int k = 0; k < K; k++){
			idVar[k] = map.T[k][j];
			coeff[k] = 1.0;
		}
		double rhs = 1.0;
		char sense = 'L';
		int matbeg = 0;
		
		snprintf(name, NAME_SIZE, "Multi (2) %d ", j);
		char * cname = (char*) (&name[0]);
		CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coeff[0], 0, &cname);
		number_of_constraint++;
	}
	printf("Number of constraints (not considering the variable bounds): %d\n", number_of_constraint);
}


/**
 * set up the model for CPLEX
 * @param env the cplex enviroment
 * @param lp the cplex problem
 * @param N the number of nodes of the TSP
 * @param C the matrix containing the costs from i to j
 * @param mapY used in order to have the result outside the setup function.
 */	
mapVar setupLP(CEnv env, Prob lp, Instance3BKP instance, optionFlag oFlag)
{	
	
	
	int N = instance.N;
	int K = instance.K;
	int R = 6;
	mapVar map(K, N, R);

	/* We deal with a maximization problem */
	CHECKED_CPX_CALL( CPXchgobjsen, env, lp, CPX_MAX); 
	/* Set up the variables */
	setupLPVariables(env, lp, instance, map, oFlag);
	/* Set up the constraints */
	setupLPConstraints(env, lp, instance, map, oFlag);
	
	
	if(oFlag.output_required){
		CHECKED_CPX_CALL( CPXwriteprob, env, lp, "/tmp/Model.lp", NULL ); 
	}
	
	return map;
}

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
	int N = instance.N;
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
	
	cout << "Problem Size N" << N << endl;
	cout << "in " << user_time << " seconds (user time)\n";
	cout << "in " << cpu_time << " seconds (CPU time)\n";
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
	
	for(int i = 0; i < N; i++){
		vector<double> tmp(6);
		int begin = map.Rho[i][0];
		int end = map.Rho[i][5];
		CHECKED_CPX_CALL( CPXgetx, env, lp, &tmp[0], begin, end);
		std::transform(tmp.begin(), tmp.end(), variable_values.rho[i].begin(), [](double val){ return val < 0.5? 0:1; });
	}
	
	for(int i = 0; i < N; i++){
		for(int r = 0; r < 6; r++){
			if(variable_values.rho[i][r] >= 0.9) { //Taking into account round errors due to the double representation
				for(int delta = 0; delta < 3; delta++){
					variable_values.rotation[i][delta] = instance.R[r][delta];
				}
				break;
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
 
void output(CEnv env, Prob lp, Instance3BKP instance, VarVal v, char * filename){
	int N = instance.N;
	int K = instance.K;
	
	
	
	ofstream outfile(filename);
	if(!outfile.good()) {
		cerr << filename << endl;
		throw std::runtime_error("Cannot open the file ");
	}
	
	outfile << "#Vengono inclusi solo gli oggetti messi nello zaino" << endl;
	for(int k = 0; k < K; k++){
		if(v.z[k] == 1)
			outfile << "#Dimensione del " << k << "-mo Zaino " << instance.S[k][0] << " " << instance.S[k][1] <<" "<< instance.S[k][2] << endl;
	} 
	
	for(int k = 0; k < K; k++){
		if(v.z[k] == 0)
			continue;
		outfile << "#Knapsack " << k << "-mo" << endl;
		outfile << instance.S[k][0] << " " << instance.S[k][1] << " " << instance.S[k][2] << endl;
	
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
		CHECKED_CPX_CALL(CPXsetintparam, env,  CPX_PARAM_THREADS, 2);
		
		// find the solution
		solve(env, lp, instance, oFlag);
		
		printf("Solved Master Problem\n");
		
		
		// fetch variables from the solved model
		VarVal fetched_variables = fetchVariables(env, lp, instance, map);
		
		printf("Fetched variables Successfully\n");
		
		sprintf(OUTPUTFILENAME, "output_%s%c", oFlag.filename, '\0');
		output(env, lp, instance, fetched_variables,  OUTPUTFILENAME);
		
		//We want to optimize at least in one direction
		if(oFlag.optimize[0] || oFlag.optimize[1] || oFlag.optimize[2]){
		
			{
			double sum = 0.;
			for(int k = 0; k < instance.K; k++){
				for(int i = 0; i < instance.N; i++){
					
						for(int delta = 0; delta < 3; delta++){
							if(oFlag.optimize[delta])
							sum += fetched_variables.chi[k][i][delta];
						}
					
				}
			}
			cout << "START CHI VALUES " << sum << endl;
			}
			
			// Setup Slave Problem
			setupSP(env, lp, instance, map, fetched_variables, oFlag);
			
			printf("Set up problem Successfully\n");
			
			
			CHECKED_CPX_CALL( CPXwriteprob, env, lp, "/tmp/Model2.lp", NULL ); 
			CHECKED_CPX_CALL(CPXchgprobtype, env, lp, CPXPROB_MILP);
			
			
			// Solve Slave Problem
			solve(env, lp, instance, oFlag); 
			
			
			
			VarVal slave_variables = fetchVariables(env, lp, instance, map);
			
		
			cout << "OK -> OUTPUT" << endl;
			sprintf(OUTPUTFILENAME, "output_%s2%c", oFlag.filename, '\0');
			
			
			{
			double sum = 0.;
			for(int k = 0; k < instance.K; k++){
				for(int i = 0; i < instance.N; i++){
					
						for(int delta = 0; delta < 3; delta++){
							if(oFlag.optimize[delta])
							sum += slave_variables.chi[k][i][delta];
						}
					
				}
			}
			cout << "END CHI VALUES " << sum << endl;
			}
			
			output(env, lp, instance, slave_variables,  OUTPUTFILENAME);
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
