// keywords: ltweek1
//

#include "log.h"
#include <gtest/gtest.h>
#include <iostream>

void ShowTopic() {
  log_info("Topic: "
           "ltweek1");
}

TEST(ltweek1, ShowTopic) { ShowTopic(); }

#include <set>
#include <vector>
using namespace std;
class Solution1 {
public:
  const char *findnotzero(const char *p) {
    while (*p == '0') {
      p = p + 1;
    }
    return p;
  }

  int numDifferentIntegers(string word) {
    std::vector<int> stidx;
    int state = 0;
    // 0 - not digit
    // 1 - digit

    for (int i = 0; i < word.size(); i++) {
      if (word[i] >= '0' && word[i] <= '9') {
        if (state == 0) {
          stidx.push_back(i);
          state = 1; // digit
        }
      } else {
        word[i] = '\0';
        state = 0; // not digit
      }
    }

    // std::set<uint64_t> am;
    std::set<std::string> am2;
    int nonrepeatcount = 0;
    for (auto p : stidx) {
      uint64_t s = std::strtoull(&word[p], nullptr, 10);
      // auto ret = am.emplace(s);
      std::string r(findnotzero(&word[p]));
      auto ret2 = am2.emplace(r);
      log_info("{} --> {}, repeat: {}", &word[p], r, ret2.second);
      if (ret2.second) {
        nonrepeatcount++;
      }
    }
    log_info("non-repeat: {}", nonrepeatcount);
    return nonrepeatcount;
  }
};

TEST(ltw, sol1) {
  Solution1 s;
  s.numDifferentIntegers("a1b01c001");
  s.numDifferentIntegers(
      "uu717761265362523668772526716127260222200144937319826j717761265362523668"
      "772526716127260222200144937319826k71776126536252366877252671612726022220"
      "0144937319826b7177612653625236687725267161272602222001449373198262hgb9ut"
      "ohfvfrxprqva3y5cdfdudf7zuh64mobfr6mhy17");
}

TEST(ltw, strtoull) {
  std::string a = "0090";
  log_info("{} --> {}", a, std::strtoull(&a[0], nullptr, 10));
}

#include <unordered_map>

class Solution2 {
public:
  struct strx {
    const char *st;
    bool conv;
  };

  string evaluate(string s, vector<vector<string>> &knowledge) {
    std::unordered_map<string, string> knowledgem;
    for (auto i : knowledge) {
      knowledgem[i[0]] = i[1];
    }
    auto GetConvStr = [&knowledgem](const string & k) -> string {
      auto it = knowledgem.find(k);
      if (it == knowledgem.end()) {
        return "?";
      } else {
        return it->second;
      }
    };
    
    int state = 0;
    bool need_conv = false;
    // 0 outside a string
    // 1 inside a string
    

    std::vector<strx> allstrings;
    for (int i = 0; i < s.size();i++) {
      if (s[i] == '(') {
        s[i] = '\0';
        state = 0;
        need_conv = true;
      } else if (s[i] == ')') {
        s[i] = '\0';
        state = 0;
        need_conv = false;
      } else {
        if (state == 0) {
          strx a;
          a.st = &s[i];
          a.conv = need_conv;
          allstrings.push_back(a);
          log_info("push_back: {}, {}", a.st, a.conv);
          state = 1;
        }
      }
    }
    string ret = "";
    for (auto i : allstrings) {
      if (i.conv) {
        ret += GetConvStr(i.st);
      } else {
        ret += std::string(i.st);
      }
      log_info("{} : {} --> {}", i.st, i.conv, GetConvStr(i.st));
    }
    
    log_info("ret: {}", ret);
    return ret;
  }
};

TEST(ltw, sol2) { 
  vector<vector<string>> km = {{"a", "yes"}}; 
  Solution2 s;
  s.evaluate("(a)(a)(a)aaa", km);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::utils::init_log_module();
  return RUN_ALL_TESTS();
}
