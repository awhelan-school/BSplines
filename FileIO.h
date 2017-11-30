
#ifndef FILEIO_H
#define FILEIO_H

#include "Bezier.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <list>


using namespace std;
class FileIO {
public:
    FileIO();
    virtual ~FileIO(){}
    static void read(vector< list<vpt> > &Curves);
    static void write(vector< list<vpt> > Curves);
private:

};

#endif /* FILEIO_H */

