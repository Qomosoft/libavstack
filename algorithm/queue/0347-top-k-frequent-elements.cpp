//给你一个整数数组 nums 和一个整数 k ，请你返回其中出现频率前 k 高的元素。你可以按 任意顺序 返回答案。
//
//
//
// 示例 1:
//
//
//输入: nums = [1,1,1,2,2,3], k = 2
//输出: [1,2]
//
//
// 示例 2:
//
//
//输入: nums = [1], k = 1
//输出: [1]
//
//
//
// 提示：
//
//
// 1 <= nums.length <= 10⁵
// k 的取值范围是 [1, 数组中不相同的元素的个数]
// 题目数据保证答案唯一，换句话说，数组中前 k 个高频元素的集合是唯一的
//
//
//
//
// 进阶：你所设计算法的时间复杂度 必须 优于 O(n log n) ，其中 n 是数组大小。
//
// Related Topics 数组 哈希表 分治 桶排序 计数 快速选择 排序 堆（优先队列） 👍 1439 👎 0

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iterator>

using namespace std;

class Solution {
public:
    vector<int> topKFrequent(vector<int>& nums, int k) {
        unordered_map<int, int> m;
        for (auto &num : nums) m[num]++;

        priority_queue<pair<int, int>> pq;
        for (auto &e : m) pq.emplace(e.second, e.first);

        vector<int> ans;
        for (; k > 0; --k) {
            if (!pq.empty()) {
                ans.emplace_back(pq.top().second);
                pq.pop();
            }
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
//    vector<int> nums = {1,1,1,2,2,3};
    vector<int> nums = {4,4,2,2,2,3};
    int k = 2;
    Solution solution;
    cout << solution.topKFrequent(nums, k) << endl;
    return 0;
}