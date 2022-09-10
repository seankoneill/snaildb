#include "lsm.h"

#include <sstream>

namespace snaildb {

  LsmTree::LsmTree(std::string name) : dbName(name) {
    std::stringstream ss;

    ss << dbName << ".db";
    sst_.open(ss.str());

    ss.str("");

    ss << dbName << ".wlog";
    wal_.open(ss.str());
  }

  LsmTree::~LsmTree() {
    wal_.close();
  }

  std::optional<std::string> LsmTree::get(std::string key) {
    //if key is in current memtable, retrieve it
    if (mem_table_.count(key))
      return mem_table_[key];

    //else get from on-disk SST
    return sst_.get(key);
  }

  bool LsmTree::put(std::string key, std::string value) {
    writeToLog(key,value);
    mem_table_[key] = value;

    if (mem_table_.size() > MEM_TABLE_THRESHOLD_) {
      if (sst_.write(mem_table_)) {
        //memtable is written to new segment successfully
        //clear table and log
        mem_table_.clear();
        wal_.open(dbName);
      }
    }

    return true;
  }

  bool LsmTree::writeToLog(std::string key, std::string value) {
    wal_.write(key.c_str(), key.size());
    wal_.write(":",1);
    wal_.write(value.c_str(), value.size());
    wal_.write("\n", 1);
    wal_.flush();
    return true;
  }

  bool LsmTree::remove(std::string key) {
    //need to add tombstone to mem table
    mem_table_[key] = "";
    return true;
  }

}
