//给你一个字符串 s ，请你反转字符串中 单词 的顺序。
//
// 单词 是由非空格字符组成的字符串。s 中使用至少一个空格将字符串中的 单词 分隔开。
//
// 返回 单词 顺序颠倒且 单词 之间用单个空格连接的结果字符串。
//
// 注意：输入字符串 s中可能会存在前导空格、尾随空格或者单词间的多个空格。返回的结果字符串中，单词间应当仅用单个空格分隔，且不包含任何额外的空格。
//
//
//
// 示例 1：
//
//
//输入：s = "the sky is blue"
//输出："blue is sky the"
//
//
// 示例 2：
//
//
//输入：s = "  hello world  "
//输出："world hello"
//解释：反转后的字符串中不能存在前导空格和尾随空格。
//
//
// 示例 3：
//
//
//输入：s = "a good   example"
//输出："example good a"
//解释：如果两个单词间有多余的空格，反转后的字符串需要将单词间的空格减少到仅有一个。
//
//
//
//
// 提示：
//
//
// 1 <= s.length <= 10⁴
// s 包含英文大小写字母、数字和空格 ' '
// s 中 至少存在一个 单词
//
//
//
//
//
//
//
// 进阶：如果字符串在你使用的编程语言中是一种可变数据类型，请尝试使用 O(1) 额外空间复杂度的 原地 解法。
//
// Related Topics 双指针 字符串 👍 721 👎 0

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stack>

using namespace std;

class Solution {
public:
    string reverseWords(string s) {
        int left = 0, right = s.size() - 1;
        while (left <= right && s[left] == ' ') ++left;
        while (left <= right && s[right] == ' ') --right;

        stack<string> st;
        string word;

        while (left <= right) {
            char c = s[left];
            if (word.size() && c == ' ') {
                st.push(word);
                word = "";
            } else if (c != ' ') {
                word += c;
            }
            ++left;
        }
        st.push(word);

        string ans;
        while (!st.empty()) {
            ans += st.top();
            st.pop();
            if (!st.empty()) ans += ' ';
        }

        return ans;
    }

    string reverseWordsI(string s) {
        reverse(s.begin(), s.end());

        int n = s.size(), index = 0;
        for(int start = 0; start < n; ++start) {
            if (s[start] != ' ') {
                if (index != 0) s[index++] = ' ';

                int end = start;
                while (end < n && s[end] != ' ') s[index++] = s[end++];

                reverse(s.begin() + index - (end -start), s.begin() + index);

                start = end;
            }
        }

        s.erase(s.begin() + index, s.end());
        return s;
    }
};

int main(int argc, const char *argv[])
{
    string s = "the sky is blue";
    Solution solution;
//    cout << solution.reverseWords(s);
    cout << solution.reverseWordsI(s);
    return 0;
}