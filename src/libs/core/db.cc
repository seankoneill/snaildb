#include "db.h"

std::optional<std::string> snaildb::DB::get(std::string key) {
  return {storage_engine_.get(key)};
}

void snaildb::DB::put(std::string key, std::string value) {
  storage_engine_.put(key,value);
}

bool snaildb::DB::remove(std::string key) {
  storage_engine_.remove(key);
  return true;
}
