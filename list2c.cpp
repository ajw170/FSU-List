/*  list2c.cpp
 
    Andrew J Wood
    FSU ID: ajw14m
    January 11, 2017
 
    Implementation for alt2::List<T>.  This file implements the functions and classes defined in list2c.h.
 
    Credit: Portions of this code are adopted from the List<T> implementation in the FSU COP4530 library.
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





// --Memory allocator and other private (or protected) methods

//Link constructor
template < typename T >
List<T>::Link::Link (const T& Tval) : Tval_(Tval), prev_(nullptr), next_(nullptr)
{}

template < typename T >
typename List<T>::Link * List<T>::NewLink (const T &t)
{
    Link * newLink = new(std::nothrow) Link(t);
    if (nullptr == newLink) //check if allocation worked
    {
        //exception handler
        std::cerr << "*:ost error: memory allocation failure.\n";
        return nullptr;
    }
    return newLink; //returns the addres of the newly created link
}

// -- LinkIn and LinkOut methods -- need to complete these
template < typename T >
void List<T>::LinkIn(typename List<T>::Link * location, typename List<T>::Link * newLink)
{
    newLink->next_ = location; // set newLink's next pointer to location of node following head
    newLink->prev_ = location->prev; //set newLink's prev prointer to head node
    newLink->next_->prev_ = newLink;
    newLink->prev_->next_ = newLink;
}

template < typename T >
typename List<T>::Link * List<T>::LinkOut(typename List<T>::Link * oldLink)
{
    /***********/
}

//Initialize a ciruclar list; note that head's next_ and prev_ pointers both point to tail
//and vice-versa to fulfull the cirucular idea
template < typename T >
void List<T>::Init()
{
    head_ = newLink(T());
    tail_ = newLink(T());
    head_->next_ = tail_;
    head_->prev_ = tail_;
    tail_->next_ = head_;
    tail_->prev_ = head_;
}

template < typename T >
void List<T>::Init(T fill) //constructor used in development version
{
    head_ = newLink(fill);
    tail_ = newLink(fill);
    head_->next_ = tail_;
    head_->prev_ = tail_;
    tail_->next_ = head_;
    tail_->prev_ = head_;
}

//Append method - works the same as the linear case; appends deep copy of rhs
template < typename T >
void List<T>::Append (const List<T> &list)
{
    for (ConstIterator i = list.Begin(); i != list.End(); ++i)
        PushBack(*i);
}

// - End of private (and protected) methods





// - Constructors and assignment for the lists

//default constructor
template < typename T >
List<T>::List () : head_(nullptr), tail_(nullptr)
{
    Init();
}

//constructor for development version
template < typename T >
List<T>::List (T fill): head_(nullptr), tail_(nullptr)
{
    Init(fill);
}

//destructor
template < typename T >
List<T>::~List ()
{
    Release();
}
/********* Check on this with the circular structure ********/


//copy constructor ********* asked on discussion board
template < typename T >
List<T>::List (const List<T> &x) : head_(nullptr), tail_(nullptr)
{
    //check to see if the list is the development version  ***need to check on this***
    if (x.head_->Tval) //if T value is holding a valid value
    {
        T fillValue = x.head_->Tval;
        Init(fillValue);  //initialize will fill value
    }
    else
        Init(); //initialize normally
    
    Append(x); //append the list to the available nodes
}

//assignment operator
template < typename T >
List<T>& List<T>::operator = (const List<T> &rhs) //return type List<T>& allows for chaining
{
    if (this != &rhs) //first check for self assignment
    {
        Clear(); //destroy the existing list; moves head and tail pointer next to each other
        //note - if copying an empty list, Append() will see that Begin() points to the tail, which
        //would result in the Append not actually pushing anythign to the list
        Append(rhs); //build a copy as *this
    }
    return *this;
}

//Clone operation -- returns pointer to copy of *this
template < typename T >
List<T> * List<T>::Clone() const
{
    List * clone = new List;
    clone->Append(*this);
    return clone;
}

// - End of constructors and assignments




// - General List<T> function

//PushFront operation - inserts t at the front of the list
template < typename T >
bool List<T>::PushFront (const T &t)
{
    //Case 1: There are no excess nodes available
    if (Excess() == 0)
    {
        Link * newLink = NewLink(t); //create a new link; newLink is address of new Link
        if (newLink == nullptr) return 0; //if allocation fails
        LinkIn(head_->next_, newLink); //insert the new link into the list after the head node
        return 1;
    }
    else //Case 2: There are excess nodes available
    {
        head_->Tval_ = t; //set t to the T value contained in current head node
        head_ = head_->prev_; // move head forward one node
    }
}

