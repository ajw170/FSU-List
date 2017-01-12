/*  list2c.cpp
 
    Andrew J Wood
    FSU ID: ajw14m
    January 11, 2017
 
    Implementation for alt2::List<T>.  This file implements the functions and classes defined in list2c.h.
 
 */


// --Stand alone functions: These functions are global functions and are not part of any class

template < typename T >
bool operator == (const List<T> &x1, const List<T> &x2)
{
    typename List<T>::ConstIterator i1, i2;  //declare an iterator to traverse each list; ensures iterators are
    for (                                    //are of the proper type
         i1 = x1.Begin(), i2 = x2.Begin();          //position iterators at 1st valid node
         (i1 != x1.End()) && (i2 != x2.End());      //stop iterators once they reach last valid node
         ++i1, ++i2                                 //increment iterators
         )
    {
        if (*(i1) != *(i2))
            return 0;
    }
    if (i1 != x1.End() || i2 != x2.End())   //if the for loop exited before one of the iterators reached its tail node
        return 0;
    return 1;
}


template < typename T >
bool operator != (const List<T>& x1, const List<T>& x2)
{
    return !(x1 == x2);  //utilize == operator function
}


template < typename T >
std::ostream& operator << (std::ostream& os, const List<T> list)
{
    list.Display(os);
    return os;  //enables chaining in output statements e.g. L1 << L2 << L3
}

// --End of stand alone functions

template < typename T >
List<T>::Link::Link (const T& Tval) : Tval_(Tval), prev_(nullptr), next_(nullptr)
// Link constructor
{}
