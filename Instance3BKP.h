/**
 * @author Mirko Bez
 * @file main.cpp
 * @brief help class containing the 3BKP instance. It is accountable
 * for the parsing of the input file
 */

#ifndef __INSTANCE_3BKP__H__
#define __INSTANCE_3BKP__H__
 
#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <stdexcept>
#include <sstream>      // std::stringstream
#include <string>
#include <assert.h>

class Instance3BKP {
	private : 
	Instance3BKP() : s(0, std::vector<double>(0)),mass(0) , profit(0), S(0, std::vector<double>(0)) { }
		
	
	
	void computeL() {
		L = -1e75;
		for(int k = 0; k < K; k++){
			for(int delta = 0; delta < 3; delta++){
				if(L < S[k][delta])
					L = S[k][delta];
			}
		}
		L += 1;
		printf("L = %lf\n", L);
	}
	
	void verifyExistenceCondition() {
		for(int i = 0; i < N; i++){
			assert(profit[i] >= 0);
		}
		for(int k = 0; k < K; k++){
			assert(fixedCost[k] >= 0);
		}
	}
	
	public:
		/** Number of items, i.e. cardinality of J */
		int N;
		
		/** Number of knapsacks, i.e. cardinality of K */
		int K;
		
		double L;
		
		
		/** vector containing the three dimension of the items */
		std::vector< std::vector< double > > s;
		
		
		/** vector containing the masses of the items */
		std::vector< double > mass;
		
		/** vector containing the profits of the items */
		std::vector< double > profit;
		
		
		/** vector containing the three dimension of the k knapsack */
		std::vector< std::vector< double > > S;
		
		
		std::vector< double > fixedCost;
		
		bool extended;
		
		/** Set of rotation of the object. i.e. permutation of 1,2,3 */
		double R [6][3] = {
			{0,1,2},
			{0,2,1},
			{1,0,2},
			{1,2,0},
			{2,0,1},
			{2,1,0},
		};
		
	
	public:
		/**
		 * @return the volume of the box
		 */
		double getBoxVolume(int k){
			return S[k][0]*S[k][1]*S[k][2];
		}
		/**
		 * @return the volume of the i-th item
		 */
		double getVolume(int i){
			return s[i][0]*s[i][1]*s[i][2];
		}
		
		
		/**
		 * Constructor
		 * @param filename name of the file to parse
		 * @param _extended true if the model is extended with 
		 * stability constraint, false otherwise
		 */
		Instance3BKP(const char * filename, bool _extended){
			extended = _extended;
			std::ifstream infile(filename);
			std::string line;
			std::istringstream ss;
			
			if(!infile.good()) {
				throw std::runtime_error("The given file does not exist");
			}
			std::getline(infile, line);
			ss.str(line);
			ss >> K;
			std::cout << K << std::endl;
			
			S.resize(K);
			fixedCost.resize(K);
			for(auto &i : S) i.resize(3);
			
			for(int k = 0; k < K; k++){
				std::getline(infile, line);
				ss.clear();
				ss.str(line);
				ss >> S[k][0] >> S[k][1] >> S[k][2] >> fixedCost[k];
			}

			std::getline(infile, line);
			ss.clear();
			ss.str(line);
			
			ss >> N;
			s.resize(N);
			for(auto &i : s) i.resize(3);
			mass.resize(N);
			profit.resize(N);
			
			for(int i = 0; i < N; i++){
				std::getline(infile, line);
				ss.clear();
				ss.str(line);
				ss >> s[i][0] >> s[i][1] >> s[i][2] >> mass[i] >> profit[i];
				
			}
			infile.close();
			verifyExistenceCondition();
			computeL();
		}
		
		/**
		 * Print the instance
		 */
		void print(){
			std::cout << "There are " << K << " Knapsacks " << std::endl;
			for(int k = 0; k < K; k++){
				std::cout << S[k][0] << " " << S[k][1] << " " << S[k][2] << " " << fixedCost[k] << std::endl;
			}
			std::cout << "There are " << N << " Items " << std::endl;
			for(int i = 0; i < N; i++){
				std::cout << s[i][0] << " " << s[i][1] << " " << s[i][2] << " " << mass[i] << " " << profit[i] << std::endl;
			}
			
		}


};

#endif /* __INSTANCE_3BKP__H__ */

