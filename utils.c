#include "utils.h"

/** Struct containing the long options */
static struct option long_options[] = {	
	{"help", no_argument, 0, 'h'},		/* Print a help message and exit */
	{"quiet", no_argument, 0, 'q'},		/* Do not produce extra files */
	{"extended", no_argument, 0, 'e'},  
	{"disable-x", no_argument, 0, 'x'}, /* Disable optimization wrt x */
	{"disable-y", no_argument, 0, 'y'}, /* Disable optimization wrt y */
	{"disable-z", no_argument, 0, 'z'}, /* Disable optimization wrt z */
	{"timeout", required_argument, 0, 't'}, /* Timeout */
	{"ignore-knapsack-cost", no_argument, 0, 'i'}, /* Disable optimization of knapsacks */
	{0, 0, 0, 0},
};

/** 
 * print the help message
 * @param argv in order to get the name of the compiled program
 */
void print_help(char * argv[]){
	cout << "Usage : " << argv[0] << " <instance.dat> " << " [OPT]... " << endl;
	cout << endl;
	cout << "-h, --help\t\t\t print this message and exit" << endl;
	cout << "-q, --quiet\t\t\t do not write the solved problem in a file named /tmp/Model.sol, Model.lp" << endl;
	cout << "-e, --extended\t\t\t use also the balancing constraint." << endl;
	cout << "-x, --disable-x\t\t\t do not optimize w.r.t. x" << endl;
	cout << "-y, --disable-y\t\t\t do not optimize w.r.t. y" << endl;
	cout << "-z, --disable-z\t\t\t do not optimize w.r.t. z" << endl;
	cout << "-t, --timeout\t\t\t set a timeout in seconds, after which CPLEX will stop (It returns the incumbent solution)" << endl;
	cout << "-i, --ignore-kanpsack-cost\t\t the program will not try to optimize the number of knapsacks" << endl;
	cout << endl;
}


/**
 * function that according to the given options set the flags
 * 	--output, in order to print the solved problem in the given file
 *  --print, in order to print the TSP-Solution 0 <1, 2 .. n> 0
 *  --benchmark_output print a special form of output N user_time cpu_time objval
 * @param argc 
 * @param argv
 */
optionFlag get_option(int argc,  char * argv[]){
	optionFlag oFlag;
	int c;
	if(argc < 2) {
		print_help(argv);
		exit(EXIT_FAILURE);
	}
	// One can specify only the help flag without the instance file.
	if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
		print_help(argv);
		exit(EXIT_SUCCESS);
	}
	optind = 2; //Starting from index 2, because the first place is destinated to the istance file.
	int option_index;
	oFlag.timeout = 1e75; /* Default value in CPLEX ~ 3 centuries */
	while((c = getopt_long (argc, argv, "hqet:xyzi", long_options, &option_index)) != EOF) {
		switch(c){
			case 'h': print_help(argv); exit(EXIT_SUCCESS); break;
			case 'q': oFlag.output_required=false; break;
			case 'e': oFlag.extended = true; break;
			case 't': oFlag.timeout = strtod(optarg, NULL); 
				printf("Timeout set to %.2lf\n", oFlag.timeout);
				break;
			case 'x': oFlag.optimize[0] = false; break;
			case 'y': oFlag.optimize[1] = false; break;
			case 'z': oFlag.optimize[2] = false; break;
			case 'i': oFlag.optimizeKnapsackCost = false; break;
		}
    }
    sprintf(oFlag.filename, "%s", argv[1]);
    return oFlag;
}


