/*
 * @lc app=leetcode.cn id=738 lang=cpp
 *
 * [738] 单调递增的数字
 *
 * https://leetcode.cn/problems/monotone-increasing-digits/description/
 *
 * algorithms
 * Medium (50.69%)
 * Likes:    450
 * Dislikes: 0
 * Total Accepted:    110.1K
 * Total Submissions: 217.1K
 * Testcase Example:  '10'
 *
 * 当且仅当每个相邻位数上的数字 x 和 y 满足 x <= y 时，我们称这个整数是单调递增的。
 * 
 * 给定一个整数 n ，返回 小于或等于 n 的最大数字，且数字呈 单调递增 。
 * 
 * 
 * 
 * 示例 1:
 * 
 * 
 * 输入: n = 10
 * 输出: 9
 * 
 * 
 * 示例 2:
 * 
 * 
 * 输入: n = 1234
 * 输出: 1234
 * 
 * 
 * 示例 3:
 * 
 * 
 * 输入: n = 332
 * 输出: 299
 * 
 * 
 * 
 * 
 * 提示:
 * 
 * 
 * 0 <= n <= 10^9
 * 
 * 
 */
#include <string>
using namespace std;

// @lc code=start
class Solution {
public:
    int monotoneIncreasingDigits(int n) {
        string strNum = to_string(n);
        int flag = strNum.size();
        for (int i = strNum.size() - 1; i > 0; i--) {
            if (strNum[i - 1] > strNum[i]) {
                flag = i;
                strNum[i - 1]--;
            }
        }

        for (int i = flag; i < strNum.size(); i++) {
            strNum[i] = '9';
        }

        return stoi(strNum);
    }
};
// @lc code=end

int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
