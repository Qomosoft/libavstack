//给你两个字符串 haystack 和 needle ，请你在 haystack 字符串中找出 needle 字符串的第一个匹配项的下标（下标从 0 开始）。
//如果 needle 不是 haystack 的一部分，则返回 -1 。
//
//
//
// 示例 1：
//
//
//输入：haystack = "sadbutsad", needle = "sad"
//输出：0
//解释："sad" 在下标 0 和 6 处匹配。
//第一个匹配项的下标是 0 ，所以返回 0 。
//
//
// 示例 2：
//
//
//输入：haystack = "leetcode", needle = "leeto"
//输出：-1
//解释："leeto" 没有在 "leetcode" 中出现，所以返回 -1 。
//
//
//
//
// 提示：
//
//
// 1 <= haystack.length, needle.length <= 10⁴
// haystack 和 needle 仅由小写英文字符组成
//
//
// Related Topics 双指针 字符串 字符串匹配 👍 1686 👎 0

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Solution {
public:
    int strStr(string haystack, string needle) {
        int n = haystack.size(), m = needle.size();
        for (int i = 0; i + m <= n; ++i) {
            bool flag = true;
            for (int j = 0; j < m; ++j) {
                if (haystack[i + j] != needle[j]) {
                    flag = false;
                    break;
                }
            }
            if (flag == true) {
                return i;
            }
        }

        return -1;
    }

    int strStrI(string haystack, string needle) {
        vector<int> next = build_next(needle);
        int n = haystack.size(), i = 0;
        int m = needle.size(), j = 0;

        while (j < m && i < n) {
            if (0 > j || haystack[i] == needle[j]) {
                ++i, ++j;
            } else {
                j = next[j];
            }
        }

        return j == m ? i - j : -1;
    }

private:
    vector<int> build_next(const string &p) {
        int m = p.size();
        vector<int> next(m);
        int j = 0, t = next[0] = -1;
        while (j < m - 1) {
            if (0 > t || p[j] == p[t]) {
                ++j, ++t;
                next[j] = (p[j] != p[t] ? t : next[t]);
            } else {
                t = next[t];
            }
        }

        return next;
    }
};

int main(int argc, const char *argv[])
{
    string haystack = "sadbutsad", needle = "sad";
//    string haystack = "leetcode", needle = "leeto";
//    string haystack = "hhleetcode", needle = "lee";
    Solution solution;
    cout << solution.strStr(haystack, needle) << endl;
    cout << solution.strStrI(haystack, needle) << endl;
    return 0;
}