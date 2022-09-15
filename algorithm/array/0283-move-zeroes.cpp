//给定一个数组 nums，编写一个函数将所有 0 移动到数组的末尾，同时保持非零元素的相对顺序。
//
// 请注意 ，必须在不复制数组的情况下原地对数组进行操作。
//
//
//
// 示例 1:
//
//
//输入: nums = [0,1,0,3,12]
//输出: [1,3,12,0,0]
//
//
// 示例 2:
//
//
//输入: nums = [0]
//输出: [0]
//
//
//
// 提示:
//
//
//
// 1 <= nums.length <= 10⁴
// -2³¹ <= nums[i] <= 2³¹ - 1
//
//
//
//
// 进阶：你能尽量减少完成的操作次数吗？
//
// Related Topics 数组 双指针 👍 1739 👎 0

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

using namespace std;

//leetcode submit region begin(Prohibit modification and deletion)
class Solution {
public:
    void moveZeroes(vector<int>& nums) {
    }
};
//leetcode submit region end(Prohibit modification and deletion)

template<typename T>
ostream& operator<<(ostream& out, const vector<T>& v) {
    if (v.empty())  {
        out << "[]";
        return out;
    }

    out << '[';
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
    out << "\b\b]";

    return out;
}

int main()
{
    vector<int> nums = {0, 1, 0, 3, 12};
    cout << nums << endl;
    return 0;
}
