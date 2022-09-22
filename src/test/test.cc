#include <map>
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
  std::string dbName = "freshDBTest.db";
  snaildb::LsmTree engine;
  SECTION("Init new db") {
    std::ofstream f;
    f.open(dbName);
    f.close();

    engine.open(dbName);
  }

  std::map<std::string, std::string> testMap;
  std::vector<std::string> testKeys;
  std::vector<std::string> testVals;
  SECTION("Adding random pairs") {
    for (int i = 0; i < 10000; ++i) {
      testKeys.push_back(gen_random(rand()));
      testVals.push_back(gen_random(rand()));

      engine.put(testKeys[i],testVals[i]);
      testMap[testKeys[i]] = testVals[i];
    }
  }

  for (auto e: testKeys) {
    REQUIRE(engine.get(e).value() == testMap[e]);
  }
}

TEST_CASE("Correct returns on existing database") {
  //clear file before test
  std::ofstream f;
  f.open("testDB.db");
  f.close();

  snaildb::LsmTree engine;
  engine.open("testDB.db");

  std::map<std::string, std::string> testMap;
  std::vector<std::string> testKeys;
  std::vector<std::string> testVals;
  for (int i = 0; i < 10000; ++i) {
    testKeys.push_back(gen_random(20));
    testVals.push_back(gen_random(100));

    engine.put(testKeys[i],testVals[i]);
    testMap[testKeys[i]] = testVals[i];
  }

  engine.close();
  engine.open("testDB.db");

  for (auto& e: testKeys) {
    REQUIRE(engine.get(e).value() == testMap[e]);
  }
}

TEST_CASE("Recover database from log after crash") {}
TEST_CASE("Test integrity after compaction") {}
