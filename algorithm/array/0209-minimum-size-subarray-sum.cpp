//给定一个含有 n 个正整数的数组和一个正整数 target 。
//
// 找出该数组中满足其和 ≥ target 的长度最小的 连续子数组 [numsl, numsl+1, ..., numsr-1, numsr] ，并返回其长
//度。如果不存在符合条件的子数组，返回 0 。
//
//
//
// 示例 1：
//
//
//输入：target = 7, nums = [2,3,1,2,4,3]
//输出：2
//解释：子数组[4,3]是该条件下的长度最小的子数组。
//
//
// 示例 2：
//
//
//输入：target = 4, nums = [1,4,4]
//输出：1
//
//
// 示例 3：
//
//
//输入：target = 11, nums = [1,1,1,1,1,1,1,1]
//输出：0
//
//
//
//
// 提示：
//
//
// 1 <= target <= 10⁹
// 1 <= nums.length <= 10⁵
// 1 <= nums[i] <= 10⁵
//
//
//
//
// 进阶：
//
//
// 如果你已经实现 O(n) 时间复杂度的解法, 请尝试设计一个 O(n log(n)) 时间复杂度的解法。
//
//
// Related Topics 数组 二分查找 前缀和 滑动窗口 👍 1419 👎 0

#include <iostream>
#include <vector>
#include <climits>

using namespace std;

//leetcode submit region begin(Prohibit modification and deletion)
class Solution {
public:
    int minSubArrayLen(int target, vector<int>& nums) {
        int result = INT32_MAX;
        int sum = 0; // 滑动窗口数值之和
        int i = 0; // 滑动窗口起始位置
        int window = 0; // 滑动窗口的长度
        for (int j = 0; j < nums.size(); j++) {
            sum += nums[j];
            // 注意这里使用while，每次更新 i（起始位置），并不断比较子序列是否符合条件
            while (sum >= target) {
                window = j - i + 1; // 取子序列的长度
                result = result < window ? result : window;
                sum -= nums[i++]; // 这里体现出滑动窗口的精髓之处，不断变更i（子序列的起始位置）
            }
        }
        // 如果result没有被赋值的话，就返回0，说明没有符合条件的子序列
        return result == INT32_MAX ? 0 : result;
    }

    int minSubArrayLen_1(int target, vector<int>& nums) {
        int result = INT32_MAX;
        int cur_seq_len = 0;
        for (int i = 0; i < nums.size(); i++) {
            int sum = 0;
            for (int j = i; j < nums.size(); j++) {
                sum += nums[j];
                if (sum >= target) {
                    cur_seq_len = j - i + 1;
                    result = cur_seq_len < result ? cur_seq_len : result;
                    break;
                }
            }
        }

        return result == INT32_MAX ? 0 : result;
    }

    int minSubArrayLen_2(int target, vector<int>& nums) {
        int n = nums.size();
        int ans = INT_MAX;
        int start = 0, end = 0;
        int sum = 0;
        while (end < n) {
            sum += nums[end];
            while (sum >= target) {
                ans = min(ans, end - start + 1);
                sum -= nums[start++];
            }
            end++;
        }

        return ans == INT_MAX ? 0 : ans;
    }
};
//leetcode submit region end(Prohibit modification and deletion)

int main(int argc, const char* argv[])
{
    vector<int> nums = {2,3,1,2,4,3};
    Solution solution;
    cout << solution.minSubArrayLen(7, nums) << endl;
    cout << solution.minSubArrayLen_1(7, nums) << endl;
    cout << solution.minSubArrayLen_2(7, nums) << endl;
    return 0;
}