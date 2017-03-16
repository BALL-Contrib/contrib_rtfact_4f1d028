/* 
 *  primegen.cpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Created on: 2009-03-06 23:29:23 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#ifdef _WIN32
#   define DIR_SEP '\\'
#else
#   define DIR_SEP '/'
#endif

using namespace std;

static const unsigned int precomputedPrimes[] =
{
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31
};

const int precomputedPrimeCount = sizeof(precomputedPrimes) / sizeof(int);

void printUsage(
    const string& aExecutableName)
{
    cerr <<
        endl <<
        "Usage: " <<
        aExecutableName << " count count_per_line" <<
        std::endl << std::endl <<
        "       count:           numer of prime numbers to generate" << std::endl <<
        "       count_per_line:  numer of prime numbers per line" << std::endl << std::endl;
}

int main(
    int argc,
    char *argv[])
{
    string programName = argv[0];

    size_t sepIndex = programName.find_last_of(DIR_SEP);

    if(sepIndex != string::npos)
    {
        ++sepIndex;
    }

    programName = programName.substr(sepIndex, programName.size() - sepIndex);

    if(argc != 3)
    {
        printUsage(programName);

        return 1;
    }

    const int numbersToGenerate = atoi(argv[1]);
    const int numbersPerLine = atoi(argv[2]);

    if(numbersToGenerate < 1 || numbersPerLine < 1)
    {
        printUsage(programName);

        return 1;
    }

    unsigned int* primes = new unsigned int[numbersToGenerate];

    memcpy(primes, precomputedPrimes,
           min(precomputedPrimeCount, numbersToGenerate) * sizeof(int));

    int primesFound = precomputedPrimeCount;
    int primeCandidate = precomputedPrimes[precomputedPrimeCount - 1];

    while(primesFound < numbersToGenerate)
    {
non_prime:

        primeCandidate += 2;
        const unsigned int primeCandidateSqrt =
           static_cast<int>(sqrtf(static_cast<float>(primeCandidate)));

        for(int i = 0;
            i < primesFound && primes[i] <= primeCandidateSqrt; ++i)
        {
            if(primeCandidate % primes[i] == 0)
            {
                goto non_prime;
            }
        }

        primes[primesFound] = primeCandidate;

        ++primesFound;
    }

    for(int j = 0; j < numbersToGenerate; j += numbersPerLine)
    {
        for(int i = j; i < (j + numbersPerLine) && i < numbersToGenerate; ++i)
        {
            cout << primes[i]
                 << ((i == numbersToGenerate - 1) ? "" : ", ");
        }

        cout << std::endl;
    }

    return 0;
}
