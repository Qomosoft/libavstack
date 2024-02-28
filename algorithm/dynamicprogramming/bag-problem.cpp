// 46. 携带研究材料（第六期模拟笔试）
// 时间限制：5.000S  空间限制：128MB
// 题目描述
// 小明是一位科学家，他需要参加一场重要的国际科学大会，以展示自己的最新研究成果。他需要带一些研究材料，但是他的行李箱空间有限。这些研究材料包括实验设备、文献资料和实验样本等等，它们各自占据不同的空间，并且具有不同的价值。 

// 小明的行李空间为 N，问小明应该如何抉择，才能携带最大价值的研究材料，每种研究材料只能选择一次，并且只有选与不选两种选择，不能进行切割。

// 输入描述
// 第一行包含两个正整数，第一个整数 M 代表研究材料的种类，第二个正整数 N，代表小明的行李空间。

// 第二行包含 M 个正整数，代表每种研究材料的所占空间。 

// 第三行包含 M 个正整数，代表每种研究材料的价值。

// 输出描述
// 输出一个整数，代表小明能够携带的研究材料的最大价值。
// 输入示例
// 6 1
// 2 2 3 1 5 2
// 2 3 1 5 4 3
// 输出示例
// 5
// 提示信息
// 小明能够携带 6 种研究材料，但是行李空间只有 1，而占用空间为 1 的研究材料价值为 5，所以最终答案输出 5。 

// 数据范围：
// 1 <= N <= 5000
// 1 <= M <= 5000
// 研究材料占用空间和价值都小于等于 1000
// https://kamacoder.com/problempage.php?pid=1046

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Solution {
public:
    int bagProblem(int bagweight, const vector<int>& weight, const vector<int>& value) {
        vector<vector<int>> dp(weight.size(), vector<int>(bagweight + 1, 0));
        for (int j = weight[0]; j <= bagweight; j++) {
            dp[0][j] = value[0];
        }

        for (int i = 1; i < weight.size(); i++) { // 遍历物品
            for (int j = 0; j <= bagweight; j++) { // 遍历背包容量
                if (j < weight[i]) dp[i][j] = dp[i - 1][j];
                else dp[i][j] = max(dp[i - 1][j], dp[i - 1][j - weight[i]] + value[i]);
            }
        }

        return dp[weight.size() - 1][bagweight];
    }

    int bagProblemI(int bagweight, const vector<int>& weight, const vector<int>& value) {
        vector<int> dp(bagweight + 1, 0);
        dp[0] = 0;
        for (int i = 0; i < weight.size(); i++) {
            for (int j = bagweight; j >= weight[i]; j--) {
                dp[j] = max(dp[j], dp[j - weight[i]] + value[i]);
            }
        }

        return dp[bagweight];
    }
};

int main(int argc, char const *argv[])
{
    int bagweight = 4;
    vector<int> weight = {1, 3, 4};
    vector<int> value = {15, 20, 30};

    Solution solution;
    cout << solution.bagProblem(bagweight, weight, value) << endl;
    cout << solution.bagProblemI(bagweight, weight, value) << endl;
    return 0;
}
