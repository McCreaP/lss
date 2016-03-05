#ifndef COLLECTION_H_
#define COLLECTION_H_

#include <iterator>
#include <vector>

template<class T>
class Collection {
 public:
  class Iterator;

  Iterator begin() const;
  Iterator end() const;
};

template<class T>
class Collection::Iterator : public std::iterator<std::forward_iterator_tag, T> {};

#endif  // COLLECTION_H_
