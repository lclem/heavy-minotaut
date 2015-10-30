
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file for a collection of definitions and functions which are of      *
 *  general utility, with no particular scope. The is the 'bottom' header file in   *
 *  this project, ie, any other header file is included, directly or indirectly,    *
 *  by this one.                                                                    *
 * 																					*
 ************************************************************************************/

#ifndef COMMON_HH
#define COMMON_HH


#include <utility>      // std::pair
#include <stddef.h>		/* Allows to use ptr = NULL, for eg. */
#include <string>       /* std::string, std::to_string */
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>    /* std::out_of_range */
#include <stddef.h>		/* Allows to use ptr = NULL, for eg. */
#include <stdexcept>    /* std::out_of_range */
#include <chrono>
#include <stdlib.h>
#include <stdint.h>

#include <vata/parsing/timbuk_parser.hh>

#include <vata/explicit_tree_aut.hh>	/* The internal automata representation from libvata */

typedef std::string string;
using chr_time = std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long int, std::ratio<1l, 1000000000l> > >;

using namespace std;
using std::pair;
using std::vector;
using std::tuple;
using std::set;
using std::unordered_set;
using filename = string;

extern long int TIMEOUT;

void exit_with_error(const char*);
void printVectorArrayInts(const vector<unsigned int[]> &);
unsigned int vectorUIntsAt(const vector<unsigned int>&, const unsigned int, const string);
void printVectorVectorInts(const vector<vector<int> > &);
string vectorVectorIntsToString(const vector<vector<int> >&);
vector<vector<bool> > createBoolMatrix(unsigned int numb_rows, unsigned int numb_columns, bool cell_value);
void writeToFile(string filename, string text, bool overwrite = false);
void outputText(string text, string filename = "");
string localTime();
string localTime2();


#endif // COMMON_HH
