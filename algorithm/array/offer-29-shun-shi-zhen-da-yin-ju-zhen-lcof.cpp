//输入一个矩阵，按照从外向里以顺时针的顺序依次打印出每一个数字。
//
//
//
// 示例 1：
//
// 输入：matrix = [[1,2,3],[4,5,6],[7,8,9]]
//输出：[1,2,3,6,9,8,7,4,5]
//
//
// 示例 2：
//
// 输入：matrix = [[1,2,3,4],[5,6,7,8],[9,10,11,12]]
//输出：[1,2,3,4,8,12,11,10,9,5,6,7]
//
//
//
//
// 限制：
//
//
// 0 <= matrix.length <= 100
// 0 <= matrix[i].length <= 100
//
//
// 注意：本题与主站 54 题相同：https://leetcode-cn.com/problems/spiral-matrix/
//
// Related Topics 数组 矩阵 模拟 👍 470 👎 0

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>

using namespace std;

//leetcode submit region begin(Prohibit modification and deletion)
class Solution {
public:
    vector<int> spiralOrder(vector<vector<int>>& matrix) {
        vector<int> ans;
        if (matrix.empty()) return ans;

        int top = 0, left = 0, right = matrix[0].size() - 1, bottom = matrix.size() - 1;

        while (true) {
            for (int i = left; i <= right; ++i) ans.push_back(matrix[top][i]);
            if (++top > bottom) break;

            for (int i = top; i <= bottom; ++i) ans.push_back(matrix[i][right]);
            if (--right < left) break;

            for (int i = right; i >= left; --i) ans.push_back(matrix[bottom][i]);
            if (--bottom < top) break;

            for (int i = bottom; i >= top; --i) ans.push_back(matrix[i][left]);
            if (++left > right) break;
        }

        return ans;
    }
};
//leetcode submit region end(Prohibit modification and deletion)

template <typename T>
ostream &operator<<(ostream &out, const vector<T> &v) {
    out << '[';
    copy(v.begin(), v.end(), ostream_iterator<T>(out, ","));
    out <<"\b]";

    return out;
}

int main(int argc, const char *argv[])
{
    vector<vector<int>> matrix = {
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12}
    };

    Solution solution;
    for (auto &v : matrix) {
        cout << v << endl;
    }

    cout << solution.spiralOrder(matrix) << endl;

    return 0;
}