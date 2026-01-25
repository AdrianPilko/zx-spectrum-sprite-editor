#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <algorithm>

// code to generate a Sinclair zx spectrum sprite suitable for my own programs -
// in other words the ordering in memory works for me, that is all the bytes of the
// first character (8x8 pixels) then the next horizontally for another 2, then the
// next row then the last in a total of 3x3 grid (24 pixels by 24 pixels.

// this is laid out in memory as contiguous line.

//00000000 11111111 22222222
//00000000 11111111 22222222
//00000000 11111111 22222222
//00000000 11111111 22222222
//00000000 11111111 22222222
//00000000 11111111 22222222
//00000000 11111111 22222222
//00000000 11111111 22222222
//
//33333333 44444444 55555555
//...
//...
//66666666 77777777 88888888
//...
//...

// future version will have variable width and height, as well as other options

// the input is simply pixel on and pixel off being 0 = off 1 = on
void rotateInnerDimension(std::vector<std::vector<char>>& matrix, int k) {
    for (auto& inner_vec : matrix) {
        if (inner_vec.empty()) continue;
        
        // Ensure k is within the range of the current row size
        int actual_shift = k % inner_vec.size();
        // std::rotate with reverse iterators performs a right rotation
        std::rotate(inner_vec.rbegin(), inner_vec.rbegin() + actual_shift, inner_vec.rend());
    }
}