//PushBack operation - inserts t at the back of the list
template < typename T >
bool List<T>::PushBack (const T &t)
{
    //Case 1: There are no excess nodes available
    if (Excess() == 0)
    {
        Link * newLink = newLink(t);
        if (newLink == nullptr) return 0;
        LinkIn(tail_,newLink);  //insert new link right before tail_ node
        return 1;
    }
    else //Case 2: There are excess nodes available
    {
        tail_->Tval_ = t; //set t to the T value contained in tail node
        tail_ = tail_->next_; //move tail forward 1 one
    }
}

template < typename T >
ListIterator<T> List<T>::Insert (ListIterator<T> i, const T &t)
{
    //Case 1: There are no excess nodes available
    if (Excess() == 0)
    {
        if (Empty()) //always insert if the list is empty
        {
            i = End(); //will set the iterator to "1 past back" ie the tail node
        }
        
        if (!i.Valid() || i == rEnd()) //if the iterator is null or pointing to the head node
        {
            std::cerr << " ** cannot insert at position -1\n";
            return End(); //returns tail position
        }
        Link * newLink = newLink(t);
        if (newLink == nullptr) return 0; //problem with memory allocation
        LinkIn(i.curr_,newLink); //insert link at specified location
        i.curr_ = newLink; //set i to new link location
        return i;
    }
    else //Case 2: There are excess nodes available
    {
        
        
    }
    
}

template < typename T >
ConstListIterator<T> List<T>::Insert (ConstListIterator<T> i, const T &t)
{
    /***********/
}

//inserts at default location
template < typename T >
ListIterator<T> List<T>::Insert (const T &t)
{
    return Insert(End(), t);  //note that this uses const version since End() returns const iterator
}

template < typename T >
bool List<T>::PopFront()
{
    /**********/
}

template < typename T >
bool List<T>::PopBack()
{
    /**********/
}

//Remove an item at i; note this does not delete the actual link as per Circular list implementation plan
template < typename T >
ListIterator<T> List<T>::Remove (ListIterator<T> i)
{
    /*********/
}

//const version of Remove
template < typename T >
ConstListIterator<T> List<T>::Remove (ConstListIterator<T> i)
{
    /*********/
}

//Removes all copies of t, and returns the number of items removed
template < typename T >
size_t List<T>::Remove (const T &t)
{
    /***********/
}

//Clear - makes the list empty  -- note: this does not actually de-allocate the memory; release is needed for that.
template < typename T >
void List<T>::Clear()
{
    while(!Empty())
        PopFront();
}

//Deallocate all memory except for head and tail nodes ************** come back to this to assess ************
// ***********************************************************************************************************
template < typename T >
void List<T>::Release()
{
    Clear(); //clears list, makes head and tail adjacent.
    
    //start iterator at first item after head node, iterate until it hits tail node.
    for (Iterator i = Begin(); i != End(); ++i)
    {
        delete i.curr_; //de-allocate the memory reserved for the current pointed to link
    }
    //finally, delete the head and tail nodes.
    delete head_;
    delete tail_;
} // end release

//Returns the active size of the array
template < typename T >
size_t List<T>::Size() const
{
    size_t size(0); //initialize size variable
    Link * curr(head_->next); //get pointer to link after head node
    while (curr != tail_) //while curr pointer is not at the tail node
    {
        curr = curr -> next_;
        ++size;
    }
    return size;
}

template < typename T >
size_t List<T>::Excess() const
{
    size_t excess(0); //initialize excess variable
    Link * curr(tail_->next); //get pointer to link after tail node
    while (curr != head_) //while curr pointer is not at the head node
    {
        curr = curr -> next;
        ++excess;
    }
    return excess;
}

//Returns total capacity of the ring
template < typename T >
size_t List<T>::Capacity() const
{
    return Size() + Excess();
}

//Determines if the list is empty
template < typename T >
bool List<T>::Empty () const
{
    return head_->next_ == tail_;
}

//Determines if the list is full
template < typename T >
bool List<T>::Full () const
{
    return tail_->next_ == head_;
}

//Access the front element on the list
template < typename T >
T& List<T>::Front ()
{
    if(Empty())
    {
        std::cerr << "** List error: Front() called on empty list\n";
    }
    return *Begin();
}

//Constant version of Front
template < typename T >
const T& List<T>::Front () const
{
    if(Empty())
    {
        std::cerr << "** List error: Front() called on empty list\n";
        }
    return *Begin();
}

//Access the back element on the list
template < typename T >
T& List<T>::Back ()
{
    if (Empty())
    {
        std::cerr << "** List error: Back() called on empty list\n";
    }
    return *rBegin();
}

