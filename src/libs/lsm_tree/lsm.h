#ifndef SNAILDB_LIBS_LSM_TREE_LSM_H_
#define SNAILDB_LIBS_LSM_TREE_LSM_H_

#include <map>
#include <set>
#include <string>
#include <fstream>
#include <optional>
#include <vector>

#include "sst.h"

namespace snaildb {

class LsmTree {

public:
  bool open(std::string);
  bool close();

  std::optional<std::string> get(std::string);
  bool put(std::string, std::string);
  bool remove(std::string);

  //WAL 
  //memtable (sorted set)
  //sorted string table (on disk)
  //sparse index?
private:
  std::map<std::string,std::string> mem_table_;
  SST sst_;
  std::ofstream wal_;
  std::string dbName;

  const size_t MEM_TABLE_THRESHOLD_ = 1000;
  size_t key_size_;
  size_t value_size_;

  bool writeToLog(std::string key, std::string value);
  std::map<std::string,std::string> readFromLog(std::fstream& logFile);

};

} //namespace snaildb

#endif //SNAILDB_LIBS_LSM_TREE_LSM_H_
