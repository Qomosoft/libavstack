//704. 二分查找

//给定一个n个元素有序的（升序）整型数组nums 和一个目标值target ，写一个函数搜索nums中的 target，如果目标值存在返回下标，否则返回 -1。


//示例 1:

//输入: nums = [-1,0,3,5,9,12], target = 9
//输出: 4
//解释: 9 出现在 nums 中并且下标为 4
//示例2:

//输入: nums = [-1,0,3,5,9,12], target = 2
//输出: -1
//解释: 2 不存在 nums 中因此返回 -1
//

//提示：

//你可以假设 nums中的所有元素是不重复的。
//n将在[1, 10000]之间。
//nums的每个元素都将在[-9999, 9999]之间。

//来源：力扣（LeetCode）
//链接：https://leetcode.cn/problems/binary-search
//著作权归领扣网络所有。商业转载请联系官方授权，非商业转载请注明出处。

#include <iostream>
#include <vector>

using std::vector;
using std::cout;
using std::endl;

class Solution {
public:
    int search(vector<int>& nums, int target) {
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

        return -1;
        
    }
};

int main(int argc, char const *argv[])
{
    vector<int> nums = {-1,0,3,5,9,12};
    Solution solution;
    cout << solution.search(nums, 9) << endl;

    return 0;
}