//Constant version of back
template < typename T >
const T& List<T>::Back () const
{
    if (Empty())
    {
        std::cerr << "** List error: Back() called on empty list\n";
    }
    return *rBegin();
}

// - End General List Functions





// -- Generic Display Methods

// Display the List (with no "dark side" elements or head and tail nodes)
template < typename T >
void List<T>::Display (std::ostream& os, char ofc) const
{
    ConstIterator i; //declare an iterator
    if (ofc == '\0')
        for (i = Begin(); i != End(); ++i)
            os << *i;
    else
        for (i = Begin(); i != End(); ++i)
            os << ofc << *i;
} // end Display

// Display the List in its entire structure including non-active nodes and head and tail nodes
template < typename T >
void List<T>::Dump (std::ostream& os, char ofc) const
{
    ConstIterator i = Begin(); //declear an iterator that points to first element
    if (ofc == '\0')
    {
        do //print entire structure regardless of start condition; list will have at least head and tail nodes
        {
            os << *i;
            ++i; //increment the iterator
        }
        while (i != Begin());
    }
    else
    {
        do //same as above
        {
            os << ofc << *i;
            ++i;
        }
        while (i != Begin());
    }
} // end Dump

// structural integrity check - adopted from list2c.api file
template < typename T >
void List<T>::CheckLinks (std::ostream& os) const
{
    Link * q, * p = head_;
    size_t n = 0;
    do
    {
        q = p->next_->prev_;
        if (p != q)
        {
            os << " ** forward check failure at link " << n << '\n';
        }
        q = p->prev_->next_;
        if (p != q)
        {
            os << " ** reverse check failure at link " << n << '\n';
        }
        p = p->next_;
        ++n;
    }
    while (p != head_);
    if (n != 2 + Size() + Excess())
    {
        os << " ** link count mismatch: count = " << n << " , size = " << Size() << " , excess = " << Excess() << '\n';
    }
}

// -- End Generic Display Methods





//--Iterator support--

//Returns iterator pointed to first link after the head
template < typename T >
ListIterator<T> List<T>::Begin ()
{
    Iterator i(head_->next_);
    return i;
}

//Returns iterator pointed to "1 past the back" ie the tail node
template < typename T >
ListIterator<T> List<T>::End ()
{
    Iterator i(tail_);
    return i;
}

//Returns iterator to the back (last link before tail node)
template < typename T >
ListIterator<T> List<T>::rBegin ()
{
    Iterator i(tail_->prev_);
    return i;
}

//Returns iterator pointing to "1 past the front" ie the head node
template < typename T >
ListIterator<T> List<T>::rEnd ()
{
    Iterator i(head_);
    return i;
}

//Returns position of the first occurrence of t, or End()
template < typename T >
ListIterator<T> List<T>::Includes (const T &t)
{
    //Note - the algorithm uses sequential search
    Iterator i; //declare iterator
    for (i = (*this).begin; i != (*this).end; ++i)
    {
        if (t == *i)
            return i;
    }
    return End();
}

//Returns iterator pointed to first link after the head - constant version
template < typename T >
ConstListIterator<T> List<T>::Begin () const
{
    Iterator i(head_->next_);
    return i;
}

//Returns iterator pointed to "1 past the back" ie the tail node - constant version
template < typename T >
ConstListIterator<T> List<T>::End () const
{
    Iterator i(tail_);
    return i;
}

//Returns iterator to the back (last link before tail node) - constant version
template < typename T >
ConstListIterator<T> List<T>::rBegin () const
{
    Iterator i(tail_->prev_);
    return i;
}

//Returns iterator pointing to "1 past the front" ie the head node - constant version
template < typename T >
ConstListIterator<T> List<T>::rEnd () const
{
    Iterator i(head_);
    return i;
}

//Returns position of the first occurrence of t, or End() - constant version
template < typename T >
ConstListIterator<T> List<T>::Includes (const T &t) const
{
    //Note - the algorithm uses sequential search
    Iterator i; //declare iterator
    for (i = (*this).begin; i != (*this).end; ++i)
    {
        if (t == *i)
            return i;
    }
    return End();
}

// -- End Iterator Support





// -- ConstListIterator<T>::Implementations

// default constructor - can be called by client programs
template < typename T >
ConstListIterator<T>::ConstListIterator() : curr_(nullptr)
{}

// protected constructor - create an iterator around a Link pointer; not available to client programs
template < typename T >
ConstListIterator<T>::ConstListIterator (typename List<T>::Link* link) : curr_(link)
{}

