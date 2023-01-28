//给定一个只包括 '('，')'，'{'，'}'，'['，']' 的字符串 s ，判断字符串是否有效。
//
// 有效字符串需满足：
//
//
// 左括号必须用相同类型的右括号闭合。
// 左括号必须以正确的顺序闭合。
// 每个右括号都有一个对应的相同类型的左括号。
//
//
//
//
// 示例 1：
//
//
//输入：s = "()"
//输出：true
//
//
// 示例 2：
//
//
//输入：s = "()[]{}"
//输出：true
//
//
// 示例 3：
//
//
//输入：s = "(]"
//输出：false
//
//
//
//
// 提示：
//
//
// 1 <= s.length <= 10⁴
// s 仅由括号 '()[]{}' 组成
//
//
// Related Topics 栈 字符串 👍 3704 👎 0

#include<iostream>
#include <string>
#include <stack>
#include <unordered_map>

using namespace std;

class Solution {
public:
    bool isValid(string s) {
        unordered_map<char, char> hash = {{')', '('}, {']', '['}, {'}', '{'}};
        stack<char> st;
        for (auto &c : s) {
            if (!st.empty() && st.top() == hash[c]) {
                st.pop();
            } else {
                st.push(c);
            }
        }

        return st.empty();
    }
};

int main(int argc, const char *argv[])
{
    string s = "()";
    Solution solution;
    cout << solution.isValid(s) << endl;
    return 0;
}