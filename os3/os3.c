#include <stdio.h>
#include <stdlib.h>

const int MaxParts = 8;   //最大分区数
const int Memsize = 4096; //存储大小

typedef enum boolean
{
    false = 0,
    true,
} boolean;

typedef struct Node //分区信息
{
    int start_address; //开始地址
    int length;        //分区长度
    boolean free;      //是否空闲
    struct Node *next; //下一个分区
} Node;

typedef struct Quota //分配信息
{
    boolean give; //是否给予空间
    Node *quota;  //分配的空间
    int err_type; //错误类型 0 无 1 分区数超限 2 请求不能满足
} Quota;

Node *head;
int cnt;

void init()
{ //初始化
    head = (Node *)malloc(sizeof(Node));
    head->start_address = 0;
    head->length = Memsize;
    head->free = true;
    head->next = NULL;
    cnt = 1;
}

/**
 * @brief 请求内存空间
 *
 * @param length 请求的长度
 */
Quota request_memory(int val)
{
    boolean if_give = false;
    Node *distri_quota = NULL;
    int err_type = 0;
    boolean flag = true;
    Node *temp = head;
    do
    {
        if (temp->free && temp->length > val)
        { //如果空闲且大于则分配新分区
            if (cnt + 1 > MaxParts)
            { //如果到达最大分区数且不满足其中空闲分区的长度，则会限制分配
                flag = false;
                temp = temp->next;
                continue;
            }
            else
                cnt++;

            temp->free = false;
            int tempLength = temp->length;
            temp->length = val;

            if (temp->length < tempLength)
            {
                // 把选中的分区划分一部分满足请求,另外一部分插入到其后面
                Node *tempNext = temp->next;

                temp->next = (Node *)malloc(sizeof(Node));
                temp->next->free = true;
                temp->next->length = tempLength - val;
                temp->next->start_address = temp->start_address + temp->length;
                temp->next->next = tempNext;
            }

            distri_quota = temp;
            if_give = true;
            break;
        }
        else if (temp->free && temp->length == val)
        {
            //空闲且等于请求大小则直接分配

            if (!flag)
                flag = true;

            temp->free = false;
            distri_quota = temp;
            if_give = true;
            break;
        }
        else
            temp = temp->next;

    } while (temp != NULL);
    err_type = 0;
    if (!flag)
        err_type = 1;
    Quota res = {.give = if_give, .quota = distri_quota, .err_type = err_type};

    return res;
}

/**
 * @brief 释放分区
 *
 * @param n 显示的第n个分区
 */
void free_quota(int n)
{
    int i = 1;
    Node *temp = head;
    boolean is_free = false;
    do
    {
        if (n == 1)
        {
            if (temp->next && temp->next->free)
            {
                head = temp->next;
                int length = temp->length;
                free(temp);
                temp = head;
                temp->length += length;
                is_free = true;
                temp->start_address -= length;
            }
            else if (temp->next)
            {
                temp->free = true;
                is_free = true;
            }
            else
            {
                is_free = false;
            }
            break;
        }
        else if (i + 1 == n && temp->next != NULL)
        {
            if (temp->next->next != NULL && temp->next->next->free == true)
            {
                if (temp->free)
                {
                    // 若两个节点都空闲
                    Node *center = temp->next;
                    Node *right = center->next;
                    Node *tempNext = right->next;
                    temp->length += center->length + right->length;
                    temp->next = tempNext;
                    free(center);
                    free(right);
                }
                else
                {
                    //若前面和后面都不空闲，则设置该节点为空闲即可，然后增加长度
                    Node *right = temp->next->next;
                    Node *tempNext = right->next;
                    temp->next->length += right->length;

                    temp->next->next = tempNext;
                    temp->next->free = true;
                    free(right);
                }
            }
            else
            {
                if (temp->free)
                {
                    //删除空闲节点并把长度加到上一个链表上
                    Node *center = temp->next;
                    Node *tempNext = center->next;
                    temp->length += center->length;
                    temp->next = tempNext;
                    free(center);
                }
                else
                {
                    // 若其前后不空闲
                    temp->next->free = true;
                    cnt += 1;
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
    else
    {
        cnt -= 1;
    }
}

/**
 * @brief 打印分区信息
 *
 */
void show()
{
    int i = 1;
    Node *temp = head;
    printf("分区号 是否空闲 起始地址 分区长度\n");
    do
    {
        printf("%-6d %-9s %-8d %-8d \n", i, temp->free ? "是" : "否", temp->start_address, temp->length);
        temp = temp->next;
        i++;
    } while (temp != NULL);
}

/**
 * @brief 主菜单
 *
 */
void menu()
{
    int selected;
    int val;
    Quota res;
    boolean quit = false;
    while (!quit)
    {
        printf("模拟存储管理\n");
        printf("1、申请空间\n");
        printf("2、释放空间\n");
        printf("3、退出程序\n");
        printf("您的输入是: ");
        scanf("%d", &selected);
        switch (selected)
        {
        case 1:
            printf("请输入申请的空间大小:\n");
            scanf("%d", &val);
            res = request_memory(val);
            if (res.give)
            {
                printf("分配成功.\n");
            }
            else
            {
                printf("分配失败.\n");
                printf("%s.\n", res.err_type == 1 ? "分区数达到限制" : "该请求目前不能被满足");
            }
            break;
        case 2:
            printf("请输入序号:\n");
            scanf("%d", &val);
            free_quota(val);
            break;
        case 3:
            quit = true;
            break;
        default:
            break;
        }
        if (!quit)
            show();
    }
}

int main()
{
    init();
    show();
    menu();
    getchar();
    printf("退出成功");
}
