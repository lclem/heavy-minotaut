
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
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

#include <boost/format.hpp>

typedef std::string string;
using chr_time = std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long int, std::ratio<1l, 1000000000l> > >;

using namespace std;
using std::pair;
using std::vector;
using std::tuple;
using std::set;
using std::unordered_set;
using filename = string;
using seconds = unsigned long int;

extern seconds TIMEOUT;

const char kPathSeparator =
#ifdef _WIN32
                            '\\';
#else
                            '/';
#endif

void exit_with_error(const char*);
void exit_with_error(const string);
void printVectorArrayInts(const vector<unsigned int[]> &);
unsigned int vectorUIntsAt(const vector<unsigned int>&, const unsigned int, const string);
void printVectorVectorInts(const vector<vector<int> > &);
string vectorVectorIntsToString(const vector<vector<int> >&);
vector<vector<bool> > createBoolMatrix(unsigned int numb_rows, unsigned int numb_columns, bool cell_value);
void writeToFile(string filename, string text, bool overwrite = false);
void outputText(string text, string filename = "");
string localTime();
string localTime2();
// delete these two later
chr_time startTimer();
float elapsed_sec(chr_time start);
unsigned long int secsSinceEpoch();
long unsigned int elapsed_sec(long unsigned int secsSinceEpoch_start);
unsigned int min(unsigned int a, unsigned int b);
vector<unsigned int> removeDuplicates(vector<unsigned int> vec);
vector<string> convertCharPtArrayToStrVector(char *array[], int start, int end);
string appendStrings(string s1, string s2, string s3 = "", string s4 = "");


#endif // COMMON_HH
