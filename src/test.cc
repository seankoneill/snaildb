#include <map>
#include <vector>
#include <fstream>

#include <catch2/catch_test_macros.hpp>

#include "libs/core/db.h"

//Taken from stack overflow: https://stackoverflow.com/a/440240
std::string gen_random(const int len) {
  static const char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
  std::string tmp_s;
  tmp_s.reserve(len);

  for (int i = 0; i < len; ++i) {
    tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  return tmp_s;
};

TEST_CASE("Random string test against std::map") {
  //clear file before test
  std::ofstream f;
  f.open("testDB.db");
  f.close();

  snaildb::LsmTree engine("testDB");

  std::map<std::string, std::string> testMap;
  std::vector<std::string> testKeys;
  std::vector<std::string> testVals;
  for (int i = 0; i < 10000; ++i) {
    testKeys.push_back(gen_random(20));
    testVals.push_back(gen_random(100));

    engine.put(testKeys[i],testVals[i]);
    testMap[testKeys[i]] = testVals[i];
  }

  for (auto e: testKeys) {
    REQUIRE(engine.get(e).value() == testMap[e]);
  }
}

TEST_CASE("Open existing database") {}
TEST_CASE("Recover database from log after crash") {}
TEST_CASE("Test integrity after compaction") {}
