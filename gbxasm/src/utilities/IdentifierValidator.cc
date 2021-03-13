#include "IdentifierValidator.h"

using namespace std;

namespace gbxasm::utilities
{

bool IdentifierValidator::IsValid(string_view candidate)
{
    auto characterCounter = 0;
    for (auto c : candidate)
    {
        if (characterCounter == 0 && IsInitialDigit(candidate, 0))
                return false;

        if (c != '_' && c != 'a' && c != 'b' && c != 'c' && c != 'd' && 
            c != 'e' && c != 'f' && c != 'g' && c != 'h' && c != 'i' && 
            c != 'j' && c != 'k' && c != 'l' && c != 'm' && c != 'n' && 
            c != 'o' && c != 'p' && c != 'q' && c != 'r' && c != 's' && 
            c != 't' && c != 'u' && c != 'v' && c != 'w' && c != 'x' && 
            c != 'y' && c != 'z' && c != 'A' && c != 'B' && c != 'C' && 
            c != 'D' && c != 'E' && c != 'F' && c != 'G' && c != 'H' && 
            c != 'I' && c != 'J' && c != 'K' && c != 'L' && c != 'M' &&
            c != 'N' && c != 'O' && c != 'P' && c != 'Q' && c != 'R' && 
            c != 'S' && c != 'T' && c != 'U' && c != 'V' && c != 'W' && 
            c != 'X' && c != 'Y' && c != 'Z' && c != '0' && c != '1' &&
            c != '2' && c != '3' && c != '4' && c != '5' && c != '6' &&
            c != '7' && c != '8' && c != '9')
            return false;

        characterCounter++;
    }

    return true;
}

bool IdentifierValidator::IsInitialDigit(string_view candidate, size_t position)
{
    if (candidate[position] == '1' || candidate[position] == '2' || candidate[position] == '3' || candidate[position] == '4' ||
        candidate[position] == '5' || candidate[position] == '6' || candidate[position] == '7' || candidate[position] == '8' ||
        candidate[position] == '9' || candidate[position] == '0')
        return true;

    return false;
}

}