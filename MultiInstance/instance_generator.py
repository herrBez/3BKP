#!/usr/bin/python
# File that generates a random instance of the multiknapsack problem
__author__ = 'Mirko Bez'
import sys
import traceback
from random import randint

 

cmdargs = str(sys.argv)

try:
	print(cmdargs)
	f = open(sys.argv[1], "w")
	K = int(sys.argv[2]);
	J = int(sys.argv[3]);
	# Write the number of knapsack in file
	f.write(str(K) + "\n");
	# Generating K knapsacks
	for k in range(0,K):
		f.write(str(randint(100, 1000)) + " " + str(randint(100, 1000)) + " " + str(randint(100, 1000)) + "  " +str(randint(1, 1000)) + "\n")
	
	f.write(str(J) + "\n");
	for i in range(0,J):
		f.write(str(randint(1, 500)) + " " + str(randint(1, 500)) + " " + str(randint(1, 500)) + "  " + str(randint(1, 500)) + " " +  str(randint(1, 500)) +  "\n")
		
	f.write("");
	f.close()
	print("Success\n");
except Exception, ex:
	traceback.print_exc()
