#ifndef SNAILDB_LIBS_LSM_TREE_SEGMENT_H_
#define SNAILDB_LIBS_LSM_TREE_SEGMENT_H_

#include <string>
#include <fstream>
#include <optional>
#include <map>
#include <iostream>

#include "cbf.h"
#include "record.h"

namespace snaildb {

/*
  Represents the contents of a single memtable flush
*/
class Segment {
public:
  Segment() = delete;
  Segment(std::fstream&);
  void write(std::map<std::string,std::string>& mem_table);
  void readFromFile(size_t& offset);

  std::optional<std::string> findKey(std::string) const;

  size_t getStartOffset() { return start_offset_; }
  size_t getEndOffset() { return end_offset_; }
  void setIndexThreshold(size_t t) { INDEX_THRESHHOLD_ = t; }

private:
  size_t start_offset_, end_offset_;
  std::fstream& table_file_; //Segments only refer to existing table file
  size_t INDEX_THRESHHOLD_ = 10; // How many bytes per index entry

  std::map<std::string, size_t> sparse_index_;
  CountingBloomFilter<uint16_t> filter_{100,100};

  std::pair<std::string,std::string> readRecord(size_t) const;
};

} //namespace snaildb

#endif //SNAILDB_LIBS_LSM_TREE_SEGMENT_H_
