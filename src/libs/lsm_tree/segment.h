#ifndef SNAILDB_LIBS_LSM_TREE_SEGMENT_H_
#define SNAILDB_LIBS_LSM_TREE_SEGMENT_H_

#include <filesystem>
#include <string>
#include <fstream>
#include <optional>
#include <map>
#include <iostream>

#include "cbf.h"

namespace snaildb {

/*
  Represents the contents of a single memtable flush
*/
class Segment {

public:
  Segment(size_t = 10000,size_t = 100,size_t = 100);
  void readFromFile(std::filesystem::path);
  void writeTo(std::filesystem::path, std::map<std::string,std::string>& mem_table);

  std::optional<std::string> findKey(std::string);

private:
  void write(std::map<std::string,std::string>& mem_table);
  std::fstream table_file_; 
  size_t INDEX_THRESHHOLD_; // How many bytes per index entry

  std::map<std::string, size_t> sparse_index_;
  CountingBloomFilter<uint16_t> filter_;

  std::pair<std::string,std::string> nextRecord();
};

} //namespace snaildb

#endif //SNAILDB_LIBS_LSM_TREE_SEGMENT_H_
