/**
 * @author Mirko Bez
 * @file utils.h
 * This file contains some data structure that are specific for the HULL Problem and
 * contains a data structure to map the variables to their CPLEX indexes and to map
 * the variables to the computed values.
 */
#ifndef __UTILS__H__
#define __UTILS__H__

#include <vector>
#include <string.h>

using namespace std;


/**
 * Struct that contains all the vectors used to keep track of the variables
 */
struct mapVar {
	private : 
	mapVar() : T(0, vector< int >(0)), B(0, vector<vector<vector<int>>>(0, vector<vector<int>>(0, vector< int >(0)))),
			   Chi(0, vector< vector<int> >(0, vector<int>(0))),
			   Sigma(0, vector< int >(0)),
			   Rho(0, vector< vector<int> >(0, vector<int>(0)))
			    { }
	public : 
	vector< vector < int > > T;
	vector< vector < vector < vector < int > > > > B;
	vector< vector < vector <int > > > Chi;
	vector< int > Z;
	vector< vector < int > > Sigma;
	vector< vector < vector < int > > > Rho;
	
	mapVar(int K, int N, int R){
		T.resize(K);
		for(auto &i : T)
			i.resize(N);
		
		B.resize(K);
		for(auto &i : B) {
			i.resize(N);
			for(auto &k : i) {
				k.resize(N);
				for(auto &j : k) {
					j.resize(3);
				}
			}
		}
		
		
		Chi.resize(K);
		for(auto &k : Chi){
			k.resize(N);
			for(auto &i : k){
				i.resize(3);
			}
		}
		Z.resize(K);
		Sigma.resize(K);
		for(auto &S : Sigma){
			S.resize(3);
		}
		
		
		Rho.resize(K);
		for(auto &i : Rho){
			i.resize(N);
			for(auto &j: i){
				j.resize(R);
			}
		}
		
		
		
	}
};
/**
 * Struct that contains all the vectors used to keep track of the variables
 */
struct VarVal {
	private : 
	VarVal() : t(0, vector< int >(0)), b(0, vector<vector<vector<int>>>(0, vector<vector<int>>(0, vector< int >(0)))),
			   chi(0, vector< vector<double> >(0, vector<double>(0))), rotation(0, vector<int>(0)),
			   sigma(0, vector< double >(0)),
			   rho(0, vector< vector<int> >(0, vector<int>(0)))
			    { }
	public : 
	vector< vector< int > > t;
	vector< vector < vector < vector < int > > > > b;
	vector< vector < vector < double > > > chi;
	vector< int > z;
	vector< vector< int > > rotation;
	vector< vector< double > > sigma;
	vector< vector < vector < int > > > rho;
	
	VarVal(int K, int N, int R){
		t.resize(K);
		for(auto &i : t)
			i.resize(N);
		
		b.resize(K);
		for(auto &i : b) {
			i.resize(N);
			for(auto &k : i) {
				k.resize(N);
				for(auto &j : k) {
					j.resize(3);
				}
			}
		}
		
		
		
		chi.resize(K);
		for(auto &k : chi){
			k.resize(N);
			for(auto &i : k){
				i.resize(3);
			}
		}
		z.resize(K);
		
		rotation.resize(N);
		for(auto &r : rotation){
			r.resize(3);
		}
		sigma.resize(K);
		for(auto &S : sigma){
			S.resize(3);
		}
	
		rho.resize(K);
		for(auto &i : rho){
			i.resize(N);
			for(auto &j: i){
				j.resize(R);
			}
		}
		
		
	}
};


#endif

