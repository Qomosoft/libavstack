//请实现一个函数，把字符串 s 中的每个空格替换成"%20"。
//
//
//
// 示例 1：
//
// 输入：s = "We are happy."
//输出："We%20are%20happy."
//
//
//
// 限制：
//
// 0 <= s 的长度 <= 10000
//
// Related Topics 字符串 👍 384 👎 0

#include <iostream>
#include <string>

using namespace std;

class Solution {
public:
    string replaceSpace(string s) {
        string ans;

        for (char c : s) {
            if (c == ' ') {
                ans += "20%";
            } else {
                ans += c;
            }
        }

        return ans;
    }

    string replaceSpaceI(string s) {
        int count = 0, origin_size = s.size();
        for (auto &c : s) {
            if (c == ' ') count++;
        }

        s.resize(origin_size + 2 * count);
        for (int i = origin_size - 1, j = s.size() - 1; i < j; i--, j--) {
            if (s[i] != ' ') {
                s[j] = s[i];
            } else {
                s[j - 2] = '%';
                s[j - 1] = '2';
                s[j] = '0';
                j -= 2;
            }
        }

        return s;
    }
};

int main(int argc, const char *argv[])
{
    string s = "We are happy.";
    Solution solution;
//    cout << solution.replaceSpace(s) << endl;
    cout << solution.replaceSpaceI(s) << endl;
    return 0;
}