#ifndef SNAILDB_LIBS_LSM_TREE_LSM_H_
#define SNAILDB_LIBS_LSM_TREE_LSM_H_

#include <map>
#include <string>
#include <fstream>
#include <optional>

#include "sst.h"

namespace snaildb {

/*
  Log-Structured Merge Tree

  Contains MemTable for in memory data
  Sorted String Table for on-disk lookup
  Write-Ahead Log for crash recovery

  // This is the level of the heirarchy where
     type conversion should occur. 

     Currenty: strings -> as strings
     Future:   anything -> as bytes
*/
class LsmTree {

public:
  void open(std::filesystem::path,std::string dbName);
  void close();

  std::optional<std::string> get(std::string) const;
  void put(std::string, std::string);
  bool remove(std::string);

  void flush();

private:
  std::map<std::string,std::string> mem_table_;
  std::fstream wal_;
  std::string db_name_;
  std::filesystem::path db_path_;

  SST sst_;

  const size_t MEM_TABLE_THRESHOLD_ = 1000;

  void writeToLog(std::string operation, std::string key, std::string value);
  std::map<std::string,std::string> readFromLog(std::fstream&);
};

} //namespace snaildb

#endif //SNAILDB_LIBS_LSM_TREE_LSM_H_
