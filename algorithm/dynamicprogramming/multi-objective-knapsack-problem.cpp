// 52. 携带研究材料（第七期模拟笔试）
// 时间限制：1.000S  空间限制：128MB
// 题目描述
// 小明是一位科学家，他需要参加一场重要的国际科学大会，以展示自己的最新研究成果。他需要带一些研究材料，但是他的行李箱空间有限。这些研究材料包括实验设备、文献资料和实验样本等等，它们各自占据不同的重量，并且具有不同的价值。

// 小明的行李箱所能承担的总重量为 N，问小明应该如何抉择，才能携带最大价值的研究材料，每种研究材料可以选择无数次，并且可以重复选择。

// 输入描述
// 第一行包含两个整数，N，V，分别表示研究材料的种类和行李空间 

// 接下来包含 N 行，每行两个整数 wi 和 vi，代表第 i 种研究材料的重量和价值

// 输出描述
// 输出一个整数，表示最大价值。
// 输入示例
// 4 5
// 1 2
// 2 4
// 3 4
// 4 5
// 输出示例
// 10
// 提示信息
// 第一种材料选择五次，可以达到最大值。

// 数据范围：

// 1 <= N <= 10000;
// 1 <= V <= 10000;
// 1 <= wi, vi <= 10^9.

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Solution {
public:
    int multiObjKnapsackProblem(int bagweight, const vector<int>& weight, const vector<int>& value) {
        vector<int> dp(bagweight + 1, 0);
        dp[0] = 0;
        for (int i = 0; i < weight.size(); i++) {
            for (int j = weight[i]; j <= bagweight; j++) {
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
    cout << solution.multiObjKnapsackProblem(bagweight, weight, value) << endl;
    return 0;
}