// copy constructor - copies the iterator
template < typename T >
ConstListIterator<T>::ConstListIterator (const ConstListIterator<T>& i)
:  curr_(i.curr_)
{}

// assignment operator
template < typename T >
ConstListIterator<T>& ConstListIterator<T>::operator = (const ConstListIterator <T> & i)
{
    curr_ = i.curr_;  //note - no risk involved in self-assignment
    return *this;
}

// protected method - returns a refernece to the current T value the iterator is pointing to.  Note that this
// method is used by both the const and non const versions.
template < typename T >
T&  ConstListIterator<T>::Retrieve() const
{
    if (curr_ == nullptr)
    {
        std::cerr << "** Error: ConstListIterator<T>::Retrieve() invalid dereference\n";
        exit (EXIT_FAILURE);
    }
    return curr_->Tval_;
}

// determine if the Iterator's pointer is valid or not
template < typename T >
bool ConstListIterator<T>::Valid() const
{
    return curr_ != nullptr;
}

// determine if two iterators are equivalnet; that is, if they're pointing to the same link
template < typename T >
bool ConstListIterator<T>::operator == (const ConstListIterator<T>& i2) const
{
    if (curr_ == i2.curr_)
        return 1;
    return 0;
}

// determine if two iterators are not equivalent
template < typename T >
bool ConstListIterator<T>::operator != (const ConstListIterator<T>& i2) const
{
    return !(*this == i2);
}

// returns dereferneced iterator value (the T value)
template < typename T >
const T&  ConstListIterator<T>::operator * () const
{
    return Retrieve();
}

// prefix increment
template < typename T >
ConstListIterator<T>& ConstListIterator<T>::operator ++ ()
{
    if (curr_ != nullptr)
        curr_ = curr_->next_;
    return *this;
}

// postfix increment
template < typename T >
ConstListIterator<T> ConstListIterator<T>::operator ++ (int)
{
    ConstListIterator <T> clone = *this; //set temporary clone iterator to *this
    this->operator++(); //increment *this
    return clone; //return the clone (contains value before it was incremented)
}

// prefix decrement
template < typename T >
ConstListIterator<T>& ConstListIterator<T>::operator -- ()
{
    if (curr_ != nullptr)
        curr_ = curr_->prev_;
    return *this;
}

// postfix decrement
template < typename T >
ConstListIterator<T> ConstListIterator<T>::operator -- (int)
{
    ConstListIterator <T> clone = *this; //set temporary clone iterator to *this
    this->operator--(); //decrement *this
    return clone; //return the clone (contains value before it was decremented)
}

// -- End ConstListIterator<T> Implementations





// -- ListIterator<T> Implementations

// default constructor -- can be called by client programs; creates a null list iterator
template < typename T >
ListIterator<T>::ListIterator () : ConstListIterator<T>() //inherits from ConstListIterator
{}

// protected constructor - constructs iterator based on pointer to link; not avaialble to client
template < typename T >
ListIterator<T>::ListIterator (typename List<T>::Link * link) : ConstListIterator<T> (link)
{}

// copy constructor
template < typename T >
ListIterator<T>::ListIterator (const ListIterator<T>& i)
:  ConstListIterator<T>(i)
{}

// assignment operator
template < typename T >
ListIterator<T>& ListIterator<T>::operator = (const ListIterator <T> & i)
{
    ConstListIterator<T>::operator=(i);
    return *this;
}

// dereference operator -- uses same retrieve as ConstListIterator, for use on const objects
template < typename T >
const T&  ListIterator<T>::operator * () const
{
    return ConstListIterator<T>::Retrieve();
}

// deference operator for use on non-const data objects (note - can use reference to change value)
template < typename T >
T&  ListIterator<T>::operator * ()
{
    return ConstListIterator<T>::Retrieve();
}

// prefix increment - same as Const
template < typename T >
ListIterator<T>& ListIterator<T>::operator ++ ()
{
    ConstListIterator<T>::operator++();
    return *this;
}

// postfix increment - same as Const
template < typename T >
ListIterator<T> ListIterator<T>::operator ++ (int)
{
    ListIterator <T> clone = *this;
    this->operator++();
    return clone;
}

// prefix decrement - same as Const
template < typename T >
ListIterator<T>& ListIterator<T>::operator -- ()
{
    ConstListIterator<T>::operator--();
    return *this;
}

// postfix decrement - same as Const
template < typename T >
ListIterator<T> ListIterator<T>::operator -- (int)
{
    ListIterator <T> clone = *this;
    this->operator--();
    return clone;
}

// -- End ListIterator<T> Implementations








































