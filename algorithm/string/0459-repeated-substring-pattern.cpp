//给定一个非空的字符串
// s ，检查是否可以通过由它的一个子串重复多次构成。
//
//
//
// 示例 1:
//
//
//输入: s = "abab"
//输出: true
//解释: 可由子串 "ab" 重复两次构成。
//
//
// 示例 2:
//
//
//输入: s = "aba"
//输出: false
//
//
// 示例 3:
//
//
//输入: s = "abcabcabcabc"
//输出: true
//解释: 可由子串 "abc" 重复四次构成。 (或子串 "abcabc" 重复两次构成。)
//
//
//
//
// 提示：
//
//
//
//
//
// 1 <= s.length <= 10⁴
// s 由小写英文字母组成
//
//
// Related Topics 字符串 字符串匹配 👍 858 👎 0

#include <iostream>
#include <string>

using namespace std;

class Solution {
public:
    bool repeatedSubstringPattern(string s) {
        int n = s.size();
        for (int i = 1; i * 2 <= n; ++i) {
            if (n % i == 0) {
                bool match = true;
                for (int j = i; j < n; ++j) {
                    if (s[j] != s[j - i]) {
                        match = false;
                        break;
                    }
                }

                if (match) return true;
            }
        }

        return false;
    }

    bool repeatedSubstringPatterI(string s) {
        return (s + s).find(s, 1) != s.size();
    }
};

int main(int argc, const char *argv[])
{
    string s = "abcabcabcabc";
//    string s = "aba";
    Solution solution;
    cout << solution.repeatedSubstringPattern(s) << endl;
    return 0;
}