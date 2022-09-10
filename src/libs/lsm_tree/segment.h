#ifndef SNAILDB_LIBS_LSM_TREE_SEGMENT_H_
#define SNAILDB_LIBS_LSM_TREE_SEGMENT_H_

#include <string>
#include <fstream>
#include <optional>
#include <map>
#include <iostream>

#include "cbf.h"

namespace snaildb {

  class Segment {
    public:
      Segment(std::fstream& tf);
      ~Segment();

      bool write(std::map<std::string,std::string> mem_table);
      std::optional<std::string> get(std::string key);

    private:
      //try adding keys to sparse index randomly for now
      size_t first_record_, last_record_;
      std::map<std::string, size_t> sparse_index_;
      CountingBloomFilter<uint16_t>* filter_;
      std::fstream& table_file_;
      const size_t INDEX_THRESHHOLD_ = 1000; 

      std::string findKey(std::string, size_t offset);
      std::pair<std::string, std::string> readRecord(size_t& offset);

  };

} //namespace snaildb

#endif //SNAILDB_LIBS_LSM_TREE_SEGMENT_H_
