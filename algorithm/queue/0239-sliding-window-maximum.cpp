//给你一个整数数组 nums，有一个大小为 k 的滑动窗口从数组的最左侧移动到数组的最右侧。你只可以看到在滑动窗口内的 k 个数字。滑动窗口每次只向右移动一位
//。
//
// 返回 滑动窗口中的最大值 。
//
//
//
// 示例 1：
//
//
//输入：nums = [1,3,-1,-3,5,3,6,7], k = 3
//输出：[3,3,5,5,6,7]
//解释：
//滑动窗口的位置                最大值
//---------------               -----
//[1  3  -1] -3  5  3  6  7       3
// 1 [3  -1  -3] 5  3  6  7       3
// 1  3 [-1  -3  5] 3  6  7       5
// 1  3  -1 [-3  5  3] 6  7       5
// 1  3  -1  -3 [5  3  6] 7       6
// 1  3  -1  -3  5 [3  6  7]      7
//
//
// 示例 2：
//
//
//输入：nums = [1], k = 1
//输出：[1]
//
//
//
//
// 提示：
//
//
// 1 <= nums.length <= 10⁵
// -10⁴ <= nums[i] <= 10⁴
// 1 <= k <= nums.length
//
//
// Related Topics 队列 数组 滑动窗口 单调队列 堆（优先队列） 👍 2092 👎 0

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <queue>

using namespace std;

class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> ans;
        for (int i = 0; i < n; ++i) {
            int max_num = nums[i];
            for (int j = i; j - i < k && j < n; ++j) {
                max_num = max(max_num, nums[j]);
            }
            ans.push_back(max_num);
            if (n - i <= k) return ans;
        }

        return ans;
    }

    vector<int> maxSlidingWindowI(vector<int>& nums, int k) {
        priority_queue<pair<int, int>> pq;
        for (int i = 0; i < k; ++i) pq.emplace(nums[i], i);
        vector<int> ans = {pq.top().first};
        int n = nums.size();
        for (int i = k; i < n; ++i) {
            pq.emplace(nums[i], i);
            while (pq.top().second <= i - k) pq.pop();
            ans.emplace_back(pq.top().first);
        }

        return ans;
    }
};

template <typename T>
ostream &operator<<(ostream &out, const vector<T> &v) {
    out << '[';
    copy(v.begin(), v.end(), ostream_iterator<T>(out, ","));
    out << "\b]";
    return out;
}

int main(int argc, const char *argv[])
{
    vector<int> nums = {1,3,-1,-3,5,3,6,7};
    int k = 3;
    Solution solution;
    cout << solution.maxSlidingWindow(nums, k) << endl;
    cout << solution.maxSlidingWindowI(nums, k) << endl;
    return 0;
}
