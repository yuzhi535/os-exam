#include <stdio.h>
#include <stdlib.h>

const int MaxParts = 10;  //最大分区数
const int Memsize = 4096; //存储大小

typedef enum Bool
{
    false = 0,
    true,
} bool;

typedef struct Part //分区信息
{
    int startAddress;  //开始地址
    int length;        //分区长度
    bool free;         //是否空闲
    struct Part *next=NULL; //下一个分区
} Part;

typedef struct Distribution //分配信息
{
    bool give;     //是否给予空间
    Part *part;    //分配的空间
    int errorType; //错误类型 0 无 1 分区数超限 2 请求不能满足
} Distribution;

Part part;
int cnt;

void init()
{ //初始化
    part.startAddress = 0;
    part.length = Memsize;
    part.free = true;
    part.next = NULL;
    cnt = 1;
}

/**
 * @brief 请求内存空间
 *
 * @param length 请求的长度
 */
Distribution requestMemory(int length)
{
    bool can = false;
    Part *distriPart = NULL;
    int errorType = 0;
    Part *temp = &part;
    do
    {
        if (temp->free && temp->length >= length)
        { //如果空闲且大于则分配新分区
            if (cnt + 1 > MaxParts)
            { //到达最大分区限制则分配
                errorType = 1;
                break;
            }
            else
                cnt++;

            temp->free = false;
            int tempLength = temp->length;
            temp->length = length;

            // 把选中的分区划分一部分满足请求,另外一部分插入到其后面
            Part *tempNext = temp->next;

            temp->next = &(Part){.free = true, .startAddress = temp->startAddress + temp->length, .length = tempLength - length, .next = tempNext};

            distriPart = temp;
            can = true;
            break;
        }
        else if (temp->free && temp->length == length)
        {
            //空闲且等于请求大小则直接分配
            temp->free = false;
            distriPart = temp;
            can = true;
            break;
        }
        else
            temp = temp->next;

    } while (temp != NULL);
    errorType = 2;

    Distribution res = {.give = can, .part = distriPart, .errorType = errorType};

    return res;
}

/**
 * @brief 释放分区
 *
 * @param n 显示的第n个分区
 */
void freePart(int n)
{
    int i = 1;
    Part *temp = &part;
    bool is_free = false;
    do
    {
        if (i + 1 == n && temp->next != NULL)
        {
            if (temp->next->next != NULL && temp->next->next->free == true)
            {
                if (temp->free)
                { // 三连
                    Part *center = temp->next;
                    Part *right = temp->next->next;
                    Part *tempNext = temp->next->next->next;
                    temp->length += temp->next->length + temp->next->next->length;
                    temp->next = tempNext;
                    free(center);
                    free(right);
                }
                else
                { //后连
                    Part *right = temp->next->next;
                    Part *tempNext = temp->next->next->next;
                    temp->next->length += temp->next->next->length;
                    temp->next->next = tempNext;
                    temp->next->free = true;
                    free(right);
                }
            }
            else
            {
                if (temp->free)
                { //前连
                    Part *center = temp->next;
                    Part *tempNext = temp->next->next;
                    temp->length += temp->next->length;
                    temp->next = tempNext;
                    free(center);
                }
                else
                { //独
                    temp->next->free = true;
                }
            }
            is_free = true;
            break;
        }
        temp = temp->next;
        i++;
    } while (temp != NULL);
    if (!is_free)
    {
        printf("释放失败,分区未找到.\n");
    }
}

/**
 * @brief 打印分区信息
 *
 */
void showParts()
{
    int i = 1;
    Part *temp = &part;
    printf("分区号 开始地址 分区长度 空闲\n");
    do
    {
        printf("%-6d %-8d %-8d %-4s\n", i, temp->startAddress, temp->length, temp->free ? "是" : "否");
        temp = temp->next;
        i++;
    } while (temp != NULL);
}

/**
 * @brief 主菜单
 *
 */
void selectMenu()
{
    int select;
    int value;
    Distribution ret;
    bool quit = false;
    while (!quit)
    {
        printf("模拟存储管理\n");
        printf("1、申请空间\n");
        printf("2、释放空间\n");
        printf("3、退出程序\n");
        scanf("%d", &select);
        switch (select)
        {
        case 1:
            printf("请输入大小:\n");
            scanf("%d", &value);
            ret = requestMemory(value);
            if (ret.give)
            {
                printf("分配成功.\n");
            }
            else
            {
                printf("分配失败.\n");
                printf("%s.\n", ret.errorType == 1 ? "分区数达到限制" : "该请求目前不能被满足");
            }
            break;
        case 2:
            printf("请输入序号:\n");
            scanf("%d", &value);
            freePart(value);
            break;
        case 3:
            quit = true;
            break;
        default:
            break;
        }
        if (!quit)
            showParts();
    }
}

int main()
{
    init();
    showParts();
    selectMenu();
    getchar();
    printf("退出成功");
}
