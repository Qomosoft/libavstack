/*
 * @lc app=leetcode.cn id=763 lang=cpp
 *
 * [763] 划分字母区间
 *
 * https://leetcode.cn/problems/partition-labels/description/
 *
 * algorithms
 * Medium (76.82%)
 * Likes:    1099
 * Dislikes: 0
 * Total Accepted:    206.4K
 * Total Submissions: 268.7K
 * Testcase Example:  '"ababcbacadefegdehijhklij"'
 *
 * 给你一个字符串 s 。我们要把这个字符串划分为尽可能多的片段，同一字母最多出现在一个片段中。
 * 
 * 注意，划分结果需要满足：将所有划分结果按顺序连接，得到的字符串仍然是 s 。
 * 
 * 返回一个表示每个字符串片段的长度的列表。
 * 
 * 
 * 示例 1：
 * 
 * 
 * 输入：s = "ababcbacadefegdehijhklij"
 * 输出：[9,7,8]
 * 解释：
 * 划分结果为 "ababcbaca"、"defegde"、"hijhklij" 。
 * 每个字母最多出现在一个片段中。
 * 像 "ababcbacadefegde", "hijhklij" 这样的划分是错误的，因为划分的片段数较少。 
 * 
 * 示例 2：
 * 
 * 
 * 输入：s = "eccbbbbdec"
 * 输出：[10]
 * 
 * 
 * 
 * 
 * 提示：
 * 
 * 
 * 1 <= s.length <= 500
 * s 仅由小写英文字母组成
 * 
 * 
 */

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// @lc code=start
class Solution {
public:
    vector<int> partitionLabels(string s) {
        int hash[27] = {0};
        for (int i = 0; i < s.size(); i++) {
            hash[s[i] - 'a'] = i;
        }

        vector<int> result;
        int left = 0, right = 0;
        for (int i = 0; i < s.size(); i++) {
            right = max(hash[s[i] - 'a'], right);
            if (i == right) {
                result.push_back(right - left + 1);
                left = i + 1;
            }
        }

        return result;
    }
};
// @lc code=end

int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
