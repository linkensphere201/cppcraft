#include <gtest/gtest.h>
#include <rocksdb/db.h>
#include <rocksdb/slice_transform.h>
#include <sstream>

#include "log.h"

#define DBPATH "./testdb"
#define kBacktraceMaxDepth 10

TEST(rocksdb_demo, open) {
    rocksdb::DB *db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status s1 = rocksdb::DB::Open(options, DBPATH, &db);
    log_info("db open status: {}", s1.ToString());
    rocksdb::Status s2 = db->Close();
    log_info("db close status: {}", s2.ToString());
}

#include <cxxabi.h> // for __cxa_demangle
#include <dlfcn.h>  // for dladdr
#include <execinfo.h>
void DoLogBackTrace(const std::string &pref) {
    std::stringstream s;

    void *buffer[kBacktraceMaxDepth];
    char **strings;

    int nptrs = backtrace(buffer, kBacktraceMaxDepth);
    strings = backtrace_symbols(buffer, nptrs);
    s << "\n";
    if (strings != nullptr) {
        for (int i = 0; i < nptrs; i++) {
            Dl_info info;
            if (dladdr(buffer[i], &info)) {
                char *demangled = NULL;
                int status;
                demangled =
                    abi::__cxa_demangle(info.dli_sname, NULL, 0, &status);
                s << "  f-" << (nptrs - i) << " : " << demangled << "\n";
                free(demangled);
            } else {
                s << strings[i] << "\n";
            }
        }
        log_info("---- {} ---- ", pref);
        log_info("{}", s.str());
    }
    free(strings);
}

using namespace rocksdb;

class MyCustomPrefix : public rocksdb::SliceTransform {
  public:
    MyCustomPrefix(int len, bool prefix_or_suffix)
        : len_(len), isprefix_(prefix_or_suffix) {}
    virtual const char *Name() const { return "MyCustomPrefixExtractor"; }
    virtual Slice Transform(const Slice &key) const {
        auto a = rocksdb::Slice(key.data(), len_);
        // log_info("extracting prefix: origin: {}, prefix: {}", key.ToString(),
        //          a.ToString());
        // DoLogBackTrace("who-call-Transform");
        return a;
    }
    virtual bool InDomain(const Slice &key) const { return true; }
    int len_;
    bool isprefix_;
};

#include <rocksdb/filter_policy.h>
#include <rocksdb/table.h>

void make_rocksdb_options(rocksdb::Options &opts) {
    opts.write_buffer_size = 64 << 20; // a memtable size limit, 10 mb
    opts.level0_file_num_compaction_trigger =
        1; // how many sst for l0 to trigger compaction
    opts.target_file_size_base = 64 << 20; // size of a sst, 10mb

    rocksdb::BlockBasedTableOptions bbtOpts;
    bbtOpts.filter_policy.reset(rocksdb::NewBloomFilterPolicy(10, false));
    bbtOpts.whole_key_filtering = false;
    opts.table_factory.reset(rocksdb::NewBlockBasedTableFactory(bbtOpts));
}

#include <ctime>
#include <unistd.h>
std::string gen_random(const int len) {

    std::string tmp_s;
    static const char alphanum[] = "0123456789"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";

    srand((unsigned)time(NULL) * getpid());

    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i)
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];

    return tmp_s;
}

TEST(rocksdb_demo, make_data) {
    rocksdb::DB *db;
    rocksdb::Options options;
    options.prefix_extractor.reset(new MyCustomPrefix(6, true));

    make_rocksdb_options(options);
    options.create_if_missing = true;
    rocksdb::Status s1 = rocksdb::DB::Open(options, DBPATH, &db);
    log_info("db open status: {}", s1.ToString());

    std::string s = "aaaaaa", v = "b";
    int i = 0;
    while (1) {
        db->Put(WriteOptions(), std::string("aaaaaa") + s + std::to_string(i),
                v + "valuexxx98djkxxxxx1");
        if (i % 50000 == 0) {
            log_info("put 50000 keys!");
            s = gen_random(50);
            v = gen_random(100);
        }
        i++;
    }

    rocksdb::Status s2 = db->Close();
    log_info("db close status: {}", s2.ToString());
}

TEST(rocksdb_demo, write_key) {
    rocksdb::DB *db;
    rocksdb::Options options;
    options.prefix_extractor.reset(new MyCustomPrefix(6, true));

    make_rocksdb_options(options);
    options.create_if_missing = true;
    rocksdb::Status s1 = rocksdb::DB::Open(options, DBPATH, &db);
    log_info("db open status: {}", s1.ToString());

    {
        s1 = db->Put(WriteOptions(), "kkakkk123", "value1");
        assert(s1.ok());
        s1 = db->Put(WriteOptions(), "kkbkkk124", "value1");
        assert(s1.ok());
        s1 = db->Put(WriteOptions(), "kkckkk125", "value1");
        assert(s1.ok());
        db->CompactRange(rocksdb::CompactRangeOptions(), nullptr, nullptr);
    }

    rocksdb::Status s2 = db->Close();
    log_info("db close status: {}", s2.ToString());
}

TEST(rocksdb_demo, read_key) {
    rocksdb::DB *db;
    rocksdb::Options options;
    options.prefix_extractor.reset(new MyCustomPrefix(3, true));

    make_rocksdb_options(options);
    options.create_if_missing = true;
    rocksdb::Status s1 = rocksdb::DB::Open(options, DBPATH, &db);
    log_info("db open status: {}", s1.ToString());

    {
        auto ropt1 = ReadOptions();
        ropt1.total_order_seek = false;
        {
            log_info("---- iter1 with [total_order_seek = false]-----"); // nothing is found
            auto iter = db->NewIterator(ropt1);
            for (iter->Seek("kkakkk"); iter->Valid(); iter->Next()) {
                log_info("{}", iter->key().ToString());
            }
            delete iter;
            log_info("---- iter1 -----");
        }
    }
    {
        auto ropt1 = ReadOptions();
        ropt1.total_order_seek = true;
        {
            log_info("---- iter2 with [total_order_seek = true] -----"); // found the keys insert before
            auto iter = db->NewIterator(ropt1);
            for (iter->Seek("kkakkk"); iter->Valid(); iter->Next()) {
                log_info("{}", iter->key().ToString());
            }
            delete iter;
            log_info("---- iter2 -----");
        }
    }
    rocksdb::Status s2 = db->Close();
    log_info("db close status: {}", s2.ToString());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    cp1craft::utils::init_log_module();
    return RUN_ALL_TESTS();
}
