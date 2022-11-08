//给你一个字符串 s 、一个字符串 t 。返回 s 中涵盖 t 所有字符的最小子串。如果 s 中不存在涵盖 t 所有字符的子串，则返回空字符串 "" 。
//
//
//
// 注意：
//
//
// 对于 t 中重复字符，我们寻找的子字符串中该字符数量必须不少于 t 中该字符数量。
// 如果 s 中存在这样的子串，我们保证它是唯一的答案。
//
//
//
//
// 示例 1：
//
//
//输入：s = "ADOBECODEBANC", t = "ABC"
//输出："BANC"
//
//
// 示例 2：
//
//
//输入：s = "a", t = "a"
//输出："a"
//
//
// 示例 3:
//
//
//输入: s = "a", t = "aa"
//输出: ""
//解释: t 中两个字符 'a' 均应包含在 s 的子串中，
//因此没有符合条件的子字符串，返回空字符串。
//
//
//
// 提示：
//
//
// 1 <= s.length, t.length <= 10⁵
// s 和 t 由英文字母组成
//
//
//
//进阶：你能设计一个在
//o(n) 时间内解决此问题的算法吗？
//
// Related Topics 哈希表 字符串 滑动窗口 👍 2225 👎 0

#include <iostream>
#include <string>
#include <unordered_map>
#include <climits>

using namespace std;

//leetcode submit region begin(Prohibit modification and deletion)
class Solution {
public:
    string minWindow(string s, string t) {
        string ans = "";
        int ns = s.size(), nt = t.size();
        if (ns < nt) return ans;

        unordered_map<char, int> ht, hs;
        for (char c : t) ht[c]++;

        int need = nt;
        int min_len = ns + 1;
        int st = -1;

        for (int l = 0, r = 0; r < ns; ++r) {
            if (ht.count(s[r])) {
                if (ht[s[r]] > 0) need--;
                ht[s[r]]--;
            }

            while (need == 0) {
                if (r - l + 1 < min_len) {
                    st = l;
                    min_len = r - l + 1;
//                    ans = s.substr(st, min_len); //会导致超时
                }

                if (ht.count(s[l])) {
                    if (ht[s[l]] == 0) need++;
                    ht[s[l]]++;
                }
                l++;
            }
        }

        return min_len == ns + 1 ? "" : s.substr(st, min_len);
    }
};
//leetcode submit region end(Prohibit modification and deletion)

int main(int argc, const char* argv[])
{
    string s = "ADOBECODEBANC";
    string t = "ABC";
//    string s = "a";
//    string t = "aa";
    Solution solution;
    cout << solution.minWindow(s, t) << endl;
    return 0;
}
