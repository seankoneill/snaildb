#include <map>
#include <spdlog/spdlog.h>
#include <vector>
#include <fstream>

#include <catch2/catch_test_macros.hpp>

#include "../libs/core/db.h"

//Taken from stack overflow: https://stackoverflow.com/a/440240
std::string gen_random(const int len) {
  static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
  size_t s = sizeof(alphanum);
  std::string tmp_s;
  tmp_s.reserve(len);

  for (int i = 0; i < len; ++i) {
    tmp_s += alphanum[rand() % (s - 1)];
  }

  return tmp_s;
};

TEST_CASE("Correct returns on fresh database") {
  spdlog::set_level(spdlog::level::debug);
  std::string db_name = "freshDBTest";
  std::string db_path = std::filesystem::current_path().string();

  std::filesystem::remove(db_path + "/" + db_name + ".db");

  snaildb::LsmTree engine;
  engine.open(db_path, db_name);

  std::map<std::string, std::string> test_map;
  std::vector<std::string> test_keys;
  std::vector<std::string> test_vals;
  for (int i = 0; i < 10; ++i) {
    test_keys.push_back(gen_random((rand() % 5) + 5));
    test_vals.push_back(gen_random((rand() % 5) + 5));

    engine.put(test_keys[i],test_vals[i]);
    test_map[test_keys[i]] = test_vals[i];
  }

  engine.flush();

  for (auto& e: test_keys) {
    REQUIRE(engine.get(e).value() == test_map[e]);
  }
}

TEST_CASE("Correct returns on existing database") {
  spdlog::set_level(spdlog::level::debug);
  std::string db_name = "existingDBTest";
  std::string db_path = std::filesystem::current_path().string();

  std::filesystem::remove(db_path + "/" + db_name + ".db");

  snaildb::LsmTree engine;
  engine.open(db_path, db_name);

  std::map<std::string, std::string> test_map;
  std::vector<std::string> test_keys;
  std::vector<std::string> test_vals;
  for (int i = 0; i < 1000; ++i) {
    test_keys.push_back(gen_random((rand() % 5) + 5));
    test_vals.push_back(gen_random((rand() % 5) + 5));

    engine.put(test_keys[i],test_vals[i]);
    test_map[test_keys[i]] = test_vals[i];
  }

  engine.close();
  engine.open(db_path, db_name);

  for (auto& e: test_keys) {
    REQUIRE(engine.get(e).value() == test_map[e]);
  }
}

//TEST_CASE("Recover database from log after crash") {}
//TEST_CASE("Test integrity after compaction") {}
