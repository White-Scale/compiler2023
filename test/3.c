#include <stdio.h>
void read(char* input){
    char c;
    scanf("%c", &c);
    int index = 0;
    while(c != '\n'){
        if(c == '\0')   //我觉着应该是EOF退出的，但是不行……
            break;
        input[index] = c;
        index = index + 1;
        scanf("%c", &c);
    }
    input[index] = '\0';
}
int getName(char* input, char* name, int index){
    while(input[index] != '|')
    {
        name[index] = input[index];
        index = index + 1;
    }
    name[index] = '\0';
    index = index + 1;  //escape '|'
    return index;
}
int compareString(char* str1, char* str2){
    int index = 0;
    while(str1[index] != '\0')
    {
        if(str2[index] == '\0')
            return 0;
        if(str2[index] != str1[index])
            return 0;
        index = index + 1;
    }
    if(str2[index] == '\0')
        return 1;
    else return 0;
}
int main(){
    char classList[101][5];
    float credit;
    char classes[101][8][8][5]; //[w][x][y][z]:w门课，每门有x组前置课，每组y门，每门课名字数组为z
    float totalGrade = 0;
    int attemptCredits = 0;
    int completedCredits = 0;
    int remainingCredits = 0;
    // char recommed[100][5];
    char input[500];
    char completeClass[101][5]; //[x][y]:x门已修课的课名
    int thisClass = 0;      //所有课程数量
    int completeCount = 0;  //已修课程数量
    int preLanes[101];  //每门课有多少条前置路径

    {
        int i = 0;
        while(i < 101)
        {
            i = i + 1;
            preLanes[i] = 0;
        }
    }

    read(input);

    while (input[0] != '\0'){   //还有新课
        //开始针对培养方案中某一门课进行处理
        int index = 0;
        index = getName(input, classList[thisClass], index);
        credit = input[index] - '0';
        index = index + 2;  //escape credit and '|'
        char tempClass[5];    
        int lane = 0;
        int class = 0;
        int charCount = 0;

        int hasLane = 0;
        if(input[index] != '|')
            hasLane = 1;    //需要在最后的lane数量上+1，因为下买你的lane统计的是分号数目
        while(input[index] != '|'){
            if(input[index] == ';') //将有一条新的前置路线
            {
                classes[thisClass][lane][class][charCount] = '\0';  //为该条线路最后一课封口
                classes[thisClass][lane][class+1][0] = '\0';    //该条路线下没有下一节课了
                lane = lane + 1;
                class = 0;
                charCount = 0;
            }
            else if(input[index] == ',')    //该路线下的下一门课
            {
                classes[thisClass][lane][class][charCount] = '\0';  //为该门课封口
                class = class + 1;
                charCount = 0;
            }
            else{
                classes[thisClass][lane][class][charCount] = input[index];
                charCount = charCount + 1;
            }
            index = index + 1;  //读取下一个字符
        }
        classes[thisClass][lane][class][charCount] = '\0';  //为最后一条线路最后一课封口
        classes[thisClass][lane][class+1][0] = '\0';    //该条路线下没有下一节课了
        preLanes[thisClass] = lane + hasLane;
        classes[thisClass][lane+hasLane][class][charCount] = '\0';  //没有下一条路线了

        index = index + 1;  //escapa '|'
        if(input[index] != '\0')    //这门课有分，且及格
        {
            int t = 0;
            attemptCredits = attemptCredits + credit;
            if(input[index] == 'F'){    //已修，挂科
                remainingCredits = remainingCredits + credit;
            }
            else{
                while(classList[thisClass][t] != '\0')
                {
                    completeClass[completeCount][t] = classList[thisClass][t]; //复制当前课程
                    t = t + 1; 
                }
                completeClass[completeCount][t] = '\0';
                completedCredits = completedCredits + credit;
                if(input[index] == 'A')
                    totalGrade = totalGrade + 4.0 * credit;
                else if(input[index] == 'B')
                    totalGrade = totalGrade + 3.0 * credit;
                else if(input[index] == 'C')
                    totalGrade = totalGrade + 2.0 * credit;
                else if(input[index] == 'D')
                    totalGrade = totalGrade + 1.0 * credit;
                completeCount = completeCount + 1;  //修了一门课
            }
        }
        else    //出现，但未修
        {
            remainingCredits = remainingCredits + credit;
        }
        // int i = 0;
        // while(input[i] != '\0')
        // printf("%c", input[i++]);
        // printf("%s", input);
        thisClass = thisClass + 1;  //处理下一门课
        read(input);
    }
    float GPA;
    if(attemptCredits == 0)
        GPA = 0.0;
    else
        GPA = totalGrade / attemptCredits;
    printf("GPA: %.1f\n", GPA);
    printf("Hours Attempted: %d\n", attemptCredits);
    printf("Hours Completed: %d\n", completedCredits);
    printf("Credits Remaining: %d\n", remainingCredits);
    printf("\n");
    printf("Possible Courses to Take Next\n");
    if(remainingCredits == 0){
        printf("  None - Congratulations!\n");
    }
    int count = 0;
    while (count < thisClass){
        int recommend = 0;  //是否推荐该课
        int hasTake = 0;
        int charCount = 0;
        {
            int take = 0;
            while(take < completeCount){
                hasTake = compareString(classList[count], completeClass[take]); //判断该课是否已修
                if(hasTake == 1){
                    break;  //该课已修
                }
                take = take + 1;
            }
        }
        if(hasTake == 0){//该课未修，判断是否前置已修完
            if(preLanes[count] == 0)
                recommend = 1;
            int lane = 0;
            while(lane < preLanes[count])   //还没有判断完所有的前置路线
            {
                int passLane = 1;   //是否存在没修课程
                int judgeClassIndex = 0;
                while(classes[count][lane][judgeClassIndex][0] != '\0')  //这条路线上还有课程
                {
                    int i = 0;
                    int takeThis = 0;
                    while(i < completeCount){   //遍历已修课程列表
                        takeThis = compareString(classes[count][lane][judgeClassIndex], completeClass[i]);
                        if(takeThis == 1)   //该课已修
                            break;
                        i = i + 1;
                    }
                    if(takeThis == 0)   //这条路线中该门课程未修
                    {
                        passLane = 0;
                        break;
                    }
                    else {  //这条路线中这门课程已修，前去判断下一门课程是否已修
                        judgeClassIndex = judgeClassIndex + 1;    //判断这条路线中下一门课是否已修
                    }
                }
                if(passLane == 1)   //存在一条前置路线已修完，可做推荐
                {
                    recommend = 1;
                    break;
                }
                else    //该条前置路线没完成，去判断下一条路线
                    lane = lane + 1;    //判断下一条前置路线是否全部修完
            }
        }
        else {
            recommend = 0;
        };  //该课已修，不做推荐

        if(recommend == 1)
        {
            printf("  ");
            int index = 0;
            while(classList[count][index] != '\0')
            {
                printf("%c", classList[count][index]);
                index = index + 1;
            }
            printf("\n");
        }
        count++;    //该门课判断完成，前往判断下一门课
    }
    return 0;
}
