#include "libs/core/db.h"
#include "libs/lsm_tree/cbf.h"

#include "spdlog/spdlog.h"

int main() {

  //spdlog::set_level(spdlog::level::debug);

  //clear file for testing
  std::ofstream f;
  f.open("testDB.db");
  f.close();

  snaildb::LsmTree engine;
  engine.open("testDB");

  engine.put("sean", "omegalul");
  //engine.put("user1", "{name john, age 69}");
  //engine.put("user2", "{name john, age 69}");

  //engine.put("sean", "omegalul");
  //engine.put("user1", "{name john, age 69}");
  //engine.put("user2", "{name james, age 69}");

  //std::cout << engine.get("sean").value_or("not found") << std::endl;
  //std::cout << engine.get("user1").value_or("not found") << std::endl;
  //std::cout << engine.get("ben").value_or("not found") << std::endl;
  //std::cout << engine.get("user2").value_or("not found") << std::endl;

  return 0;
};