int32_t parseInput(const std::string & inFileName, 
                        const std::string & outFileName, 
                        const int shiftBy,
                        const bool mirrorImage,
                        const int VerticalDimension=24,
                        const int HorizontalDimension=24)
{
    const int32_t c_NumberOfCharBlocks{9};

    std::fstream iStream(inFileName, std::ios::in);
    std::fstream oStream(outFileName, std::ios::out);

    if (!iStream.is_open() || !oStream.is_open()) 
    {
       std::cerr << "Error: Could not open the file(s)!" << std::endl;
       return EXIT_FAILURE;
    }

    oStream << "spriteData:" << std::endl;
    std::string line;
    std::vector<std::string> fileAsStrVec;
    
    for (int i = 0; i < VerticalDimension; i++)
    {
        fileAsStrVec.push_back(std::string(HorizontalDimension, '-'));
    }

    int32_t currentLineLen = 0;
    size_t index = 0;

    while (std::getline(iStream, line)) 
    {
        currentLineLen = line.size();
        
        if (currentLineLen >= HorizontalDimension)
        {
            std::cout << "ERROR: Lines wrong length " << currentLineLen << std::endl;
            return EXIT_FAILURE;
        }
        line.resize(HorizontalDimension,' ');
        fileAsStrVec.at(index++) = line;
        if (index > VerticalDimension)
        {
            std::cout << "ERROR: number of lines exceeds expected " << fileAsStrVec.size() << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "Number of rows == " << fileAsStrVec.size() << " number of columns==" << currentLineLen << std::endl;
    std::vector < std::vector <char > > twoDimGrid;

    twoDimGrid.resize(VerticalDimension, std::vector<char>(HorizontalDimension));
    for (int row = 0; row < VerticalDimension; row++)
    {
        for (int col = 0; col < HorizontalDimension; col++)
        {
            if (fileAsStrVec[row][col] == ' ')
            {
               twoDimGrid[row][col] = '-';
            }
            else
            {
               twoDimGrid[row][col] =  '*';
            }
        }
    } 

    std::ostringstream buffer;
    std::vector<std::ostringstream> outputSS;
    outputSS.resize(c_NumberOfCharBlocks);
    for (size_t i = 0; i < c_NumberOfCharBlocks; i++)
    {
        outputSS[i].str("");
    }

    int32_t third = 0;

    if (shiftBy != 0)
    {
        std::cout << "rotating sprite by " << shiftBy << std::endl;
        rotateInnerDimension(twoDimGrid, shiftBy);
    }

    for (auto rowOuter = 0; rowOuter < fileAsStrVec.size(); rowOuter++)
    {   
        for (auto columnOuter = 0; columnOuter < fileAsStrVec[rowOuter].size(); columnOuter++)
        {              
            if (twoDimGrid[rowOuter][columnOuter] == '*')
            {
                buffer << '1';
                std::cout << '*' ;
            }
            else
            {
                buffer << '0';
                std::cout << '-';
            }
            if (columnOuter == fileAsStrVec[rowOuter].size()-1)
            {
                std::cout << std::endl;
            }

            if (third == 0)
            {
                if (columnOuter == 7)
                {
                    outputSS[0] << "defb %" << buffer.str() << std::endl;
                    buffer.str("");
                }
                if (columnOuter == 15)
                {
                    outputSS[1] << "defb %" << buffer.str() << std::endl;
                    buffer.str("");
                }
                if (columnOuter == 23)
                {
                    outputSS[2] << "defb %" << buffer.str() << std::endl;
                    buffer.str("");
                }
            } else if (third == 1)
            {
                if (columnOuter == 7)
                {
                    outputSS[3] << "defb %" << buffer.str() << std::endl;
                    buffer.str("");
                }
                if (columnOuter == 15)
                {
                    outputSS[4] << "defb %" << buffer.str() << std::endl;
                    buffer.str("");
                }
                if (columnOuter == 23)
                {
                    outputSS[5] << "defb %" << buffer.str() << std::endl;
                    buffer.str("");
                }
            } else if (third == 2)
            {   
                if (columnOuter == 7)
                {
                    outputSS[6] << "defb %" << buffer.str() << std::endl;
                    buffer.str("");
                }
                if (columnOuter == 15)
                {
                    outputSS[7] << "defb %" << buffer.str() << std::endl;
                    buffer.str("");
                }
                if (columnOuter == 23)
                {
                    outputSS[8] << "defb %" << buffer.str() << std::endl;
                    buffer.str("");
                }
            }
        }
        if (rowOuter == 7) third++;
        if (rowOuter == 15) third++;
        if (rowOuter == 23) third++;
    }
    // 0 1 2 
    // 3 4 5
    // 6 7 8
  
    for (size_t i = 0; i < c_NumberOfCharBlocks; i++)
    {
        oStream << outputSS[i].str() << std::endl;
    }         

    iStream.close();
    oStream.close();
    std::cout << "DONE" << std::endl;
    return EXIT_SUCCESS;
}

int main(int argc, char * argv[])
{
    //#define TEST_ROTATE

    #ifdef TEST_ROTATE
    // test rotate
    std::vector< std::vector<char> > vec;
    for (int row = 0; row < 8; row++)
    {
        std::vector<char> temp;
        for (int col = 0; col < 24; col++)
        {
            char c = (char)std::to_string(col % 8).c_str()[0];
            temp.push_back(c);
        }
        vec.push_back(temp);
    }
    rotateInnerDimension(vec, 1);
    return 0;
    #endif

    int32_t retVal = EXIT_SUCCESS;
    std::string inFile("NOT_SET");
    std::string outFile("NOT_SET");
    int shiftBy = 0;
    int shiftDirection = 1;
    bool mirror = false;
    int VerticalDimension = 24;
    int HorizontalDimension = 24;
    bool argsOK = false;

    std::cout << "Text based ZX Spectrum Sprite Editor by Adrian Pilkington(2026)" << std::endl;
    if (argc >= 5)
    {
        inFile = argv[1];
        outFile= argv[2];
        shiftBy = atoi(argv[3]);
        mirror = atoi(argv[4]) == 1 ? true : false;
        argsOK = true;
    } 
    else
    {
        std::cout << "useage: " << argv[0] << " <input file> <output file> <shift by>  <mirror> [(optional)<rows><cols>])" << std::endl;
        retVal = EXIT_FAILURE;
        argsOK = false;
    }
    if (argc == 7) // optional dimensions
    {
        VerticalDimension = atoi(argv[5]);
        HorizontalDimension = atoi(argv[6]);
        argsOK = true;
    }
    if (argsOK)
    {
        std::cout << "Using input file=" << inFile << " outputting to " << outFile << std::endl;
        if (parseInput(inFile,outFile, shiftBy, mirror,VerticalDimension,HorizontalDimension) != EXIT_SUCCESS)
        {
            std::cout << "Fault found in input file" << std::endl;
        }
    }
    return retVal;
}
