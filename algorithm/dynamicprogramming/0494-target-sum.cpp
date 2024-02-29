/*
 * @lc app=leetcode.cn id=494 lang=cpp
 *
 * [494] 目标和
 *
 * https://leetcode.cn/problems/target-sum/description/
 *
 * algorithms
 * Medium (48.41%)
 * Likes:    1867
 * Dislikes: 0
 * Total Accepted:    412.9K
 * Total Submissions: 853.1K
 * Testcase Example:  '[1,1,1,1,1]\n3'
 *
 * 给你一个非负整数数组 nums 和一个整数 target 。
 * 
 * 向数组中的每个整数前添加 '+' 或 '-' ，然后串联起所有整数，可以构造一个 表达式 ：
 * 
 * 
 * 例如，nums = [2, 1] ，可以在 2 之前添加 '+' ，在 1 之前添加 '-' ，然后串联起来得到表达式 "+2-1" 。
 * 
 * 
 * 返回可以通过上述方法构造的、运算结果等于 target 的不同 表达式 的数目。
 * 
 * 
 * 
 * 示例 1：
 * 
 * 
 * 输入：nums = [1,1,1,1,1], target = 3
 * 输出：5
 * 解释：一共有 5 种方法让最终目标和为 3 。
 * -1 + 1 + 1 + 1 + 1 = 3
 * +1 - 1 + 1 + 1 + 1 = 3
 * +1 + 1 - 1 + 1 + 1 = 3
 * +1 + 1 + 1 - 1 + 1 = 3
 * +1 + 1 + 1 + 1 - 1 = 3
 * 
 * 
 * 示例 2：
 * 
 * 
 * 输入：nums = [1], target = 1
 * 输出：1
 * 
 * 
 * 
 * 
 * 提示：
 * 
 * 
 * 1 <= nums.length <= 20
 * 0 <= nums[i] <= 1000
 * 0 <= sum(nums[i]) <= 1000
 * -1000 <= target <= 1000
 * 
 * 
 */
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// @lc code=start
class Solution {
public:
    int findTargetSumWays(vector<int>& nums, int target) {
        int sum = 0;
        for (auto num : nums) sum += num;
        if (abs(target) > sum) return 0;
        if ((target + sum) % 2 == 1) return 0;
        int bagWeight = (target + sum) / 2;
        vector<int> dp(bagWeight + 1, 0);
        dp[0] = 1;
        for (int i = 0; i < nums.size(); i++) {
            for (int j = bagWeight; j >= nums[i]; j--) {
                dp[j] += dp[j - nums[i]];
            }
        }

        return dp[bagWeight];
    }
};
// @lc code=end

int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
