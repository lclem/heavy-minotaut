#ifndef COMMON_H
#define COMMON_H


#include "../src/Common/common.hh"
#include <vata/serialization/timbuk_serializer.hh>

#include <dirent.h>

//#include <boost/filesystem/operations.hpp>
//#include <boost/filesystem/path.hpp>
//#include <boost/filesystem.hpp>

vector<filename> getAllFilenames(const vector<string>& dirs);
vector<filename> getAllFilenames(const string dir);
vector<string> getAllSubfolders(const string folder_str);
string appendTimbukFilename(string filename, string sufix);

#endif // COMMON_H
