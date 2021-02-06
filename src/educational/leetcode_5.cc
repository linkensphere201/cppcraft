// keywords: leetcode
//

#include <gtest/gtest.h>

#include <iostream>

#include "log.h"

void ShowTopic() {
  log_info(
      "Topic: "
      "leetcode");
}

TEST(leetcode, ShowTopic) { ShowTopic(); }

using namespace std;

#include <string>
#define MAX 1005
static int subl[MAX][MAX];
static int stofs[MAX][MAX];
class Solution {
 public:
  string longestPalindrome(string s) {
    memset(subl, 0, sizeof(subl));
    memset(stofs, 0, sizeof(stofs));
    int len = s.length();
    for (int i = 1; i <= len; i++) {
      subl[i][i] = 1;
      stofs[i][i] = i;
    }
    for (int sc = 2; sc <= len; sc++) {
      for (int i = 1; i <= len; i++) {
        int j = i + sc - 1;
        if (j <= len) {
          int m = 0;
          if (s[i - 1] == s[j - 1] && subl[i + 1][j - 1] == (j - i - 1)) {
            m = subl[i + 1][j - 1] + 2;
          }
          if (m > subl[i + 1][j] && m > subl[i][j - 1]) {
            subl[i][j] = m;
            stofs[i][j] = i;
          } else if (subl[i + 1][j] > subl[i][j - 1]) {
            subl[i][j] = subl[i + 1][j];
            stofs[i][j] = stofs[i + 1][j];
          } else {
            subl[i][j] = subl[i][j - 1];
            stofs[i][j] = stofs[i][j - 1];
          }
        }
      }
    }
    std::string target = s.substr(stofs[1][len] - 1, subl[1][len]);
    // printf("origin: %s \n"
    //       "target: %s \n", s.c_str() , target.c_str());
    return target;
  }
};

TEST(longestPalindrome, sol1) {
  Solution s;
  s.longestPalindrome("aabaabaa");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  cp1craft::common::init_log_module();
  return RUN_ALL_TESTS();
}
