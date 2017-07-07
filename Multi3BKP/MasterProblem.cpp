#include "MasterProblem.h"
using namespace std;

/**
 * set up balancing constraints (this function is called only if the flag
 * extended is given, i.e. if the option -e is used)
 * @param env the CPLEX environment
 * @param lp the CPLEX problem
 * @param instance the problem instance
 * @param map an object containing the indexes used by CPLEX to represent the 
 * variables
 */
void setupLPBalancingConstraints(CEnv env, Prob lp, Instance3BKP instance, mapVar map){
	printf("BALANCING CONSTRAINTS\n");
	const int NAME_SIZE = 512;
	char name[NAME_SIZE];
	int N = instance.N;
	int K = instance.K;
	/* (59) */ 
	for(int k = 0; k < K; k++){
		for(int delta = 0; delta < 3; delta++){
			vector< int > idVar(N + N*6 + N);
			vector< double > coeff(N + N*6 + N);
			int index = 0;
			for(int i = 0; i < N; i++){
				idVar[index] = map.Chi[k][i][delta];
				coeff[index] = instance.mass[i];
				index++;
			} 
			for(int i = 0; i < N; i++){
				for(int r = 0; r < 6; r++){
					idVar[index] = map.Rho[i][r];
					coeff[index] = instance.mass[i]*(instance.s[i][instance.R[r][delta]]/2.0);
					index++;
				}
			}
			for(int i = 0; i < N; i++){
				idVar[index] = map.T[k][i];
				coeff[index] = -instance.L[k][delta]*instance.mass[i];  
				index++;
			}
			double rhs = 0.0;
			char sense = 'G';
			int matbeg = 0;
			
			snprintf(name, NAME_SIZE, "(14) %d %d", k, delta);
			char * cname = (char*) (&name[0]);
			CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coeff[0], 0, &cname);
		}
	}
		
	/* (60) */
	for(int k = 0; k < K; k++){
		for(int delta = 0; delta < 3; delta++){
			vector< int > idVar(N + N*6 + N);
			vector< double > coeff(N + N*6 + N);
			int index = 0;
			for(int i = 0; i < N; i++){
				idVar[index] = map.Chi[k][i][delta];
				coeff[index] = instance.mass[i];
				index++;
			} 
			for(int i = 0; i < N; i++){
				for(int r = 0; r < 6; r++){
					idVar[index] = map.Rho[i][r];
					coeff[index] = instance.mass[i]*(instance.s[i][instance.R[r][delta]]/2.0);
					index++;
				}
			}
			for(int i = 0; i < N; i++){
				idVar[index] = map.T[k][i];
				coeff[index] = -instance.U[k][delta]*instance.mass[i];
				index++;
			}
			snprintf(name, NAME_SIZE, "(15) %d %d", k, delta);
			double rhs = 0.0;
			char sense = 'L';
			int matbeg = 0;
			
			char * cname = (char*) (&name[0]);
			CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coeff[0], 0, &cname);
		}
	}
}



/**
 * set up the variables
 * @param env the CPLEX environment
 * @param lp the CPLEX problem
 * @param instance the problem instance
 * @param map the map that is filled up with the indexes
 * @param oFlag an object containing the configuration information of this program
 * instance
 */
void setupLPVariables(CEnv env, Prob lp, Instance3BKP instance, mapVar &map, optionFlag oFlag){
	const int NAME_SIZE = 512;
	char name[NAME_SIZE];
	int current_var_position = 0;
	int N = instance.N;
	int K = instance.K;
	
	/*
	 * Adding z variables
	 * z_k = 1 if the k-th knapsack is used, 0 otherwise
	 * + Constraint (37)
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
	 * + constraint(33)
	 * 
	 */
	 for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int delta = 0; delta < 3; delta++){
				char xtype = 'C';
				double obj = 0.0;
				double lb = 0.0;
				double ub = CPX_INFBOUND;
				snprintf(name, NAME_SIZE, "chi %d %d %d", k, i, delta);
				char * xname = (char*)(&name[0]);
				CHECKED_CPX_CALL( CPXnewcols, env, lp, 1, &obj, &lb, &ub, &xtype, &xname );
				map.Chi[k][i][delta] = current_var_position++;
			}
		}
	 }

	/* 
	 * adding t variables
	 * t_kj := binary value {1 if j-th item is loaded in the KP 0 otherwise} 
	 * + constraint(34)
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
	 * + constraint(35)
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
	 * + constraint (36)
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
	if(!oFlag.benchmark)
		cout << "Number of variables: " << current_var_position << endl;
}



/**
 * Set up the constraints 
 * @param env the CPLEX environment
 * @param lp the CPLEX problem
 * @param instance the problem instance
 * @param map the map initialized by the setupLPVariables function
 * 
 */
