//给定 s 和 t 两个字符串，当它们分别被输入到空白的文本编辑器后，如果两者相等，返回 true 。# 代表退格字符。
//
// 注意：如果对空文本输入退格字符，文本继续为空。
//
//
//
// 示例 1：
//
//
//输入：s = "ab#c", t = "ad#c"
//输出：true
//解释：s 和 t 都会变成 "ac"。
//
//
// 示例 2：
//
//
//输入：s = "ab##", t = "c#d#"
//输出：true
//解释：s 和 t 都会变成 ""。
//
//
// 示例 3：
//
//
//输入：s = "a#c", t = "b"
//输出：false
//解释：s 会变成 "c"，但 t 仍然是 "b"。
//
//
//
// 提示：
//
//
// 1 <= s.length, t.length <= 200
// s 和 t 只含有小写字母以及字符 '#'
//
//
//
//
// 进阶：
//
//
// 你可以用 O(n) 的时间复杂度和 O(1) 的空间复杂度解决该问题吗？
//
//
// Related Topics 栈 双指针 字符串 模拟 👍 526 👎 0

#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

//leetcode submit region begin(Prohibit modification and deletion)
class Solution {
public:
    string getString(string s) {
        int slow = 0, fast = 0;
        while (fast < s.size()) {
            if (s[fast] != '#') {
                swap(s[slow++], s[fast]);
            } else {
                slow = slow == 0 ? 0 : --slow;
            }
            fast++;
        }
        return s.substr(0, slow);
    }

    bool backspaceCompare(string s, string t) {
        return getString(s) == getString(t);
    }
};
//leetcode submit region end(Prohibit modification and deletion)

int main(int argc, const char* argv[])
{
    Solution solution;
    cout << solution.backspaceCompare("ab#c", "ad#c") << endl;
    cout << solution.backspaceCompare("ab##", "c#d#") << endl;
    cout << solution.backspaceCompare("a#c", "b") << endl;
    return 0;
}