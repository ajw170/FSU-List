/*
    Andrew J Wood
    FSU ID: ajw14m
    January 11, 2017
 
    Header file for list2c.h

    Definition the alt2c::List<T> API

    Using ringbuffer model for Push/Pop
    
    This file provides the interface for the alt2c::List<T> class.  This class will implement a
    linked list using a circular model with head and tail notes.  Note that deletion of a node
    does not actually delete the node but rather moves the head and tail nodes that they only
    "surround" the active links in the list; the unused node is then moved to the tail.

    The only time that new links are added to the list is if the ring is full; that is, there
    is no excess link as defined by the Excess() method.
 
    Note the following (from assignment requirements API):
 
    - Push Front/Back retreats/advances the head/tail pointer when there are unused nodes
    - Pop Front/Back advances/retreats the head/tail pointer
    - Insert(i,t) uses tail_ link if available
    - Remove(i) stores old link at tail_
*/

#ifndef _LIST_H
#define _LIST_H

#include <iostream>    // class ostream and objects cerr, cout
#include <cstdlib>     // EXIT_FAILURE, size_t
#include <compare.h>   // needed for Sort()

namespace alt2c //required to resolve the type of list being used, as opposed to fsu::List
{

  template < typename T >
  class List;

  template < typename T >
  class ConstListIterator;

  template < typename T >
  class ListIterator;

  //----------------------------------
  //     List<T>
  //----------------------------------

  template < typename T >
  class List
  {
  public:
    //definition support for clients; clients can simply type List<T>::Iterator to get
    //an iterator object appropriate for the list.
    typedef T                                      ValueType;
    typedef ListIterator < T >                     Iterator;
    typedef ConstListIterator < T >                ConstIterator;
    //for adapters, could say
    // typedef ConstRAIterator< ListIterator<T> >   ConstIterator;
      // friend class ConstRAIterator

    // constructors and assignment
                   List       ();              // default constructor
                   List       (T fill);        // development constructor
    virtual        ~List      ();              // destructor
                   List       (const List& );  // copy constructor
    List&          operator = (const List& );  // assignment
    virtual List * Clone      () const;        // returns ptr to deep copy of this list [13]

    // modifying List structure - mutators
    bool      PushFront  (const T& t);   // Insert t at front of list
    bool      PushBack   (const T& t);   // Insert t at back of list
    Iterator  Insert     (Iterator i, const T& t);  // Insert t at i
    ConstIterator  Insert     (ConstIterator i, const T& t);  // ConstIterator version
    Iterator  Insert     (const T& t);   // Insert t
    List&     operator+= (const List& list); // append list

    bool      PopFront  ();              // Remove the Tval at front
    bool      PopBack   ();              // Remove the Tval at back 
    Iterator  Remove    (Iterator i);    // Remove item at I
    ConstIterator  Remove    (ConstIterator i);    // ConstIterator version
    size_t    Remove    (const T& t);    // Remove all copies of t
    void      Clear     ();              // make list empty
    void      Release   ();              // de-allocate all memory except head and tail nodes

    // macroscopic (whole list) mutators
    void      Sort      ();               // default order <
    void      Merge     (List<T>& list);  // merges "list" into this list
    void      Reverse   ();
    void      Shuffle   ();

    template < class Predicate > // Predicate object used to determine order
    void      Sort      (Predicate& p);

    template < class Predicate > // Predicate object used to determine order
    void      Merge     (List<T>& list, Predicate& p);

    // information about the list - accessors [15]
    size_t    Size     () const;  // return the number of elements on the list
    size_t    Excess   () const;  // return the number of unused nodes in the ring
    size_t    Capacity () const;  // return the number of usable nodes in the ring
    bool      Empty    () const;
    bool      Full     () const;

    // accessing values on the list - more accessors
    T&        Front ();        // return reference to Tval at front of list
    const T&  Front () const;  // const version
    T&        Back  ();        // return reference to Tval at back of list
    const T&  Back  () const;  // const version
      
    // generic display methods
    void Display (std::ostream& os, char ofc = '\0') const;
    void Dump    (std::ostream& os, char ofc = '\0') const;
      
    // structural integrity check
    void CheckLinks(std::ostream& os = std::cout) const;

    // Iterator support - locating places on the list
    Iterator  Begin      (); // return iterator to front
    Iterator  End        (); // return iterator "1 past the back"
    Iterator  rBegin     (); // return iterator to back
    Iterator  rEnd       (); // return iterator "1 past the front" in reverse
    Iterator  Includes   (const T& t); // returns position of first occurrence of t, or End()

