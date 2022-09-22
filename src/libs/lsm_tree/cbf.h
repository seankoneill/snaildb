#ifndef SNAILDB_LIBS_LSM_TREE_CBF_H_
#define SNAILDB_LIBS_LSM_TREE_CBF_H_

#include <map>
#include <set>
#include <string>
#include <functional>
#include <iostream>
#include <vector>

namespace snaildb {

template <typename CountType>
class CountingBloomFilter {
  unsigned int rows_;
  unsigned int columns_;
  unsigned char seed;
  std::vector<
  std::function<unsigned int(const void *, size_t)>> hash_functions_;

  CountType* array;

public:
  CountingBloomFilter(unsigned int rows, unsigned int columns);
  ~CountingBloomFilter();
  void add(const void * data, size_t len);
  bool remove(const void * data, size_t len);
  bool contains(const void * data, size_t len);
  void display(std::ostream&);

};

} //namespace snaildb


#endif //SNAILDB_LIBS_LSM_TREE_CBF_H_
