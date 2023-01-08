#include <map>
#include <spdlog/spdlog.h>
#include <vector>
#include <fstream>

#include <catch2/catch_test_macros.hpp>

#include "../libs/core/db.h"
#include "test_util.h"

TEST_CASE("Correct returns on existing database") {
  //spdlog::set_level(spdlog::level::debug);
  std::string db_name = "existingDBTest";
  std::string db_path = std::filesystem::current_path().string();

  std::filesystem::remove(db_path + "/" + db_name + ".db");

  snaildb::LsmTree engine;
  engine.open(db_path, db_name);

  std::map<std::string, std::string> test_map;
  std::vector<std::string> test_keys;
  std::vector<std::string> test_vals;
  for (int i = 0; i < 100; ++i) {
    std::string k = snaildb::TestUtil::random_string(10);
    std::string v = snaildb::TestUtil::random_string(10);
    test_keys.push_back(k);
    test_vals.push_back(v);

    spdlog::debug("Test pair {}: {},{}",i,k,v);
    engine.put(test_keys[i],test_vals[i]);
    test_map[test_keys[i]] = test_vals[i];
  }

  engine.close();
  engine.open(db_path, db_name);

  for (auto& e: test_keys) {
    REQUIRE(engine.get(e).value() == test_map[e]);
  }
}

TEST_CASE("Correct returns on fresh database") {
  //spdlog::set_level(spdlog::level::debug);
  std::string db_name = "freshDBTest";
  std::string db_path = std::filesystem::current_path().string();

  std::filesystem::remove(db_path + "/" + db_name + ".db");

  snaildb::LsmTree engine;
  engine.open(db_path, db_name);

  std::map<std::string, std::string> test_map;
  std::vector<std::string> test_keys;
  std::vector<std::string> test_vals;
  for (int i = 0; i < 100; ++i) {
    test_keys.push_back(snaildb::TestUtil::random_string(5));
    test_vals.push_back(snaildb::TestUtil::random_string(10));

    engine.put(test_keys[i],test_vals[i]);
    test_map[test_keys[i]] = test_vals[i];
  }

  engine.flush();

  for (auto& e: test_keys) {
    REQUIRE(engine.get(e).value() == test_map[e]);
  }
}

//TEST_CASE("Recover database from log after crash") {}
//TEST_CASE("Test integrity after compaction") {}
