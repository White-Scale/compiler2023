#include <stdio.h>
void read(char input[]){
    char c;
    int index = 0;

    scanf("%c", &c);
    while(c != '\n' && c != '\0'){
        // if(c == '\0')   //我觉着应该是EOF退出的，但是不行……
            // break;
        input[index] = c;
        index = index + 1;
        scanf("%c", &c);
    }
    input[index] = '\0';
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

    int i = 0;
    int index = 0;

    int lane = 0;
    int class = 0;
    int charCount = 0;

    int hasLane = 0;

    float GPA, rem;

    int GPA1, GPA2;

    int count = 0;

    int recommend = 0;  //是否推荐该课
    int hasTake = 0;

    int take, finalResult;
    int breakLabel = 0;
    int passLane = 1;   //是否存在没修课程
    int judgeClassIndex = 0;
    int takeThis = 0;
    {
        i = 0;
        while(i < 101)
        {
            i = i + 1;
            preLanes[i] = 0;
        }
    }

    read(input);

    while (input[0] != '\0'){   //还有新课
        //开始针对培养方案中某一门课进行处理
        index = 0;

        {
            while(input[index] != '|')
            {
                classList[thisClass][index] = input[index];
                index = index + 1;
            }
            classList[thisClass][index] = '\0';
            index = index + 1;  //escape '|'
        }
        credit = input[index] - '0';
        index = index + 2;  //escape credit and '|'
        lane = 0;
        class = 0;
        charCount = 0;

        hasLane = 0;
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

        thisClass = thisClass + 1;  //处理下一门课
        read(input);
    }

    if(attemptCredits == 0)
        GPA = 0.0;
    else
        GPA = totalGrade / attemptCredits;
    GPA1 = GPA;
    GPA = GPA * 10 - GPA1 * 10;
    GPA2 = GPA;
    rem = GPA * 10 - GPA2 * 10;
    if(rem > 5) GPA2 = GPA2 + 1;
    if(GPA2 == 10)
    {
        GPA2 = 0;
        GPA1 = GPA1 + 1;
    }
    printf("GPA: %d.%d\n", GPA1, GPA2);
    printf("Hours Attempted: %d\n", attemptCredits);
    printf("Hours Completed: %d\n", completedCredits);
    printf("Credits Remaining: %d\n", remainingCredits);
    printf("\n");
    printf("Possible Courses to Take Next\n");
    if(remainingCredits == 0){
        printf("  None - Congratulations!\n");
    }
    count = 0;
    while (count < thisClass){
        recommend = 0;  //是否推荐该课
        hasTake = 0;
        charCount = 0;
        {
            take = 0;
            while(hasTake == 0 && take < completeCount){
                //compareString
                {
                    index = 0;
                    finalResult = 1;
                    while(finalResult == 1 && classList[count][index] != '\0')
                    {
                        if(completeClass[take][index] == '\0') //length2 < length1
                            finalResult = 0;
                        if(completeClass[take][index] != classList[count][index])  //2 != 1
                            finalResult = 0;
                        index = index + 1;
                    }
                    if (completeClass[take][index] != '\0') //langth1 < length2
                        finalResult = 0;
                    hasTake = finalResult;  //return finalResult to hasTake
                }

                take = take + 1;
            }
        }
        if(hasTake == 0){//该课未修，判断是否前置已修完
            if(preLanes[count] == 0)
                recommend = 1;
            lane = 0;
            breakLabel = 0;
            while(breakLabel == 0 && lane < preLanes[count])   //还没有判断完所有的前置路线
            {
                passLane = 1;   //是否存在没修课程
                judgeClassIndex = 0;
                while(passLane == 1 && classes[count][lane][judgeClassIndex][0] != '\0')  //这条路线上还有课程
                {
                    i = 0;
                    takeThis = 0;
                    while(takeThis == 0 && i < completeCount){   //遍历已修课程列表
                        //compareString
                        {
                            index = 0;
                            finalResult = 1;
                            while(finalResult == 1 && classes[count][lane][judgeClassIndex][index] != '\0')
                            {
                                if(completeClass[i][index] == '\0') //length2 < length1
                                    finalResult = 0;
                                if(completeClass[i][index] != classes[count][lane][judgeClassIndex][index])  //2 != 1
                                    finalResult = 0;
                                index = index + 1;
                            }
                            if (completeClass[i][index] != '\0') //langth1 < length2
                                finalResult = 0;
                            takeThis = finalResult;  //return finalResult to hasTake
                        }

                        i = i + 1;
                    }
                    if(takeThis == 0)   //这条路线中该门课程未修
                    {
                        passLane = 0;
                        // break;
                    }
                    else {  //这条路线中这门课程已修，前去判断下一门课程是否已修
                        judgeClassIndex = judgeClassIndex + 1;    //判断这条路线中下一门课是否已修
                    }
                }
                if(passLane == 1)   //存在一条前置路线已修完，可做推荐
                {
                    recommend = 1;
                    breakLabel = 1;
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
            index = 0;
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
