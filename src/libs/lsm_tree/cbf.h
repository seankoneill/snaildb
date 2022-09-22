#ifndef SNAILDB_LIBS_LSM_TREE_CBF_H_
#define SNAILDB_LIBS_LSM_TREE_CBF_H_

#include <map>
#include <set>
#include <string>
#include <functional>
#include <iostream>
#include <vector>
#include <random>

#include "../util/hash_util.h"

namespace snaildb {

template <typename CountType>
class CountingBloomFilter {
  unsigned int rows_;
  unsigned int columns_;
  unsigned char seed;
  std::vector<std::function<unsigned int(const void *, size_t)>> hash_functions_;

  CountType* array;

public:
  CountingBloomFilter(unsigned int rows, unsigned int columns);
  ~CountingBloomFilter();
  void add(const void * data, size_t len);
  bool remove(const void * data, size_t len);
  bool contains(const void * data, size_t len) const;
  void display(std::ostream&) const;

};

template <typename T>
CountingBloomFilter<T>::CountingBloomFilter(unsigned int rows, unsigned int columns) 
: rows_(rows), columns_(columns) {
  srand(time(NULL));
  for (int i = 0; i < rows_; ++i) {
    int r = rand();
    hash_functions_.push_back(
        [this,r] (const void * data, size_t len) {
          return HashUtil::MurmurHash2(data,len,r) % columns_;
        });
  }
  array = new T[rows_ * columns_];
}

template <typename T>
CountingBloomFilter<T>::~CountingBloomFilter() {
  delete array;
}

template <typename T>
void CountingBloomFilter<T>::add(const void * data, size_t len) {
  for (int i = 0; i < rows_; ++i) {
    size_t col = hash_functions_[i](data,len);
    array[(i*columns_) + col] += 1;
  }
}

template <typename T>
bool CountingBloomFilter<T>::remove(const void * data, size_t len) {
  if (!contains(data,len)) { return false; }
  for (int i = 0; i < rows_; ++i) {
    size_t col = hash_functions_[i](data,len);
    array[(i*columns_) + col] -= 1;
  }
  return true;
}

template <typename T>
bool CountingBloomFilter<T>::contains(const void * data, size_t len) const {
  for (int i = 0; i < rows_; ++i) {
    size_t col = hash_functions_[i](data,len);
    if (array[(i*columns_) + col] == 0)
      return false;
  }
  return true;
}

template <typename T>
void CountingBloomFilter<T>::display(std::ostream& os) const {
  os << std::endl << "BloomFilter" << std::endl;
  for (int i = 0; i < rows_ * columns_; ++i) {
    os << array[i] << ", ";
    if ((i + 1) % columns_ == 0)
      os << std::endl;
  }
}

} //namespace snaildb

#endif //SNAILDB_LIBS_LSM_TREE_CBF_H_
