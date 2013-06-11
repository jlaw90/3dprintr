/*
* list.h
*
* Created: 06/06/2013 19:19:43
* Author: James
*/
#ifndef __LIST_H__
#define __LIST_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../new.h"


template <class T> class List{
  public:
  T **list; // The list
  protected:
  private:
  uint16_t capacity; // capacity
  uint16_t size; // size

  public:
  List(int capacity = 4) {
    list = NULL; // set to null initially so the call to realloc acts like malloc
    resize_to(capacity);
  }

  uint16_t length() {
    return size;
  }

  void add(T *t) {
    assert(t != NULL);
    ensure_capacity(size+1);
    list[size++] = t;
  }

  void remove(uint16_t idx) {
    memcpy(&list[idx], &list[idx+1], sizeof(T*) * (size - idx));
    size--;
    list[size] = NULL;
  }

  T * operator[](const uint16_t index) {
    ensure_capacity(index+1);
    return list[index];
  }

  void empty() {
    for(uint16_t i = 0; i < capacity; i++)
    list[i] = NULL;
  }

  ~List() {
    free(list);
  }

  protected:
  private:
  List( const List &c );
  List& operator=( const List &c );

  void resize_to(uint16_t ca) {
    capacity = ca;
    list = (T**) realloc(list, ca * sizeof(T*));
    assert(list != NULL);
    for(uint16_t i = size; i < ca; i++)
    list[i] = NULL;
  }

  void ensure_capacity(uint16_t idx) {
    if(idx >= capacity) {
      while(idx >= capacity)
      capacity = (capacity - 1) << 1;
      resize_to(capacity);
    }
  }
}; //List

#endif //__LIST_H__
