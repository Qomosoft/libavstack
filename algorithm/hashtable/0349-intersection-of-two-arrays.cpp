//给定两个数组 nums1 和 nums2 ，返回 它们的交集 。输出结果中的每个元素一定是 唯一 的。我们可以 不考虑输出结果的顺序 。
//
//
//
// 示例 1：
//
//
//输入：nums1 = [1,2,2,1], nums2 = [2,2]
//输出：[2]
//
//
// 示例 2：
//
//
//输入：nums1 = [4,9,5], nums2 = [9,4,9,8,4]
//输出：[9,4]
//解释：[4,9] 也是可通过的
//
//
//
//
// 提示：
//
//
// 1 <= nums1.length, nums2.length <= 1000
// 0 <= nums1[i], nums2[i] <= 1000
//
//
// Related Topics 数组 哈希表 双指针 二分查找 排序 👍 685 👎 0

#include <iostream>
#include <vector>
#include <unordered_set>
#include <iterator>
#include <algorithm>

using namespace std;

//leetcode submit region begin(Prohibit modification and deletion)
class Solution {
public:
    vector<int> intersection(vector<int>& nums1, vector<int>& nums2) {
        unordered_set<int> ans;
        unordered_set<int> nums_set(nums1.begin(), nums1.end());
        for (auto &num : nums2) {
            if (nums_set.count(num)) {
                ans.insert(num);
            }
        }

        return vector<int>(ans.begin(), ans.end());
    }
};
//leetcode submit region end(Prohibit modification and deletion)

template <typename T>
ostream &operator<<(ostream &out, const vector<T> &elements) {
    out << '[';
    copy(elements.begin(), elements.end(), ostream_iterator<T>(out, ","));
    out << "\b]";

    return out;
}

int main(int argc, const char *argv[])
{
    vector<int> num1 = {1, 2, 2, 1}, num2 = {2, 2};
    Solution solution;
    cout << solution.intersection(num1, num2) << endl;
    return 0;
}