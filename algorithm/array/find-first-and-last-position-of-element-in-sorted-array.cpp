// 34. 在排序数组中查找元素的第一个和最后一个位置

// 给你一个按照非递减顺序排列的整数数组 nums，和一个目标值 target。请你找出给定目标值在数组中的开始位置和结束位置。

// 如果数组中不存在目标值 target，返回 [-1, -1]。

// 你必须设计并实现时间复杂度为 O(log n) 的算法解决此问题。

//  

// 示例 1：

// 输入：nums = [5,7,7,8,8,10], target = 8
// 输出：[3,4]
// 示例 2：

// 输入：nums = [5,7,7,8,8,10], target = 6
// 输出：[-1,-1]
// 示例 3：

// 输入：nums = [], target = 0
// 输出：[-1,-1]
//  

// 提示：

// 0 <= nums.length <= 105
// -109 <= nums[i] <= 109
// nums 是一个非递减数组
// -109 <= target <= 109

// 来源：力扣（LeetCode）
// 链接：https://leetcode.cn/problems/find-first-and-last-position-of-element-in-sorted-array
// 著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using std::vector;
using std::cout;
using std::endl;
using std::ostream;
using std::stringstream;
using std::string;

class Solution {
public:
    vector<int> searchRange(vector<int>& nums, int target) {
        int left = 0, right = nums.size() - 1;
        int ans = 0;
        while (left <= right) {
            int middle = left + (right - left) / 2;
            if (target < nums[middle]) {
                right = middle - 1;
            } else if (target > nums[middle]) {
                left =  middle + 1;
            } else {
                ans = middle;
                break;
            }
        }

        if (ans >= 0 && ans < nums.size() && nums[ans] == target) {
            for (int i = ans; i >= 0; i--) {
                if (nums[i] == target) {
                    left = i;
                }
            }

            for (int i = ans ; i <= nums.size() - 1; i++) {
                if (nums[i] == target) {
                    right = i;
                }
            }

            return {left, right};
        }

        return {-1, -1};
    }
};

ostream& operator<< (ostream& out, const vector<int>& v) {
    out << '[';
    string comma = "";
    for (auto& i : v) {
        out << comma;
        out << i;
        comma = ',';
    }
    out << ']';

    return out;
}

int main(int argc, char const *argv[])
{
    vector<int> nums = {5,7,7,8,8,10};
    Solution solution;
    cout << solution.searchRange(nums, 8);
    cout << solution.searchRange(nums, 6);
    cout << solution.searchRange(nums, 0);
    return 0;
}
