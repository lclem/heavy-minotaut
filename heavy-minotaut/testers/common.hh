
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Header file declaring auxiliary functions for the execution of              *
 * experiments. It includes functions that list files and folders in the            *
 * filesystem.                                                                      *
 * 																					*
 ************************************************************************************/

#ifndef COMMON_H
#define COMMON_H


#include "../src/Common/common.hh"
#include <dirent.h>

vector<filename> getAllFilenames(const vector<string>& dirs);
vector<filename> getAllFilenames(const string dir);
vector<string> getAllSubfolders(const string folder_str);
string appendTimbukFilename(string filename, string sufix);

#endif // COMMON_H
