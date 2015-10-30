
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-15	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file defining auxiliary functions for the execution of       *
 * tests. It includes functions that list files and folders in the                  *
 * filesystem.                                                                      *
 * 																					*
 ************************************************************************************/

#include "common.hh"

vector<filename> getAllFilenames(const string directory_str)
{
    vector<filename> all_filenames;

    const char* directory = directory_str.c_str();

    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir (directory)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            std::string buf(directory);
            buf.append(ent->d_name);

            std::string dir(directory);

            if (buf != dir+"."
                    && buf != dir+".."                  // Exclude the parent directory from the list.
                    && buf.at(buf.size()-1) != '~'      // Exclude temp files ~.
                    && buf != dir+"README.txt"
                    && VATA::Util::ReadFile(buf)!="")   // Exclude files with wrong syntax.
            {
                all_filenames.push_back(buf);
            }
        }
        closedir (dir);
    }
    else
    {
        /* If it could not open directory */
        perror ("error opening dir.");
    }

    return all_filenames;
}

vector<filename> getAllFilenames(const vector<string> &dirs)
{
    vector<filename> all_filenames;

    for (const string directory : dirs)
    {
        vector<filename> aux = getAllFilenames(directory);
        all_filenames.insert(all_filenames.end(), aux.begin(), aux.end());
    }

    return all_filenames;
}

vector<string> getAllSubfolders(const string folder_str)
{
    DIR *dir;
    struct dirent *ent;
    char* name;
    vector<string> subfolders;
    const char* folder = folder_str.c_str();

    if ((dir = opendir (folder)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            name = ent->d_name;

            if (name[0] == '.')     /* This ensures "." and ".." are not */
                continue;           /* added to the subfolders vector.   */

            if (ent->d_type == DT_DIR)
            {
                string name_str(name);
                subfolders.push_back(name_str);
            }
        }
    }

    return subfolders;
}

string appendTimbukFilename(string filename, string sufix)
{
    return filename.insert((filename.find(".timbuk")==string::npos) ? filename.size() : filename.find(".timbuk"), sufix);
}

string getLocalFilename(const string filename)
{

    std::size_t found = filename.find_last_of("/\\");
    if (found == string::npos)
        return filename;
    else
        return filename.substr(found+1);

}

