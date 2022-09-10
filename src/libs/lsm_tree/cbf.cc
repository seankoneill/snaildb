#include "cbf.h"
#include "hash_util.h"

#include <random>

namespace snaildb {

  template <typename T>
  CountingBloomFilter<T>::CountingBloomFilter(unsigned int rows, unsigned int columns) 
  : rows_(rows), columns_(columns) {
    srand(time(NULL));
    for (int i = 0; i < rows_; ++i) {
      int r = rand();
      hash_functions_.push_back([this,r](const void * data, size_t len){
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
  bool CountingBloomFilter<T>::contains(const void * data, size_t len) {
    for (int i = 0; i < rows_; ++i) {
      size_t col = hash_functions_[i](data,len);
      if (array[(i*columns_) + col] == 0)
        return false;
    }
    return true;
  }

  template <typename T>
  void CountingBloomFilter<T>::display(std::ostream& os) {
    os << std::endl << "BloomFilter" << std::endl;
    for (int i = 0; i < rows_ * columns_; ++i) {
      os << array[i] << ", ";
      if ((i + 1) % columns_ == 0)
        os << std::endl;
    }
  }

  template class CountingBloomFilter<uint8_t>;
  template class CountingBloomFilter<uint16_t>;
  template class CountingBloomFilter<uint32_t>;
  template class CountingBloomFilter<uint64_t>;

} //namespace snaildb

