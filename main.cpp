#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <sstream>

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

// the input is simply pixel on and pixel off being 0 = off 1 = on
                                            
int32_t parseInput(const std::string & inFileName, const std::string & outFileName)
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
    int32_t lastLineLen = 0;
    int32_t currentLineLen = 0;
    while (std::getline(iStream, line)) 
    {
        currentLineLen = line.size();
        
        if ((currentLineLen != lastLineLen && lastLineLen != 0) || (currentLineLen % 2 != 0))
        {
            std::cout << "ERROR: Lines not all even length" << std::endl;
            return EXIT_FAILURE;
        }
        lastLineLen = currentLineLen;
        fileAsStrVec.push_back(line);
    }
    // the number of lines also has to be even
    if (fileAsStrVec.size() % 2 != 0)
    {
        std::cout << "ERROR: number of lines is not even" << fileAsStrVec.size() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Number of rows == " << fileAsStrVec.size() << " number of columns==" << currentLineLen << std::endl;
    
    std::vector < std::vector <char > > twoDimGrid;

    twoDimGrid.resize(24, std::vector<char>(24)); 

    std::ostringstream buffer;
    std::vector<std::ostringstream> outputSS;
    outputSS.resize(c_NumberOfCharBlocks);
    for (size_t i = 0; i < c_NumberOfCharBlocks; i++)
    {
        outputSS[i].str("");
    }

    int32_t third = 0;

    for (auto rowOuter = 0; rowOuter < fileAsStrVec.size(); rowOuter++)
    {   
        for (auto columnOuter = 0; columnOuter < fileAsStrVec[rowOuter].size(); columnOuter++)
        {  
            twoDimGrid[rowOuter][columnOuter] = fileAsStrVec[rowOuter][columnOuter];
            std::cout << twoDimGrid[rowOuter][columnOuter] << " " ;
            
            if (twoDimGrid[rowOuter][columnOuter] == '1')
            {
                buffer << "1";
            }
            else
            {
                buffer << "0";
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
                    third++;
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
                    third++;
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
                    third = 0;
                }
            }
        }
    }
    // 0 1 2 
    // 3 4 5
    // 6 7 8
  
    oStream << outputSS[0].str() << std::endl;
    oStream << outputSS[1].str() << std::endl;
    oStream << outputSS[2].str() << std::endl;

    oStream << outputSS[3].str() << std::endl;
    oStream << outputSS[4].str() << std::endl;
    oStream << outputSS[5].str() << std::endl;

    oStream << outputSS[6].str() << std::endl;
    oStream << outputSS[7].str() << std::endl;     
    oStream << outputSS[8].str() << std::endl;     
                 

    iStream.close();
    oStream.close();
    std::cout << "----" << std::endl;
    return EXIT_SUCCESS;
}

int main(int argc, char * argv[])
{
    int32_t retVal = EXIT_SUCCESS;
    std::string inFile("NOT_SET");
    std::string outFile("NOT_SET");

    std::cout << "Text based ZX Spectrum Sprite Editor by Adrian Pilkington(2026)" << std::endl;
    if (argc == 3)
    {
        inFile = argv[1];
        outFile= argv[2];
        std::cout << "Using input file=" << inFile << " outputting to " << outFile << std::endl;
        if (parseInput(inFile,outFile) != EXIT_SUCCESS)
        {
            std::cout << "Fault found in input file" << std::endl;
        }
    }
    else
    {
        std::cout << "useage: " << argv[0] << " <input file> <output file>" << std::endl;
        retVal = EXIT_FAILURE;
    }
    return retVal;
}