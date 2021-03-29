#include <gtest/gtest.h>
#include <rocksdb/db.h>

#include "log.h"

TEST(rocksdb_demo, open) {
  rocksdb::DB* db;
  rocksdb::Options options;
  options.create_if_missing = true;
  rocksdb::Status s1 = rocksdb::DB::Open(options, "./testdb", &db);
  log_info("db open status: {}", s1.ToString()); 
  rocksdb::Status s2 = db->Close();
  log_info("db close status: {}", s2.ToString()); 
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::common::init_log_module(); 
  return RUN_ALL_TESTS();
}
