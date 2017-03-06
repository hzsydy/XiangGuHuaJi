#include "military_kernel.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

static string replaceAll( string const& original, string const& from, string const& to )
{
    string results;
    string::const_iterator end = original.end();
    string::const_iterator current = original.begin();
    string::const_iterator next = search( current, end, from.begin(), from.end() );
    while ( next != end ) {
        results.append( current, next );
        results.append( to );
        current = next + from.size();
        next = search( current, end, from.begin(), from.end() );
    }
    results.append( current, next );
    return results;
}


bool loadMilitaryKernel(vector<vector<float> >& militaryKernel, string mknl_filename) {

    try {

        ifstream ifs(mknl_filename);
        if( !ifs ){
            cout<<"Failed to load \""<< mknl_filename << "\". Aborted. " << endl;
            return false;
        }
        string line, linereplaced;
        stringstream ss;

        militaryKernel.resize(2*MILITARY_KERNEL_SIZE-1);

        for (int i=0; i<2*MILITARY_KERNEL_SIZE-1; i++) {
            while (!ifs.eof()) {
                getline(ifs, line);
                if (line.c_str()[0] == '|') break;
            }

            linereplaced = replaceAll(line, "|", "");
            ss = stringstream(linereplaced);

            militaryKernel[i].resize(2*MILITARY_KERNEL_SIZE-1);
            for (int j=0; j<2*MILITARY_KERNEL_SIZE-1; j++)
            {
                float f = 0;
                ss >> f;
                militaryKernel[i][j] = f;
            }
        }

        return true;

    }
    catch (...) {
        cout << "Please check out the file " + mknl_filename << " which seems to be damaged. "<< endl;
        return false;
    }

    return false;
}