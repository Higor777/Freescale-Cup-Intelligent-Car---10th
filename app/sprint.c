#include "sprint.h"

//==============================================================
//函数名：float my_abs(float n)
//功能描述：求浮点数绝对值
//参数：n 浮点数
//返回：无
//==============================================================
float my_abs(float n)
{
    if(n<0)
        return -n;
    else
        return n;
}

//==============================================================
//函数名：void sprint(char snum[],int seat,int res,float ori_num)
//功能描述：浮点数转字符串
//参数：snum字符串地址 seat 补齐数 res 保留小数点位数 ori_num 浮点数
//返回：无
//==============================================================
void sprint(char snum[],int seat,int res,float ori_num)
{
    char str[10];
    int int_part;
    float dec_part;
    int dec_part_int;
    int str_len=0;
    int i;
    float num=my_abs(ori_num);

    int_part=(int)num;
    dec_part=num-int_part;

    do
    {
        str[str_len]='0'+(int_part%10);
        int_part/=10;

        str_len++;
    }while(int_part!=0);
    for(i=0;i<str_len;i++)
    {
        snum[i]=str[str_len-i-1];
    }
    snum[str_len]='.';
    str_len++;
    if(ori_num<0)
    {
        for(i=str_len;i>0;i--)
        {
            snum[i]=snum[i-1];
        }
        snum[0]='-';
        str_len++;
    }

    for(i=0; i<res; i++)
    {
        dec_part*=10;
    }
    dec_part_int=(int)dec_part;
    for(i=0;i<res;i++)
    {
        str[i]='0'+(dec_part_int%10);
        dec_part_int/=10;
    }
    for(i=0;i<res;i++)
    {
        snum[str_len+i]=str[res-i-1];
    }
    str_len+=res;

    for(i=str_len;i<seat;i++)
    {
        snum[str_len]=' ';
        str_len++;
    }
    snum[str_len]='\0';
}
