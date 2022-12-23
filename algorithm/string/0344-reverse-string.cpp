//编写一个函数，其作用是将输入的字符串反转过来。输入字符串以字符数组 s 的形式给出。
//
// 不要给另外的数组分配额外的空间，你必须原地修改输入数组、使用 O(1) 的额外空间解决这一问题。
//
//
//
// 示例 1：
//
//
//输入：s = ["h","e","l","l","o"]
//输出：["o","l","l","e","h"]
//
//
// 示例 2：
//
//
//输入：s = ["H","a","n","n","a","h"]
//输出：["h","a","n","n","a","H"]
//
//
//
// 提示：
//
//
// 1 <= s.length <= 10⁵
// s[i] 都是 ASCII 码表中的可打印字符
//
//
// Related Topics 双指针 字符串 👍 701 👎 0

#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iterator>

using namespace std;

class Solution {
public:
    void reverseString(vector<char>& s) {
        int left = 0, right = s.size() - 1;
        while (left < right) {
            swap(s[left++], s[right--]);
        }
    }
};

int main(int argc, const char *argv[])
{
    vector<char> s = {'h', 'e', 'l', 'l', 'o'};
    Solution solution;
    solution.reverseString(s);

    ostringstream oss;
    copy(s.begin(), s.end(), ostream_iterator<char>(oss, ""));
    cout << oss.str() << endl;

    return 0;
}