#include "lsm.h"

#include <sstream>

namespace snaildb {

void LsmTree::open(std::string name) {
  std::stringstream ss;

  ss << dbName << ".db";
  sst_.open(ss.str());

  ss.str("");

  ss << dbName << ".wlog";
  wal_.open(ss.str());
}

void LsmTree::close() {
  wal_.close();
}

std::optional<std::string> LsmTree::get(std::string key) const {
  //if key is in current memtable, retrieve it
  if (mem_table_.count(key))
    return mem_table_.at(key);

  //else get from on-disk SST
  return sst_.get(key);
}

void LsmTree::put(std::string key, std::string value) {
  writeToLog(key,value);
  mem_table_[key] = value;

  if (mem_table_.size() > MEM_TABLE_THRESHOLD_) {
    sst_.write(mem_table_);
    mem_table_.clear();
    wal_.open(dbName);
  }
}

void LsmTree::writeToLog(std::string key, std::string value) {
  wal_.write(key.c_str(), key.size());
  wal_.write(":",1);
  wal_.write(value.c_str(), value.size());
  wal_.write("\n", 1);
  wal_.flush();
}

bool LsmTree::remove(std::string key) {
  //need to add tombstone to mem table
  mem_table_[key] = "";
  return true;
}

}
