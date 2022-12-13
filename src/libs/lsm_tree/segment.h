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
  Segment(std::fstream&,std::map<std::string,std::string>&);
  Segment(std::fstream&,size_t offset);

  std::optional<std::string> findKey(std::string) const;

private:
  Segment(std::fstream&);

  size_t start_offset_, end_offset_;
  std::fstream& table_file_; //Segments only refer to existing table file
  const size_t INDEX_THRESHHOLD_ = 1000; // How many bytes per index entry


  std::map<std::string, size_t> sparse_index_;
  CountingBloomFilter<uint16_t> filter_{100,100};

  void write(std::map<std::string,std::string>& mem_table);
  void read(size_t offset);
  std::pair<std::string,std::string> readRecord(size_t) const;
};

} //namespace snaildb

#endif //SNAILDB_LIBS_LSM_TREE_SEGMENT_H_
