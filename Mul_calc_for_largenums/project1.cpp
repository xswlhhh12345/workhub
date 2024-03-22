#include <iostream>
#include <string.h>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <time.h>
using namespace std;

#define MAX 1024

bool flag1=true,flag2=true;
int *mul_result = new int [MAX]{0};
int len1=0;
int len2=0; 
int len3=0;
int dotflag=0;

void Float_Mul(char *ptr1,char* ptr2,int* result);
void Big_Mul(char *ptr1,char* ptr2,int* result);
bool Dot_Flag(char *ptr1,char* ptr2);
void Scientific_Notation(char* ptr,int*plen);
void check(char *ptr1,char* ptr2);
void Ptr_Print(char*ptr);
void Is_Scientific(char *ptr,int *plen);


int main(int argc,char* argv[])
{
    
    time_t begin,end;
    begin=clock();
    if(argc == 3)
    {
        char ptr1[MAX]={0};
        char ptr2[MAX]={0};

        //判断是否为负数，是的话通过前移字符串就取绝对值，然后打印负号
        //打印符号在最后面输出的时候打印
        if(argv[1][0]=='-')
        {
            strcpy(&argv[1][0],&argv[1][1]);
            flag1=false;
        }
        if(argv[2][0]=='-')
        {
            strcpy(&argv[2][0],&argv[2][1]);
            flag2=false;
        }

        //打印表达式
        Ptr_Print(argv[1]);
        cout<<" *"<<" "; 
        Ptr_Print(argv[2]);
        len1=strlen(&argv[1][0]);
        len2=strlen(&argv[2][0]);

        //检查字符串是否合法,不合法直接退出程序
        Is_Scientific(argv[1],&len1);
        Is_Scientific(argv[2],&len2);
        check(argv[1],argv[2]);
        
        //将字符倒过来才符合规律
        for(int i=0;i<len1;i++)
        {
            ptr1[i]=int(argv[1][len1-1-i]-'0');//注意这里ptr里面存的已经是数字了，不是它的ASCII码
        }
        for(int i=0;i<len2;i++)
        {
            ptr2[i]=int(argv[2][len2-1-i]-'0');
        }

        cout<<" ="<<" "; 
        //根据判断有没有.的结果来选择进入函数
        //输出计算结果(逆序输出逆序算出的结果就是正序)
        if (Dot_Flag(argv[1],argv[2]))
        {//Entering float_Mul func()
            Float_Mul(ptr1,ptr2,mul_result);
            if(flag1==true&&flag2==false||flag1==false&&flag2==true)
	            cout<<"-";
            if(mul_result[len3-1]==0)
                mul_result[len3-1]='.';
            if(mul_result[len3-1]==46)
                cout<<"0";
            for(int i=len3-1 ; i>=0 ; i--)
            {
                if(mul_result[i]!=46)
                {
                    cout<<mul_result[i];
                }
                else    putchar('.');
            }
        }
        else
        {//Entering Big_Mul func()
            Big_Mul(ptr1,ptr2,mul_result);
            if(flag1==true&&flag2==false||flag1==false&&flag2==true)
	            cout<<"-";
            for(int i=len3-1 ; i>=0 ; i--)
            {
                cout<<mul_result[i];
            }
        }
    }
    else
    {
        cout<<"two argument expected."<<endl;
    }
    delete []mul_result;
    end=clock();
    cout<<endl;
    cout<<endl<<"runtime:   "<<double(end-begin)/CLOCKS_PER_SEC<<endl;


    return 0;
}

//浮点数乘法
void Float_Mul(char *ptr1,char* ptr2,int* result)
{
    int temp;
    int dotflag1=0;
    int dotflag2=0;
    len3=len1+len2;//这里len还算有.的长度

//判断是否为'.',并记录位置，我们处理是将dot展示移出，等计算完再放回来
    for(int i=0;i<len1;i++)
    {
        if(int(ptr1[i])==int('.'-'0'))
        {
            dotflag1=i;
            strcpy(&ptr1[i],&ptr1[i+1]);
        }
    }
    for(int i=0;i<len2;i++)
    {
        if(int(ptr2[i])==int('.'-'0'))
        {
            dotflag2=i;
            strcpy(&ptr2[i],&ptr2[i+1]);
        }
        
    }
    
    dotflag=dotflag1+dotflag2;
    Big_Mul(ptr1,ptr2,mul_result);
    
    //把小数点插进去，计算结果是 123.45的话，在存储中会是54321，把小数点插入54.321,长度len+1
    //strcpy(&mul_result[dotflag+1],&mul_result[dotflag]);
    if(dotflag)
    {
        len3=len3+1;
        for(int i=len3-1;i>dotflag;i--)
        {
            mul_result[i]=mul_result[i-1];
        }
        mul_result[dotflag]='.';//这里本来放点。可是左值是一个INT类型
        //所以会转为ACSII码，这个地方会显示46,我们遇到46去替换就行了
    }
    /*
    cout<<endl;
    cout<<"dot"<<dotflag<<endl;
    for(int i=0;i<len3+1;i++)
    {
        cout<<mul_result[i]<<",";
    }
    */
}

