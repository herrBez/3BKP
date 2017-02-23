#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
class Instance3BKP {
	Instance3BKP() : s(0, std::vector<double>(0)),mass(0) , profit(0) { }
	public:
		/* Size of the 3D Box */
		double S[3];
		double W, D, H;
		
		double M;
		
		std::vector< std::vector< double > > s;
		
		std::vector< double > mass;
		std::vector< double > profit;
		
		
		
		
		
		/* Set of rotation of the object. i.e. permutation of 1,2,3 */
		double R [6][3] = {
			{0,1,2},
			{0,2,2},
			{1,0,2},
			{1,2,0},
			{2,0,1},
			{2,1,0},
		};
		
	public:
		double getBoxVolume(){
			return W*D*H;
		}
		
		double getVolume(int i){
			return s[i][0]*s[i][1]*s[i][2];
		}
		
		/* Number of items */
		int N;
		/**
		 * Constructor
		 * @param filename name of the file to parse
		 * 
		 */
		Instance3BKP(const char * filename){
			std::ifstream infile(filename);
			infile >> S[0] >> S[1] >> S[2];
			W = S[0];
			D = S[1];
			H = S[2];
			infile >> N;
			s.resize(N);
			for(auto &i : s) i.resize(3);
			
			mass.resize(N);
			profit.resize(N);
			
			for(int i = 0; i < N; i++){
				infile >> s[i][0] >> s[i][1] >> s[i][2] >> mass[i] >> profit[i];
			}
			
			/* Initialize M as the sum over all masses */
			M = std::accumulate(mass.begin(), mass.end(), 0.0, 
									[](double a, double b) {
											return a + b;
									});
			
			infile.close();
		}
		
		/**
		 * Print the instance
		 */
		void print(){
			std::cout << S[0] << " " << S[1] << " " << S[2] << std::endl;
			for(int i = 0; i < N; i++){
				std::cout << s[i][0] << " " << s[i][1] << " " << s[i][2] << " " << mass[i] << " " << profit[i] << std::endl;
			}
		}
	


};