    // ConstIterator support
    ConstIterator  Begin      () const;
    ConstIterator  End        () const;
    ConstIterator  rBegin     () const;
    ConstIterator  rEnd       () const;
    ConstIterator  Includes   (const T& t) const;

   
  protected:
    // A scope List<T>:: class usable only by its friends (all members are private)
    class Link
    {
      friend class List<T>;
      friend class ConstListIterator<T>;
      friend class ListIterator<T>;

      // Link variables
      T       Tval_;        // data
      Link *  prev_;        // ptr to predecessor Link
      Link *  next_;        // ptr to successor Link

      // Link constructor - parameter required
      Link(const T& );
    } ; // end class list

    Link *  head_,  // node representing "one before the first"
         *  tail_;  // node representing "one past the last"

    // protected methods -- used only by other methods
    void Init   ();                 // sets up head and tail nodes
    void Init   (T fill);           // development version - makes links visible under Dump
    void Append (const List& list); // append deep copy of list

    // protected method isolates memory allocation and associated exception handling
    static Link * NewLink (const T&);

    // standard link-in and link-out processes
    static void   LinkIn  (Link * location, Link * newLink);
    static Link * LinkOut (Link * oldLink);

    // tight couplings
    friend class ListIterator<T>;
    friend class ConstListIterator<T>; // [1]
    // numbers in square brackets refer to notes at the end of this file
  } ; // end class List<T>

  // global scope operators and functions; this is required for output using cout
  // and also to ensure that lists may occur on either side of the equal sign.

  template <typename T>
  bool operator == (const List<T>& list1, const List<T>& list2);

  template <typename T>
  bool operator != (const List<T>& list1, const List<T>& list2);

  template <typename T>
  std::ostream& operator << (std::ostream& os, const List<T>& list);

  //----------------------------------
  //     ConstListIterator<T>
  //----------------------------------

  template <typename T>
  class ConstListIterator
  {
  public:
    // terminology support
    typedef T                                      ValueType;
    typedef ConstListIterator < T >                ConstIterator;
    typedef ListIterator < T >                     Iterator;

    // constructors
    ConstListIterator       ();                          // default constructor
    ConstListIterator       (const ConstListIterator& ); // copy constructor

    // information/access
    bool Valid         () const;  // cursor is valid element

    // various operators
    bool            operator == (const ConstIterator& i2) const;
    bool            operator != (const ConstIterator& i2) const;
    const T&        operator *  () const; // Return const reference to current Tval
    ConstIterator&  operator =  (const ConstIterator& i);
    ConstIterator&  operator ++ ();    // prefix
    ConstIterator   operator ++ (int); // postfix
    ConstIterator&  operator -- ();    // prefix
    ConstIterator   operator -- (int); // postfix

  protected:
    // data
    typename List<T>::Link * curr_;

    // methods
    ConstListIterator (typename List<T>::Link * linkPtr); // type converting constructor
    T& Retrieve () const; // conflicted Retrieve used by both versions of operator*

    // tight couplings
    friend class List<T>;
  } ;  //end class ConstListIterator<T>

  //----------------------------------
  //     ListIterator<T>
  //----------------------------------

  template <typename T>
  class ListIterator : public ConstListIterator<T>
  {
  public:
    // terminology support
    typedef T                                      ValueType;
    typedef ConstListIterator < T >                ConstIterator;
    typedef ListIterator < T >                     Iterator;

    // constructors
    ListIterator       ();                       // default constructor
    ListIterator       (List<T>& );              // initializing constructor
    ListIterator       (const ListIterator& );   // copy constructor

    // information/access
    // bool Valid         () const;

    // various operators
    // bool       operator == (const Iterator& i2) const;
    // bool       operator != (const Iterator& i2) const;
    T&         operator *  ();       // Return reference to current Tval
    const T&   operator *  () const; // const version
    Iterator&  operator =  (const Iterator& i);
    Iterator&  operator ++ ();    // prefix
    Iterator   operator ++ (int); // postfix
    Iterator&  operator -- ();    // prefix
    Iterator   operator -- (int); // postfix

  protected:
    // methods
    ListIterator (typename List<T>::Link * linkPtr); // type converting constructor
    // T& Retrieve () const; // conflicted Retrieve used by both versions of operator*

    // tight couplings
    friend class List<T>;
  } ;  //end class ListIterator : public ConstListIterator<T>


//slave files
#include <list2c.cpp>
#include <list2c_macro.cpp>
#include <list_sort.cpp>

    
} // namespace alt2c


#endif

