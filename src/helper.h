#include <sstream>

std::wstring to_wstring_prec(double n, int prec)
{
    std::wstringstream out;
    out.precision(prec);
    out << std::fixed << n;
    return out.str();
}