void setupLPConstraints(CEnv env, Prob lp, Instance3BKP instance, mapVar map, optionFlag oFlag){
	
	const int NAME_SIZE = 512;
	char name[NAME_SIZE];
	
	int N = instance.N;
	int K = instance.K;
	int R = 6; //Cardinality of the set R
	//int DELTA = 3; //Cardinality of the set \Delta
	double M = instance.E; //Used in constraint 9-10 
	
	unsigned int number_of_constraint = 0;
	
	
	//Constraint (22): sum_{j \in J} w_j*d_j*h_j*t_kj <= W_k D_k H_k
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
	
	
	//Constraint (23) : (\sum_{\delta \in \Delta} b_kij^\delta + b_kij^\delta) -t_ki -t_kj >= -1
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
		
	/* (24) */
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int delta = 0; delta < 3; delta++){
				vector< int > idVar(1 + R + 1);
				vector< double > coeff(1 + R + 1);
				unsigned int index = 0;
				idVar[index] = map.Chi[k][i][delta];
				coeff[index] = 1.0;
				index++;
				for(int r = 0; r < R; r++){
					idVar[index] = map.Rho[i][r];
					coeff[index] = instance.s[i][instance.R[r][delta]];
					index++;
				}
				idVar[index] = map.T[k][i];
				coeff[index] = -instance.S[k][delta]+instance.E;
				index++;
				char sense = 'L';
				int matbeg = 0;
				double rhs = instance.E;
				snprintf(name, NAME_SIZE, "(8) %d %d",i,delta);
				char* cname = (char*)(&name[0]);
				CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coeff[0], 0, &cname);
				/* Checking that the vector is correctly initialized */
				assert(index == idVar.size());
				number_of_constraint++;
			}
		}
	}
	
	/* (25) */
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
	
	
	/* (26) */
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
		
		
	//Constraint (27) 
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int delta = 0; delta < 3; delta++){
				vector< int > idVar(2);
				vector< double > coeff(2);
				idVar[0] = map.Chi[k][i][delta];
				coeff[0] = 1.0;
				idVar[1] = map.T[k][i];
				coeff[1] = -instance.E;
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
	
	//Add this constraint for the accessory problem
	
	for(int k = 0; k < K; k++){
		for(int i = 0; i < N; i++){
			for(int j = 0;  j < N; j++){
				for(int delta = 0; delta < 3; delta++){
					vector <int> idVar(2);
					vector <double> coef(2);
					idVar[0] = map.B[k][i][j][delta];
					coef[0] = 1.0;
					idVar[1] = map.Z[k];
					coef[1] = -1.0;
					char sense = 'L';
					int matbeg = 0;
					double rhs = 0;
					//snprintf(name, NAME_SIZE, "(12) %d %d %d", i, j, delta);
					//char * cname = (char*) (&name[0]);
					CHECKED_CPX_CALL( CPXaddrows, env, lp, 0, 1, idVar.size(), &rhs, &sense, &matbeg, &idVar[0], &coef[0], 0, NULL);
				}
			}
		}
	}
	
	
	
	
	
	
	
	
	//Constraint (28): b_{kij}^\delta <= t_i ==> b_{ij}^\delta - t_i = 0
	
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
	
	
	/*Constraint (29): b_{ji}^\delta <= t_j ==> b_{ji}^\delta - t_j = 0
	for(int k = 0; k < K; k++){
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
	
	
	/* Constraint (30) */
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
	
	
	/* Constraint (Multi) (31) */
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
	
	/* (32) */
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
	
	
}

/**
 * set up the model for CPLEX
 * @param env the cplex enviroment
 * @param lp the cplex problem
 * @param instance the 3(B)KP instance of the problem
 * @param oFlag an object containing the configuration information
 * @return an object containing the indixes of the variables inside the CPLEX problem 
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
	if(oFlag.extended){
		setupLPBalancingConstraints(env, lp, instance, map);
	}
	
	
	if(oFlag.output_required){
		CHECKED_CPX_CALL( CPXwriteprob, env, lp, "/tmp/Model.lp", NULL ); 
	}
	
	return map;
}
