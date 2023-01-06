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
segment files

Manages seg files / directory and performes
compaction
*/

class SST {
public:
  ~SST();

  void open(std::filesystem::path dir);
  void close();

  void write(std::map<std::string, std::string> mem_table);
  std::optional<std::string> get(std::string key) const;

private:
  uint32_t next_id_;
  std::filesystem::path db_directory_;
  std::vector<std::unique_ptr<Segment>> segments_;

  std::filesystem::path nextSegmentPath();
  void readFromFile();
  void compact();
};

} //namespace snaildb

#endif //SNAILDB_LIBS_LSM_TREE_SST_H_
