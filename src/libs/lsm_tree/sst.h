#ifndef SNAILDB_LIBS_LSM_TREE_SST_H_
#define SNAILDB_LIBS_LSM_TREE_SST_H_

#include <string>
#include <fstream>
#include <vector>
#include <optional>
#include <map>
#include <iostream>

#include "segment.h"

namespace snaildb {

//sorted string table
class SST {
public:
  ~SST();

  void open(std::string dbName);

  void write(std::map<std::string, std::string> mem_table);
  std::optional<std::string> get(std::string key) const;

private:
  std::fstream table_file_; //One file per SST
  std::vector<Segment*> segments_;

  void compact();

};

} //namespace snaildb

#endif //SNAILDB_LIBS_LSM_TREE_SST_H_
