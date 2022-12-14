//35. 搜索插入位置

//给定一个排序数组和一个目标值，在数组中找到目标值，并返回其索引。如果目标值不存在于数组中，返回它将会被按顺序插入的位置。

//请必须使用时间复杂度为 O(log n) 的算法。

//

//示例 1:

//输入: nums = [1,3,5,6], target = 5
//输出: 2
//示例2:

//输入: nums = [1,3,5,6], target = 2
//输出: 1
//示例 3:

//输入: nums = [1,3,5,6], target = 7
//输出: 4
//

//提示:

//1 <= nums.length <= 104
//-104 <= nums[i] <= 104
//nums 为无重复元素的升序排列数组
//-104 <= target <= 104

//来源：力扣（LeetCode）
//链接：https://leetcode.cn/problems/search-insert-position
//著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

class Solution {
public:
    int searchInsert(vector<int>& nums, int target) {
        int left = 0, right = nums.size();
        while (left < right) {
            int middle = left + (right - left) / 2;

            if (target < nums[middle]) {
                right = middle;
            } else if (target > nums[middle]) {
                left = middle + 1;
            } else {
                return middle;
            }
        }

        return left;
    }
};

int main(int argc, char const *argv[])
{
    vector<int> nums = {1,3,5,6};
    Solution solution;
    cout << solution.searchInsert(nums, 0);
    return 0;
}
