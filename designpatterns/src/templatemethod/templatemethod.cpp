#include <iostream>
#include <string>
#include <memory>

using namespace std;

class TestPaper {
public:
    void TestQuestion1() {
        cout << "杨过得到，后来给了郭靖，炼成倚天剑、屠龙刀的玄铁可能是[]\n"
                "a. 球磨铸铁\n"
                "b. 马口铁\n"
                "c. 高速合金铁\n"
                "d. 碳素纤维\n";
        cout << "答案：" << Answer1() << endl;
    }

    void TestQuestion2() {
        cout << "杨过、程英、陆无双铲除了情花，造成[ ]\n"
                "a.使这种植物不再害人\n"
                "b.使一种珍稀物种灭绝\n"
                "c.破坏了那个生物圈的生态平衡\n"
                "d.造成该地区沙漠化\n";
        cout << "答案：" << Answer2() << endl;
    }

    void TestQuestion3() {
        cout << "蓝凤凰致使华山师徒、桃谷六仙呕吐不止，如果你是大夫，会给他们开什么药[]\n"
                "a.阿司匹林\n"
                "b.牛黄解毒片\n"
                "c.氟哌酸\n"
                "d.让他们喝大量的生牛奶e.以上全不对\n";
        cout << "答案：" << Answer3() << endl;
    }

    virtual string Answer1() = 0;
    virtual string Answer2() = 0;
    virtual string Answer3() = 0;
};

class TestPaperA : public TestPaper {
public:
    string Answer1() override {
        return "b";
    }

    string Answer2() override {
        return "c";
    }

    string Answer3() override {
        return "a";
    }
};

class TestPaperB : public TestPaper {
public:
    string Answer1() override {
        return "c";
    }

    string Answer2() override {
        return "a";
    }

    string Answer3() override {
        return "a";
    }
};

int main(int argc, const char *argv[])
{
    cout << "学生甲抄的试卷：\n";
    unique_ptr<TestPaper> student_a = make_unique<TestPaperA>();
    student_a->TestQuestion1();
    student_a->TestQuestion2();
    student_a->TestQuestion3();

    cout << "学生乙抄的试卷：\n";
    unique_ptr<TestPaper> student_b = make_unique<TestPaperB>();
    student_b->TestQuestion1();
    student_b->TestQuestion2();
    student_b->TestQuestion3();
    return 0;
}