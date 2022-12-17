#include <map>
#include <spdlog/spdlog.h>
#include <vector>
#include <fstream>

#include <catch2/catch_test_macros.hpp>

#include "../libs/core/db.h"

std::string random_string( size_t length )
{
  auto randchar = []() -> char
    {
      const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
      const size_t max_index = (sizeof(charset) - 1);
      std::random_device rd;  //Will be used to obtain a seed for the random number engine
      std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
      std::uniform_int_distribution<> distrib(0, max_index - 1);
      //int r = rand();                      // SOMEHOW KEPT GIVING SAME SEQUENCE????
      //std::cout << r << std::endl;
      //return charset[ r % max_index ];
      return charset[ distrib(gen) ];
    };
  std::string str(length,0);
  std::generate_n( str.begin(), length, randchar );
  return str;
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
    std::string k = random_string(10);
    std::string v = random_string(10);
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
  spdlog::set_level(spdlog::level::debug);
  std::string db_name = "freshDBTest";
  std::string db_path = std::filesystem::current_path().string();

  std::filesystem::remove(db_path + "/" + db_name + ".db");

  snaildb::LsmTree engine;
  engine.open(db_path, db_name);

  std::map<std::string, std::string> test_map;
  std::vector<std::string> test_keys;
  std::vector<std::string> test_vals;
  for (int i = 0; i < 500; ++i) {
    test_keys.push_back(random_string(5));
    test_vals.push_back(random_string(10));

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
