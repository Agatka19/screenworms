#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <arpa/inet.h>


using namespace std;

/****************************
*       return values
****************************/
const int RETURN_OK = 0;
const int RETURN_ERROR = 1;

/****************************
*     conversion to number
****************************/
template <typename T, typename U, typename V>
T to_number(U &&name, V &&string,
            T min_val = numeric_limits<T>::min(),
            T max_val = numeric_limits<T>::max())
{
    T result;
    istringstream str(forward<V &&>(string));

    auto fail = [&str, &name]() {
        ostringstream msg;
        msg << "Option \"" << name << "\": cannot convert \"" << str.str() << "\" to number in a proper range.";
        throw runtime_error(msg.str());
    };

    for (auto c : str.str())
        if (!isdigit(c))
            fail();

    str >> result;
    if (result < min_val || max_val < result)
        fail();

    if (!str || str.peek() != EOF)
        fail();

    return result;
}

/****************************
*       throw error
****************************/
template <typename T>
void exit_with_error(T &&error_msg) 
{
    cerr << forward<T>(error_msg) << endl;
    exit(1);
}

#endif // UTILS_H