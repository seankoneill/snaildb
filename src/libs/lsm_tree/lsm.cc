#include "lsm.h"

#include <spdlog/spdlog.h>
#include <sstream>

namespace snaildb {

void LsmTree::open(std::filesystem::path path,std::string db_name) {
  spdlog::debug("Opening LsmTree at path: {}/{}",path.string(),db_name);
  sst_.open(path.string());
  wal_file_.open(path.string() + "/" + db_name + ".wal");
}

void LsmTree::close() {
  flush();
  wal_file_.close();
  sst_.close();
}

std::optional<std::string> LsmTree::get(std::string key) const {
  //if key is in current memtable, retrieve it
  if (mem_table_.contains(key))
    return mem_table_.at(key);

  //else get from on-disk SST
  return sst_.get(key);
}

void LsmTree::flush() {
  sst_.write(mem_table_);
  mem_table_.clear();
  wal_file_.open(db_path_.string() + "/" + db_name_ + ".wal");
}

void LsmTree::put(std::string key, std::string value) {
  mem_table_[key] = value;
  writeToLog("PUT",key,value);
  if (mem_table_.size() > MEM_TABLE_THRESHOLD_) flush();
}

void LsmTree::writeToLog(std::string op, std::string key, std::string value) {
  wal_file_ << op << key << value << std::endl;
}

bool LsmTree::remove(std::string key) {
  if (!mem_table_.contains(key)) return false;
  mem_table_.erase(key);
  writeToLog("REMOVE",key,"");
  return true;
}

}
