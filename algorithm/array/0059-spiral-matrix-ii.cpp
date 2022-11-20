//给你一个正整数 n ，生成一个包含 1 到 n² 所有元素，且元素按顺时针顺序螺旋排列的 n x n 正方形矩阵 matrix 。
//
//
//
// 示例 1：
//
//
//输入：n = 3
//输出：[[1,2,3],[8,9,4],[7,6,5]]
//
//
// 示例 2：
//
//
//输入：n = 1
//输出：[[1]]
//
//
//
//
// 提示：
//
//
// 1 <= n <= 20
//
//
// Related Topics 数组 矩阵 模拟 👍 858 👎 0

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>

using namespace std;

//leetcode submit region begin(Prohibit modification and deletion)
class Solution {
public:
    vector<vector<int>> generateMatrix(int n) {
        vector<vector<int>> ans(n, vector<int>(n, 0));
        int top = 0, bottom = n - 1, left = 0, right = n - 1, num = 1;

        while (num <= n * n) {
            for (int i = left; i <= right; ++i, ++num) ans[top][i] = num;

            ++top;
            for (int i = top; i <= bottom; ++i, ++num) ans[i][right] = num;

            --right;
            for (int i = right; i >= left; --i, ++num) ans[bottom][i] = num;

            --bottom;
            for (int i = bottom; i >= top; --i, ++num) ans[i][left] = num;

            ++left;
        }

        return ans;
    }
};
//leetcode submit region end(Prohibit modification and deletion)

template<typename T>
ostream &operator<<(ostream &out, const vector<T> &v) {
    out << '[';
    copy(v.begin(), v.end(), ostream_iterator<T>(out, ","));
    out << "\b]";

    return out;
}

int main(int argc, const char *argv[])
{
    Solution solution;
    vector<vector<int>> ans = solution.generateMatrix(3);
    for (auto &v : ans) {
        cout << v << endl;
    }

    return 0;
}