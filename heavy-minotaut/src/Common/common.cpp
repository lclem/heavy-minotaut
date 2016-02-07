
/************************************************************************************
 *                                  Heavy MinOTAut                  				*
 *              - heavy minimization algorithms for tree automata					*
 *                                                                                  *
 * 		Copyright (c) 2014-16	Ricardo Almeida	(LFCS - University of Edinburgh)	*
 * 																					*
 *	Description:																	*
 * 		Implementation file for a collection of definitions and functions which     *
 *  are of general utility, with no particular scope.                               *
 * 																					*
 ************************************************************************************/

#include "common.hh"

void exit_with_error(const char* msg)
{
	cerr << msg << endl;
	exit(EXIT_FAILURE);
}

void exit_with_error(const string msg)
{
    const char* c = msg.c_str();
    exit_with_error(c);
}

/* Functions that try to return the element at a certain position of a given vector
 * and throw an exception if a out_of_range error occurs. */

unsigned int vectorUIntsAt(const vector<unsigned int>& vec, const unsigned int pos, const string strct_name) {
	unsigned int result;
	
	try {
		result = vec.at(pos);
	}
	catch (const std::out_of_range& oor) {
		std::cerr << "Out of Range error: " << oor.what();
		std::cerr << " when acessing the position " << std::to_string(pos) <<
		" from " << strct_name << " which has size " << std::to_string(vec.size());
		exit_with_error("");
	}
	
	return result;
}

// Delete me later
chr_time startTimer()
{
    return std::chrono::high_resolution_clock::now();
}

chr_time time_now()
{
    return std::chrono::high_resolution_clock::now();
}

unsigned long int secsSinceEpoch()
{
    chr_time time = time_now();
    auto duration = time.time_since_epoch();
    unsigned long int secs = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    return secs;
}

long unsigned int elapsed_sec(long unsigned int secsSinceEpoch_start)
{
    unsigned long int secsSinceEpoch_now = secsSinceEpoch();
    return (secsSinceEpoch_now - secsSinceEpoch_start);
}

// Delete me later
float elapsed_sec(chr_time start)
{
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / (float) 1000000;
}

unsigned int min(unsigned int a, unsigned int b)
{
    return (a<b ? a : b);
}

vector<string> convertCharPtArrayToStrVector(char* array[], int start, int end)
{
    vector<string> vec;

    for (int i=start; i<=end; i++)
        vec.push_back(array[i]);

    return vec;
}


/* String and IO functions */

void printVectorArrayInts(const vector<unsigned int[2]>& vec)
{
	for (unsigned int j=0; j<vec.size(); j++){
        for (unsigned int i=0; i<5; i++)
			std::cout << (vec.at(j))[i] << ",";
		std::cout << "\n";
	}
}

string vectorVectorIntsToString(const vector<vector<int> >& vec)
{
	string str;
	
	for (unsigned int i=0; i<vec.size(); i++){
		vector<int> subvec = vec.at(i);
		for (unsigned int j=0; j<subvec.size(); j++)
			str += std::to_string(subvec.at(j)) + ",";
		str += "\n";
	}
	
	return str;
}

void printVectorVectorInts(const vector<vector<int> >& vec)
{
	std::cout << vectorVectorIntsToString(vec) << "\n";
}

vector<vector<bool> > createBoolMatrix(unsigned int numb_rows, unsigned int numb_columns, bool cell_value)
{
    vector<bool> column(numb_rows,cell_value);
    vector<vector<bool> > matrix(numb_columns,column);

    return matrix;
}

vector<unsigned int> removeDuplicates(vector<unsigned int> vec)
{
    sort( vec.begin(), vec.end() );
    vec.erase( unique( vec.begin(), vec.end() ), vec.end() );

    return vec;
}

string localTime()
{

    char s[1000];

    time_t t = time(NULL);
    struct tm * p = localtime(&t);

    strftime(s, 1000, "%a_(%Y-%b-%d-%T)", p);

    return s;
}

string localTime2()
{

    char s[1000];

    time_t t = time(NULL);
    struct tm * p = localtime(&t);

    strftime(s, 1000, "%Y_%b_%d_%T", p);

    return s;
}

void writeToFile(string filename, string text, bool overwrite)
{

    std::ofstream out;
    // std::ios::app is the open mode "append" meaning
    // new data will be written to the end of the file.
    out.open(filename, overwrite ? std::ios::trunc : std::ios::app);
    out << text;

}

void outputText(string text, string filename)
{

    if (filename == "")
    {
        std::cout << text << std::flush;
    }
    else
        writeToFile(filename, text);

}

string appendStrings(string s1, string s2, string s3, string s4)
{
    std::stringstream ss;
    ss << s1 << s2 << s3 << s4;
    return ss.str();
}
