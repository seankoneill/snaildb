#ifndef SNAILDB_LIBS_LSM_TREE_SST_H_
#define SNAILDB_LIBS_LSM_TREE_SST_H_

#include <string>
#include <fstream>
#include <vector>
#include <optional>
#include <map>
#include <iostream>
#include <filesystem>

#include "segment.h"

namespace snaildb {

/* 
Sorted string table structure backed by
file of segments

Segments are flushed memtables
*/

class SST {
public:
  ~SST();

  void open(std::string db_name);

  void write(std::map<std::string, std::string> mem_table);
  std::optional<std::string> get(std::string key) const;

private:
  std::fstream table_file_; //One file per SST
  std::string db_name_;
  std::vector<std::unique_ptr<Segment>> segments_;

  void readFromFile();
  void compact();
};

} //namespace snaildb

#endif //SNAILDB_LIBS_LSM_TREE_SST_H_
