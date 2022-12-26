//给定一个字符串 s 和一个整数 k，从字符串开头算起，每计数至 2k 个字符，就反转这 2k 字符中的前 k 个字符。
//
//
// 如果剩余字符少于 k 个，则将剩余字符全部反转。
// 如果剩余字符小于 2k 但大于或等于 k 个，则反转前 k 个字符，其余字符保持原样。
//
//
//
//
// 示例 1：
//
//
//输入：s = "abcdefg", k = 2
//输出："bacdfeg"
//
//
// 示例 2：
//
//
//输入：s = "abcd", k = 2
//输出："bacd"
//
//
//
//
// 提示：
//
//
// 1 <= s.length <= 10⁴
// s 仅由小写英文组成
// 1 <= k <= 10⁴
//
//
// Related Topics 双指针 字符串 👍 396 👎 0

#include <iostream>
#include <string>
#include <algorithm>

using namespace std;
class Solution {
public:
    string reverseStr(string s, int k) {
        int n = s.size();

        int start = 0;
        while (start < n) {
            int end = min(start + k, n);
            reverse(s.begin() + start, s.begin() + end);
            start = end + k;
        }

        return s;
    }
};

int main(int argc, const char *argv[])
{
//1. 输入：s = "abcdefg", k = 2
//输出："bacdfeg"
//
//2. 输入：s = "abcd", k = 2
//输出："bacd"
    string s = "abcdefg";
//    string s = "abcd";
    int k = 2;
    Solution solution;
    cout << solution.reverseStr(s, k) << endl;

    return 0;
}
