/*
 * new.cpp
 *
 * Created: 07/06/2013 16:54:41
 *  Author: James
 */ 

 #include <stdlib.h>

 // C++ new operator
 void * operator new(size_t size)
 {
   return malloc(size);
 }

 // C++ delete operator
 void operator delete(void * ptr)
 {
   free(ptr);
 }