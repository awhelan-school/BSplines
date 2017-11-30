#include "FileIO.h"

FileIO::FileIO()
{
}

std::string inputFile("./input.txt");

void FileIO::read(vector< list<vpt> > &Curves)
{

    Curves.clear();

    std::ifstream infile(inputFile, std::ios::in);

    std::string token;
    unsigned int objects = 0;
    unsigned int points = 0;
    vpt pt;

    unsigned int i = 0, j = 0, k = 0;

    if (infile.is_open()) {

        infile >> token;


        //Allocate for # of Curves
        if (token != "") {
            objects = std::stoi(token);
            Curves.resize(objects);
        }
        else {
            Curves.resize(1);
            return;
        }//If empty File 

        for (i = 0; i < objects; i++) {

            //GET Curve ID
            infile >> token;

            //Control Points
            infile >> points;

            for (j = 0; j < points; j++) {
                infile >> pt.x >> pt.y;
                Curves[i].push_back(pt); //push back point
            }//Push points on List


        }//scanObjects    
    } else {
        std::cout << "Unable to Open File " << inputFile << std::endl;
    }
    infile.close();
}//ReadData

void FileIO::write(vector< list<vpt> > Curves)
{

    std::ofstream outfile(inputFile, std::ios::trunc);
    std::string token;

    unsigned int objects = 0;
    unsigned int points = 0;
    vpt pt;


    if (outfile.is_open()) {

        objects = Curves.size();
        outfile << Curves.size();
        outfile << "\n";
        outfile << "\n";

        for (int i = 0; i < objects; i++) {

            //Put Curve ID
            outfile << i;
            outfile << "\n";

            //Put Control Points
            outfile << Curves[i].size();
            outfile << "\n";

            for (auto itr = Curves[i].begin(); itr != Curves[i].end(); itr++) {

                outfile << itr->x << " " << itr->y << "\n";

            }//Push points on List
            outfile << "\n";

        }//scanObjects   

    } else {
        std::cout << "Unable to Open File " << inputFile << std::endl;
    }
    outfile.close();
}//WriteData
