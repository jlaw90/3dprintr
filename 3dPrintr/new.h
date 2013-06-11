/*
* new.h
*
* Created: 07/06/2013 16:48:57
*  Author: James
*/


#ifndef NEW_H_
#define NEW_H_


// C++ new operator
void * operator new(size_t size);

// C++ delete operator
void operator delete(void * ptr);


#endif /* NEW_H_ */