//判断是否采用浮点数
bool Dot_Flag(char *ptr1,char* ptr2)
{
    int max=len1;
    if(len1<len2)
        max=len2;
    //获取两者的最大值，以用于比较
    for(int i=0;i<max;i++)
    {
        if(i<len1)
        {
            if(ptr1[i]=='.')
            {
                return true;
            }
        }
        if(i<len2)
        {
            if(ptr2[i]=='.')
            {
                return true;
            }
        }
    }
    return false;
}

//大整数乘法
void Big_Mul(char *ptr1,char* ptr2,int* mul_result)
{
    int temp;
    len3=len1+len2;
    for(int i=0 ; i < len1 ; ++i)
    {
        temp =0;
        for(int j=0 ; j < len2 ; ++j)
        {
            mul_result[i+j]=ptr1[i]*ptr2[j]+temp+mul_result[i+j];//现有的加，之前的进位+原本的数据
            temp=mul_result[i+j]/10; //留下temp用于退出循环进位，如果没有退出循环，就在循环里进位
            mul_result[i+j]%=10;
            //cout<<"mul_result_"<<i+j<<":"<<mul_result[i+j]<<endl;
        }
        mul_result[i+len2]=temp;
    }
    
    for(int i=len3-1 ; i>=0 ; --i)//从倒数开始，减去多余的位置
    {
        if(mul_result[i]==0 && len3 > 1)
        {
            len3--;
        }
        else
        {
            break;
        }
    }
    
}

void check(char *ptr1,char* ptr2)
{   
    int char_e=0;
    int char_dot=0;
    for(int i=0;i<len1;i++)
    {
        int qwq=((ptr1[i]=='.'||ptr1[i]=='-')||(ptr1[i]=='+'||ptr1[i]=='E')||(ptr1[i]=='e'));
        if(!(('0'<=ptr1[i]&&ptr1[i]<='9')||qwq))
        {
            cout<<"ERROR[0]1:Please enter a valid number"<<endl;
            exit(0);
        }
        if(ptr1[i]=='.')
        {
            char_dot++;
            if(char_dot>=2)
            {
                cout<<"ERROR[1]:Too many dot!"<<endl;
                exit(0);
            }
        }
        if(ptr1[i]=='E'||ptr1[i]=='e')
        {
            char_e++;
            if(char_e>=2)
            {
                cout<<"ERROR[2]:To many 'E'!"<<endl;
                exit(0);
            }
        }
        
    }
    char_e=0;
    char_dot=0;
    for(int i=0;i<len2;i++)
    {
        int qwq=((ptr2[i]=='.'||ptr2[i]=='-')||(ptr2[i]=='+'||ptr2[i]=='E')||(ptr2[i]=='e'));
        if(!(('0'<=ptr2[i]&&ptr2[i]<='9')||qwq))
        {
            cout<<"ERROR[0]2:Please enter a valid number"<<endl;
            exit(0);
        }
        if(ptr2[i]=='.')
        {
            char_dot++;
            if(char_dot>=2)
            {
                cout<<"ERROR[1]:Too many dot!"<<endl;
                exit(0);
            }
        }
        if(ptr2[i]=='E'||ptr2[i]=='e')
        {
            char_e++;
            if(char_e>=2)
            {
                cout<<"ERROR[2]:To many 'E'!"<<endl;
                exit(0);
            }
        }
        
    }


}
    
void Scientific_Notation(char* ptr,int *plen)
{
    char c[50]={'\x00'};
    string s=ptr;
    double double_num;
    int eIndex = s.find("E");
    double base = stod(s.substr(0, eIndex)); //获取小数部分
    int exp = stoi(s.substr(eIndex + 1)); //获取指数部分
    double_num= base * pow(10, exp);
    sprintf(ptr,"%f",double_num);
    *plen=strlen(ptr);

}

void Ptr_Print(char*ptr)
{
    int len=strlen(ptr);
    for(int i=0;i<len;i++)
    {
        cout<<ptr[i];
    }
}

void Is_Scientific(char *ptr,int *plen)
{
    for ( int i = 0; i < *plen; i++)
    {
        if(ptr[i]=='E'||ptr[i]=='e')
        {
            Scientific_Notation(ptr,plen);
        }
    }
}


