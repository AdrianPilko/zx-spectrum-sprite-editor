#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>

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
    std::fstream iStream(inFileName, std::ios::in);
    std::fstream oStream(outFileName, std::ios::out);

    if (!iStream.is_open() || !oStream.is_open()) 
    {
       std::cerr << "Error: Could not open the file(s)!" << std::endl;
       return EXIT_FAILURE;
    }

    oStream << "spriteData" << std::endl;
    oStream << "   DB ";   
    std::string line;
    std::vector<std::string> fileAsStrVec;
    int32_t lastLineLen = 0;
    int32_t currentLineLen = 0;
    while (std::getline(iStream, line)) 
    {
        currentLineLen = line.size();
        // check all lines are the same length and have to be even number
        // to maintain the 2x2 block valid size
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
    std::cout << "This becomes a graphic of " << fileAsStrVec.size() / 2 << "x" << currentLineLen/2 << " character blocks"<< std::endl;
    

    // need to check each 2x2 block for the pixel type, the -1 is to go to penulimate row in outer loop
    // as the nested loop will handle this
    
    for (auto rowOuter = 0; rowOuter < fileAsStrVec.size() - 1; rowOuter+=2)
    {   
        for (auto columnOuter = 0; columnOuter < fileAsStrVec[rowOuter].size() - 1; columnOuter+=2)
        {
            int32_t linearIndex = 0;
            std::string linearVersion = "    ";
            for (auto row = 0; row < 2; row++)
            {
                for (auto col = 0; col < 2; col++)
                {
                    linearVersion[linearIndex++] = fileAsStrVec[row+rowOuter][col+columnOuter];
                }
            }
            bool found = false;

            for (size_t fIndex = 0; fIndex < patterns.size(); fIndex++)
            {
                if (patterns[fIndex].find(linearVersion) != std::string::npos)
                {
                    oStream << patternHexCodes.at(fIndex);

                    if (columnOuter < fileAsStrVec[rowOuter].size() - 2)
                    {
                        oStream << ",";
                    }
                    found = true; 
                } 
            }
            if (found == false)
            {
                std::cerr << "unknown character combination at row" << rowOuter << ", col " << columnOuter << 
                    " inserting blank to maintain row column in output!!" << std::endl;
                    oStream << patternHexCodes.at(0);

                if (columnOuter < fileAsStrVec[rowOuter].size() - 2)
                {
                    oStream << ",";
                }
            }                  
        }

        oStream <<std::endl;
        if (rowOuter < fileAsStrVec.size() - 2)
        {
            oStream << "   DB ";
        }
    }

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

    std::cout << "Text based ZX81 Sprite Editor by Adrian Pilkington(2025)" << std::endl;
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