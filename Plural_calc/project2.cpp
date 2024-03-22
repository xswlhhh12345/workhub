#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <gmpxx.h>
#include <unordered_map>
#include <stack>
#include <vector>
#include <queue>//运用到C++的栈和队列
using namespace std;
#define MAXSIZE 1024

void CountEr(string &buf);
void BlankFliter(string &buf);
void ModTraslate(string &buf);//转换模，转换科学计数法，转换共轭复数  
void ConTraslate(string &buf);
void ETraslate(string &buf);
bool NameCheck(string &buf);
void NameTraslate(string &buf);
string AdjustResult(int exp,string &buf);
string MpfAdjustResult(int exp,string &buf);
bool CheckImga(string &buf);
int WhoFirst(char op);
bool Invalid=0;

unordered_map<string,string>umap{

};
int Out_State=0;

//Complex一个浮点类，一个整数类，可以用函数模板，对一些必要的操作进行重载.
class Complex{
    public://也设立两个负数的flag
        mpf_t realPart;//实
        mpf_t imaginaryPart;//虚
    Complex()
    {
        mpf_init2(imaginaryPart,333);
        mpf_init2(realPart,333);
        mpf_set_ui(realPart,0);
        mpf_set_ui(imaginaryPart,0);
    }
    Complex(mpf_t initx,mpf_t inity)
    {
        mpf_init2(imaginaryPart,333);
        mpf_init2(realPart,333);
        mpf_set(realPart,initx);
        mpf_set(imaginaryPart,inity);
    }
    char* getx(){
        return mpf_get_str(nullptr,0,10,0,realPart);
    }
    char* gety(){
        return mpf_get_str(nullptr,0,10,0,imaginaryPart);
    }
    Complex & operator=(Complex b)
    {
        mpf_set(this->realPart,b.realPart);
        mpf_set(this->imaginaryPart,b.imaginaryPart);
        return *this;
    }
    Complex operator+(Complex b) const
    {
        Complex result= Complex();
        mpf_add(result.realPart,b.realPart,this->realPart);
        mpf_add(result.imaginaryPart,b.imaginaryPart,this->imaginaryPart);
        return result;
    }
    Complex operator-(Complex b) const
    {
        Complex result= Complex();
        mpf_sub(result.realPart,b.realPart,this->realPart);
        mpf_sub(result.imaginaryPart,b.imaginaryPart,this->imaginaryPart);
        return result;
    }
    Complex operator*(Complex b) const
    {
        Complex result= Complex();
        mpf_t ac,bd,bc,ad;
        mpf_init2(ac,333);
        mpf_init2(bd,333);
        mpf_init2(bc,333);
        mpf_init2(ad,333);

        mpf_mul(ac,b.realPart,this->realPart);
        mpf_mul(bd,b.imaginaryPart,this->imaginaryPart);
        mpf_mul(bc,b.realPart,this->imaginaryPart);
        mpf_mul(ad,b.imaginaryPart,this->realPart);
        mpf_sub(result.realPart,ac,bd);
        mpf_add(result.imaginaryPart,bc,ad);


        return result;
    }
    Complex operator/(Complex b) const
    {
        Complex result= Complex();
        mpf_t ac,bd,bc,ad;
        mpf_t temp1,temp2;

        mpf_init2(ac,333);
        mpf_init2(bd,333);
        mpf_init2(bc,333);
        mpf_init2(ad,333);
        mpf_init2(temp1,333);
        mpf_init2(temp2,333);
        mpf_mul(ac,b.realPart,this->realPart);
        mpf_mul(bd,b.imaginaryPart,this->imaginaryPart);
        mpf_mul(bc,b.realPart,this->imaginaryPart);
        mpf_mul(ad,b.imaginaryPart,this->realPart);
        
        //(a+bi)/(c+di)=(ac + bd)/(c^2 + d ^2) +((bc - ad)/(c ^2 + d ^2)) i
        mpf_pow_ui(temp1,b.realPart,2);
        mpf_pow_ui(temp2,b.imaginaryPart,2);
        mpf_add(temp1,temp1,temp2);
        mpf_add(result.realPart,ac,bd);
        mpf_div(result.realPart,result.realPart,temp1);
        mpf_sub(result.imaginaryPart,bc,ad);
        mpf_div(result.imaginaryPart,result.imaginaryPart,temp1);

        return result;
    }
    Complex operator+(string buf) const
    {
        
        Complex result= Complex();
        result=(*this);
        mpf_t temp;
        mpf_init2(temp,333);

        if(CheckImga(buf)){
            if(buf.length()==1)//说明是纯虚数
                buf="1";//将唯一的i，替换为数字1
            else
                buf.pop_back();//删除最后一个为 i
            mpf_set_str(temp,buf.c_str(),10);
            mpf_add(result.imaginaryPart,this->imaginaryPart,temp);
        }else{
            mpf_set_str(temp,buf.c_str(),10);
            mpf_add(result.realPart,this->realPart,temp);
        }

        return result;
    }
    Complex operator-(string buf) const
    {
        
        Complex result= Complex();
        result=(*this);
        mpf_t temp;
        mpf_init2(temp,333);

        if(CheckImga(buf)){
            if(buf.length()==1)//说明是纯虚数
                buf="1";//将唯一的i，替换为数字1
            else
                buf.pop_back();//删除最后一个为 i  
            mpf_set_str(temp,buf.c_str(),10);          
            mpf_sub(result.imaginaryPart,this->imaginaryPart,temp);
        }else{
            mpf_set_str(temp,buf.c_str(),10); 
            mpf_sub(result.realPart,this->realPart,temp);
        }

        return result;
    }
    Complex operator*(string buf) const
    {
        Complex result= Complex();
        result=(*this);
        Complex temp=Complex();
        if(CheckImga(buf)){
            if(buf.length()==1)//说明是纯虚数
                buf="1";//将唯一的i，替换为数字1
            else
                buf.pop_back();//删除最后一个为 i  
            mpf_set_str(temp.imaginaryPart,buf.c_str(),10);
        }else{
            mpf_set_str(temp.realPart,buf.c_str(),10);
        }

        result=(*this)*temp;
        return result;
    }
    Complex operator/(string buf) const
    {
        Complex result= Complex();
        result=(*this);
        Complex temp=Complex();
        if(CheckImga(buf)){
            if(buf.length()==1)//说明是纯虚数
                buf="1";//将唯一的i，替换为数字1
            else
                buf.pop_back();//删除最后一个为 i  
            mpf_set_str(temp.imaginaryPart,buf.c_str(),10);
        }else{
            mpf_set_str(temp.realPart,buf.c_str(),10);
        }
        result=(*this)/temp;
        return result;
    }
    string mod() const
    {
        mpf_t a,b,c;
        string result;
        mp_exp_t exponent;
        char* word;

        mpf_init2(a,333);
        mpf_init2(b,333);
        mpf_init2(c,333);
        mpf_pow_ui(a,this->realPart,2);
        mpf_pow_ui(b,this->imaginaryPart,2);
        mpf_add(a,a,b);
        mpf_sqrt(c,a);

        word=mpf_get_str(nullptr,&exponent,10,0,c);
        result=word;
        result=MpfAdjustResult(exponent,result);
        return result;
    }
    string con() const
    {
        mpf_t a,b,c;
        mp_exp_t exponent;//long
        char* word;
        string result1;
        string result2;
        int idx_neg;
        bool flag_neg=0;

        mpf_init2(a,333);
        mpf_init2(b,333);
        mpf_init2(c,333);
        result1=this->realdata();
        mpf_neg(b,this->imaginaryPart);//取反
        word=mpf_get_str(nullptr,&exponent,10,0,b);
        result2=word;
        idx_neg=result2.find('-');
        if(idx_neg!=std::string::npos){
            result2.erase(idx_neg,1);
            flag_neg=1;
        }
        result2=MpfAdjustResult(exponent,result2);
        if(flag_neg){
            result2.insert(idx_neg,"-");
        }

        if(!result2.empty()&&!result1.empty()){
            result2.append("i");
            return result1+"+"+result2;
        }else if(result1.empty()){
            result2.append("i");
            return result2;
        }else{
            return result1;
        }
    }
    string alldata() const
    {
        string str1=this->realdata();
        string str2=this->imagdata();
        return str1+str2;
    }
    string realdata() const
    {
        mp_exp_t exponent;
        char* word;
        string result;
        char buffer[100];
        bool neg_flag=0;

        mpf_get_str(buffer,&exponent,10,0,this->realPart);
        result=buffer;
        if(result.find("-")!=std::string::npos){
            neg_flag=true;
            result.erase(result.find("-"),1);
        }
        
        result=MpfAdjustResult(exponent,result);
        if(neg_flag){
            result.insert(0,"-");
        }
        return result;
    }
    string imagdata() const
    {
        mp_exp_t exponent;
        string result;
        char buffer[100]; 
        bool neg_flag=false;

        mpf_get_str(buffer,&exponent,10,0,this->imaginaryPart);
        result = buffer;
        if(result.find("-")!=std::string::npos){
            neg_flag=true;
            result.erase(result.find("-"),1);
        }
        result=MpfAdjustResult(exponent,result);

        if(!result.empty()){
            result+="i";
            if(neg_flag){
                result.insert(0,"-");
            }else{
                result.insert(0,"+");
            }
        }
        return result;
    }
    void print()const
    {
        gmp_printf("实部:%Ff\n",this->realPart);
        gmp_printf("虚部:%Ff\n",this->imaginaryPart);
        cout<<this->realdata()<<endl;
        cout<<this->imagdata()<<endl;
    }
};

int main()
{
 // 为 a 赋值，a=0
    Complex Com_Result=Complex();
    
    string key;//变量名
    string value;//存储的值
    string buf;
    int idx_equal=0;
    int prec_set=0;
    // cout<<"请输入你要的精度:";
    // cin>>prec_set;
    // getchar();
    // mpf_set_default_prec(prec_set);

    while(1){
        cout<<">>> ";
        Invalid=0;
        getline(cin,buf);//从输入流中读取一行数据到buf
        BlankFliter(buf);
        if(buf.find("=")!= std::string::npos){
            bool is_exist=0;
            idx_equal=buf.find("=");
            key=buf.substr(0,idx_equal);
            value=buf.substr(idx_equal+1,buf.length()-idx_equal-1);//a=2+3  len=5 idx_equal=1  传入的大小是5-1-1
            NameTraslate(value);
            NameTraslate(value);
            BlankFliter(value);
            ModTraslate(value);
            ConTraslate(value);
            CountEr(value);
            if(NameCheck(key)){
                umap[key]=value;
            }else{
                cout<<"变量名格式错误!请重新命名..."<<endl;
            }//这里要处理value的值
        }//第二个是调用变量名  
        else if(umap.find(buf)!=umap.end()){//判断迭代器是否到尾部,到尾部说明没有这个变量
            cout<<umap[buf]<<endl;
        }
        else{
            NameTraslate(buf);
            NameTraslate(buf);
            ModTraslate(buf);
            ConTraslate(buf);
            BlankFliter(buf);
            CountEr(buf);
            if(!Invalid){
                cout<<buf<<endl;
            }
        }
        //表达式
   }
    return 0;
}

void CountEr(string &buf){//引用修改是直接修改变量    //当然对于一些特殊符号，要在前面进行语义解析和翻译工作.     //比如mod运算，和++运算，还有共轭复数运算,甚至是变量
    char oper;//操作符
    Complex result= Complex();//最后的结果。
    Complex temp_result=Complex();
    Complex zero_com=Complex();
    vector<Complex> mulsave(0);
    int idx_i;//虚数i的下标//科学计数法e的下标
    int idx_oper=0;
    int idx_num2_end;
    int idxoffset=0;
    int pre_idx=0;
    stack<char> opstack1;
    stack<char> opstack2;
    stack<string> bufstack1;
    stack<string> bufstack2;

    //去除空格之后，通过运算符 来筛选操作符和操作数。
    for(int i=0;i<buf.length();i++){
        int idx_e=buf.find('E',i);//找到E的位置
        if(idx_e!= std::string::npos){
            if(buf[idx_e+1]=='-'){//如果e的下个字符是-号的话,换成不处理的符号
              buf[idx_e+1]='#';
            }
        }
        if(buf[i]=='+'||buf[i]=='-'||buf[i]=='*'||buf[i]=='/'||buf[i]=='('||buf[i]==')'){
            idxoffset=i-idx_oper;
            idx_oper=i;
            
            if(pre_idx==0||idxoffset>1){// 1*(2+3)  *和(下标差距为1   idx_oper=1, i=2  idxoffset=1
                string temp;
                if(pre_idx==0){
                    temp=buf.substr(pre_idx,idxoffset);
                }
                else if(pre_idx!=0){
                    temp=buf.substr(pre_idx,idxoffset-1);
                }
                ETraslate(temp);
                bufstack1.push(temp);//2+3+6   则插入3
                if(buf[idx_oper+1]=='('){
                    pre_idx=idx_oper+2;
                }else{
                    pre_idx=idx_oper+1;
                }

            }
            opstack1.push(buf[i]);
            
        }
        if(buf[i]!=')'&&i==buf.length()-1){//到字符串末尾   idx_op=3  i=4  idx_offset=1 buf.length=5
            string temp;
            if(pre_idx==0){
                temp=buf.substr(idx_oper,buf.length()-idx_oper);
            }else{
                temp=buf.substr(idx_oper+1,buf.length()-idx_oper-1);
            }
            ETraslate(temp);
            bufstack1.push(temp);//2+3-6  插入6   
        }
    }
    //利用另外两个栈,实现后缀表达式
    bufstack2.push(bufstack1.top());//先入栈一个操作数
    bufstack1.pop();
    
    //入栈第二个操作数，入栈第一个操作符，防止访问空栈
    if(!opstack1.empty()&&opstack1.top()==')'){//第一个括号都是)先入栈
        opstack2.push(opstack1.top());
        opstack1.pop();
    }

    if(!opstack1.empty()){
        opstack2.push(opstack1.top());
        opstack1.pop();
    }
    if(!bufstack1.empty()){
        bufstack2.push(bufstack1.top());
        bufstack1.pop();
    }

    if(opstack2.empty()&&bufstack2.size()==1){
        ETraslate(buf);
        for(int i=0;i<buf.length();i++){
            if(isalpha(buf[i])&&buf[i]!='i'){
                
                cerr<<"存在非法变量"<<endl;
                Invalid=1;
                return ;
            }
        }
        buf=bufstack2.top();
        bufstack2.pop();
        return;//只有一个值,就不管他,直接返回
    }
    while(bufstack2.top()!="@"||bufstack2.size()>1||!opstack1.empty()||!opstack2.empty()){//依据操作符栈1是否为空判断是否结束
        if((!opstack1.empty())){
            if(!opstack1.empty()&&!opstack2.empty()&&(WhoFirst(opstack1.top())>=WhoFirst(opstack2.top()))){//来者的优先级比较大,就不管它
                if(opstack1.top()=='('&&opstack2.top()==')'){
                    opstack1.pop();
                    opstack2.pop();
                }
                opstack2.push(opstack1.top());
                opstack1.pop();
                bufstack2.push(bufstack1.top());
                bufstack1.pop();
            }else if(!opstack1.empty()&&!opstack2.empty()&&(WhoFirst(opstack1.top())<WhoFirst(opstack2.top()))){//来者的优先级比较小,我们就要进行运算。
//这里的运算也要判断是否为虚数i，先放进类
            char op=opstack2.top();
            opstack2.pop();
            switch (op){
                case '+':
                    //bufstack2.top()=bufstack2.top()+bufstack1.top();//进行运算
                    if(bufstack2.top() =="@" && mulsave.size()!=0){
                        result=result+mulsave.back();
                        mulsave.pop_back();
                        bufstack2.pop();
                    }else{
                        if(bufstack2.top()=="@")
                            result=result+"0";
                        else
                            result=result+bufstack2.top();
                        cout<<"2+1:     "<<endl;
                        bufstack2.pop();//把0pop掉
                    }
                    if(bufstack2.top() =="@" && mulsave.size()!=0){
                        result=result+mulsave.back();
                        mulsave.pop_back();
                        bufstack2.pop();
                    }else{
                        if(bufstack2.top()=="@")
                            result=result+"0";
                        else
                            result=result+bufstack2.top();
                        bufstack2.pop();//把0pop掉
                    }
                    bufstack2.push("@");//推入0,是为了代替运算后的结构的数据,因为运算后的结果已经保存到COM中了
                    break;
                case '-':
                    //bufstack2.top()=bufstack2.top()+bufstack1.top();//进行运算
                    if(bufstack2.top() =="@" && mulsave.size()!=0){
                        result=result+mulsave.back();
                        mulsave.pop_back();
                        bufstack2.pop();
                    }else{
                        if(bufstack2.top()=="@")
                            result=result+"0";
                        else
                            result=result+bufstack2.top();
                        bufstack2.pop();//把0pop掉
                    }
                    if(bufstack2.top() =="@" && mulsave.size()!=0){
                        result=result-mulsave.back();
                        mulsave.pop_back();
                        bufstack2.pop();
                    }else{
                        if(bufstack2.top()=="@")
                            result=result+"0";
                        else
                            result=result-bufstack2.top();
                        bufstack2.pop();//把0pop掉
                    }
                    bufstack2.push("@");//推入0,是为了代替运算后的结构的数据,因为运算后的结果已经保存到COM中了
                    break;
                case '*':
                    //bufstack2.top()=bufstack2.top()+bufstack1.top();//进行运算
                    temp_result=zero_com;
                    if(bufstack2.top() =="@" && mulsave.size()!=0){
                        temp_result=mulsave.back();
                        mulsave.pop_back();
                        bufstack2.pop();
                    }else{
                        if(bufstack2.top()=="@")
                            temp_result=temp_result+"0";
                        else
                            temp_result=temp_result+bufstack2.top();
                        bufstack2.pop();//把0pop掉
                    }
                    if(bufstack2.top() =="@" && mulsave.size()!=0){
                        temp_result=temp_result*mulsave.back();
                        mulsave.pop_back();
                        bufstack2.pop();
                    }else{
                        if(bufstack2.top()=="@")
                            temp_result=temp_result+"0";
                        else
                            temp_result=temp_result*bufstack2.top();
                        bufstack2.pop();//把0pop掉
                    }
                    bufstack2.push("@");//推入0,是为了代替运算后的结构的数据,因为运算后的结果已经保存到COM中了
                    mulsave.push_back(temp_result);
                    break;
                case '/':
                    //bufstack2.top()=bufstack2.top()+bufstack1.top();//进行运算
                    temp_result=zero_com;
                    if(bufstack2.top() =="@" && mulsave.size()!=0){
                        temp_result=mulsave.back();
                        mulsave.pop_back();
                        bufstack2.pop();
                    }else{
                        if(bufstack2.top()=="@")
                            temp_result=temp_result+"0";
                        else
                            temp_result=temp_result+bufstack2.top();
                        bufstack2.pop();//把0pop掉
                    }
                    if(bufstack2.top() =="@" && mulsave.size()!=0){
                        temp_result=temp_result/mulsave.back();
                        mulsave.pop_back();
                        bufstack2.pop();
                    }else{
                        if(bufstack2.top()=="@")
                            temp_result=temp_result+"0";
                        else
                            temp_result=temp_result/bufstack2.top();
                        bufstack2.pop();//把0pop掉
                    }//推入0,是为了代替运算后的结构的数据,因为运算后的结果已经保存到COM中了
                    mulsave.push_back(temp_result);
                    break;
                case '(':
                    //不断计算算直到遇到 (
                    if(opstack2.top()!=')'){
                        ///////////////////////////////////////////////////////////////////////////////////////////////////
                        switch (op){//用上面的东西再运算一遍
                            case '+':
                                //bufstack2.top()=bufstack2.top()+bufstack1.top();//进行运算
                                if(bufstack2.top() =="@" && mulsave.size()!=0){
                                    result=result+mulsave.back();
                                    mulsave.pop_back();
                                    bufstack2.pop();
                                }else{
                                    if(bufstack2.top()=="@")
                                        result=result+"0";
                                    else
                                        result=result+bufstack2.top();
                                    bufstack2.pop();//把0pop掉
                                }
                                if(bufstack2.top() =="@" && mulsave.size()!=0){
                                    result=result+mulsave.back();
                                    mulsave.pop_back();
                                    bufstack2.pop();
                                    
                                }else{
                                    if(bufstack2.top()=="@")
                                        result=result+"0";
                                    else
                                        result=result+bufstack2.top();
                                    bufstack2.pop();//把0pop掉
                                }
                                bufstack2.push("@");//推入0,是为了代替运算后的结构的数据,因为运算后的结果已经保存到COM中了
                                break;
                            case '-':
                                //bufstack2.top()=bufstack2.top()+bufstack1.top();//进行运算
                                if(bufstack2.top() =="@" && mulsave.size()!=0){
                                    result=result+mulsave.back();
                                    mulsave.pop_back();
                                    
                                     bufstack2.pop();
                                }else{
                                    if(bufstack2.top()=="@")
                                        result=result+"0";
                                    else
                                        result=result+bufstack2.top();
                                    bufstack2.pop();//把0pop掉
                                }
                                if(bufstack2.top() =="@" && mulsave.size()!=0){
                                    result=result-mulsave.back();
                                    mulsave.pop_back(); 
                                    bufstack2.pop();
                                }else{
                                    if(bufstack2.top()=="@")
                                        result=result+"0";
                                    else
                                        result=result-bufstack2.top();
                                    bufstack2.pop();//把0pop掉
                                }
                                bufstack2.push("@");//推入0,是为了代替运算后的结构的数据,因为运算后的结果已经保存到COM中了
                                break;
                            case '*':
                                temp_result=zero_com;
                                if(bufstack2.top() =="@" && mulsave.size()!=0){
                                    temp_result=mulsave.back();
                                    mulsave.pop_back();
                                
                                    bufstack2.pop();
                                }else{
                                    if(bufstack2.top()=="@")
                                        temp_result=temp_result+"0";
                                    else
                                        temp_result=temp_result+bufstack2.top();
                                    bufstack2.pop();//把0pop掉
                                }
                                if(bufstack2.top() =="@" && mulsave.size()!=0){
                                    temp_result=temp_result*mulsave.back();
                                    mulsave.pop_back();
                                    
                                    bufstack2.pop();
                                }else{
                                    if(bufstack2.top()=="@")
                                        temp_result=temp_result+"0";
                                    else
                                        temp_result=temp_result*bufstack2.top();
                                    bufstack2.pop();//把0pop掉
                                }
                                bufstack2.push("@");//推入0,是为了代替运算后的结构的数据,因为运算后的结果已经保存到COM中了
                                mulsave.push_back(temp_result);
                                break;
                            case '/':
                                temp_result=zero_com;
                                //bufstack2.top()=bufstack2.top()+bufstack1.top();//进行运算
                                if(bufstack2.top() =="@" && mulsave.size()!=0){
                                    temp_result=mulsave.back();
                                    mulsave.pop_back();
                                    
                                    bufstack2.pop();
                                }else{
                                    if(bufstack2.top()=="@")
                                        temp_result=temp_result+"0";
                                    else
                                        temp_result=temp_result+bufstack2.top();
                                    bufstack2.pop();//把0pop掉
                                }
                                if(bufstack2.top() =="@" && mulsave.size()!=0){
                                    temp_result=temp_result/mulsave.back();
                                    mulsave.pop_back();
                                    
                                    bufstack2.pop();
                                }else{
                                    if(bufstack2.top()=="@")
                                        temp_result=temp_result+"0";
                                    else
                                        temp_result=temp_result/bufstack2.top();
                                    bufstack2.pop();//把0pop掉
                                }
                                bufstack2.push("@");//推入0,是为了代替运算后的结构的数据,因为运算后的结果已经保存到COM中了
                                mulsave.push_back(temp_result);
                                break;
                            default:
                                break;
                        ////////////////////////////////////////////////////////////////////////////////////////////////////
                        }
                    }else{
                        cout<<"将)出栈"<<endl;
                        opstack2.pop();//将stack2的op )出栈
                    }
                default:
                    break;
                }
            }else{//opstack1不空，但是opstack2空。
                if(opstack1.top()=='('&&opstack2.top()==')'){
                    opstack1.pop();
                    opstack2.pop();
                }
                opstack2.push(opstack1.top());
                opstack1.pop();
                bufstack2.push(bufstack1.top());
                bufstack1.pop();
            }
        } //opstack2.size()<1
        else {//只有一个符号或者为空.
            if(opstack1.empty()){//符号栈空了，说明已经用完了，应该计算
                //要么是运算符号
                while (!opstack2.empty()){
                    char op=opstack2.top();
                    opstack2.pop();
                    switch (op){
                    case '+':
                        //bufstack2.top()=bufstack2.top()+bufstack1.top();//进行运算
                        if(bufstack2.top() =="@" && mulsave.size()!=0){
                            result=result+mulsave.back();
                            mulsave.pop_back();
                            
                            bufstack2.pop();
                        }else{
                            if(bufstack2.top()=="@")
                                result=result+"0";
                            else
                                result=result+bufstack2.top();
                            bufstack2.pop();//把0pop掉
                        }
                        if(bufstack2.top() =="@" && mulsave.size()!=0){
                            result=result+mulsave.back();
                            mulsave.pop_back();
                        
                            bufstack2.pop();
                        }else{
                            if(bufstack2.top()=="@")
                                result=result+"0";
                            else
                                result=result+bufstack2.top();
                            bufstack2.pop();//把0pop掉
                        }
                        bufstack2.push("@");//推入0,是为了代替运算后的结构的数据,因为运算后的结果已经保存到COM中了
                        break;
                    case '-':
                        //bufstack2.top()=bufstack2.top()+bufstack1.top();//进行运算
                        if(bufstack2.top() =="@" && mulsave.size()!=0){
                            result=result+mulsave.back();
                            mulsave.pop_back();
                        }else{
                            if(bufstack2.top()=="@")
                                result=result+"0";
                            else
                                result=result+bufstack2.top();
                            bufstack2.pop();//把0pop掉
                        }
                        if(bufstack2.top() =="@" && mulsave.size()!=0){
                            result=result-mulsave.back();
                            mulsave.pop_back();
                        }else{
                            if(bufstack2.top()=="@")
                                result=result+"0";
                            else
                                result=result-bufstack2.top();
                            bufstack2.pop();//把0pop掉
                        }
                        bufstack2.push("@");//推入0,是为了代替运算后的结构的数据,因为运算后的结果已经保存到COM中了
                        break;
                    case '*':
                        temp_result=zero_com;
                        if(bufstack2.top() =="@" && mulsave.size()!=0){
                            temp_result=temp_result+mulsave.back();
                            mulsave.pop_back();
                            bufstack2.pop();
                        }else{
                            if(bufstack2.top()=="@")
                                temp_result=temp_result+"0";
                            else
                                temp_result=temp_result+bufstack2.top();
                            bufstack2.pop();//把0pop掉
                        }
                        if(bufstack2.top() =="@" && mulsave.size()!=0){
                            temp_result=temp_result*mulsave.back();
                            mulsave.pop_back();
                            bufstack2.pop();
                        }else{
                            if(bufstack2.top()=="@")
                                temp_result=temp_result+"0";
                            else
                                temp_result=temp_result*bufstack2.top();
                            bufstack2.pop();//把0pop掉
                        }
                        bufstack2.push("@");//推入0,是为了代替运算后的结构的数据,因为运算后的结果已经保存到COM中了
                        mulsave.push_back(temp_result);
                        break;
                    case '/':
                        temp_result=zero_com;
                        //bufstack2.top()=bufstack2.top()+bufstack1.top();//进行运算
                        if(bufstack2.top() =="@" && mulsave.size()!=0){
                            temp_result=temp_result+mulsave.back();
                            mulsave.pop_back();
                            bufstack2.pop();
                        }else{
                            if(bufstack2.top()=="@")
                                temp_result=temp_result+"0";
                            else
                                temp_result=temp_result+bufstack2.top();
                            bufstack2.pop();//把0pop掉
                        }
                        if(bufstack2.top() =="@" && mulsave.size()!=0){
                            temp_result=temp_result/mulsave.back();
                            mulsave.pop_back();
                            bufstack2.pop();
                        }else{
                            if(bufstack2.top()=="@")
                                temp_result=temp_result+"0";
                            else
                                temp_result=temp_result/bufstack2.top();
                            bufstack2.pop();//把0pop掉
                        }
                        bufstack2.push("@");//推入0,是为了代替运算后的结构的数据,因为运算后的结果已经保存到COM中了
                        mulsave.push_back(temp_result);
                        break;
                    }
                }
            }else{
                cerr<<"异常操作"<<endl;//此时opstack2为空，opstack1还没有操作
                exit(0);
            }
        }
    }
    if(mulsave.size()==1){
        result=mulsave.back();
        mulsave.pop_back();
    }
    ETraslate(buf);
    for(int i=0;i<buf.length();i++){
        if(isalpha(buf[i])&&buf[i]!='i'){
            cerr<<"存在非法变量"<<endl;
            Invalid=1;
        }
    }
    //对应输出
    if(Out_State == 0){
        buf=result.alldata();
        Out_State=0;
    }else if(Out_State == 1){
        buf=result.mod();
        Out_State=0;
    }else if(Out_State == 2 ){
        //buf=result.con();
        Out_State=0;
    }
    return ;
}
void NameTraslate(string &buf){
    string::const_iterator it = buf.begin();
    string::const_iterator word_start;
    string::const_iterator word_end;
    string word;
    int idx=0;
    int next_idx=0;
    for (const auto& it : umap) {
		idx=buf.find(it.first,0);
        if(idx!= std::string::npos){//如果不等于，说明找到了
            next_idx=idx+it.first.length();
            buf.replace(idx,it.first.length(),it.second);//用值替换对应对的键
        }
	}
    if(buf.find('_')!=std::string::npos){//如果还找到相对应对的标识符,输出,未定义对的变量.
        cerr<<"未定义的变量"<<endl;
    }
}
bool NameCheck(string &buf){
    if((buf=="mod")||(buf=="con")||(buf=="i")||(buf=="E")){
        cerr<<"变量名和关键字方法相同,请重新命名..."<<endl;
    }
    if(std::isalpha(buf[0])||buf[0]=='_'){
        for(int i=1;i<buf.length();i++){
            if(std::isalpha(buf[i])||buf[i]=='_'||('0'<=buf[i]&&buf[i]<='9')){
            }else{
               cerr<<"变量名不符合规范..."<<endl;
                return 0;
            }
        }
    }else{
        cerr<<"变量名首字符不符合规范..."<<endl;
        return 0;
    }
    return 1;
}
void ModTraslate(string &buf){  //mod(3+5i)
    Complex result=Complex();
    int find_idx=buf.find("mod(",0);
    int end_idx=buf.find(")",find_idx);
    int start_idx=find_idx+4;
    string temp;
    if(find_idx!=std::string::npos||end_idx!=std::string::npos){
        Out_State=1;
        temp=buf.substr(start_idx,end_idx-start_idx);
        CountEr(temp);
        result=result+temp;
        temp=result.mod();
        buf.replace(find_idx,end_idx-find_idx+1,temp);
    }
}
void ConTraslate(string &buf){
    Complex result=Complex();
    int find_idx=buf.find("con(",0);
    int end_idx=buf.find(")",find_idx);
    int start_idx=find_idx+4;
    string temp;
    if(find_idx!=std::string::npos||end_idx!=std::string::npos){
        Out_State=2;
        temp=buf.substr(start_idx,end_idx-start_idx);
        CountEr(temp);
        result=result+temp;
        temp=result.con();
        buf.replace(find_idx,end_idx-find_idx+1,temp);
    }
}
void ETraslate(string &buf){
    string s=buf;
    int idx_e = s.find("E");
    int idx_ch =s.find("#");
    int idx_i=s.find("i");
    bool flag_i=0;
    
    if(idx_e!= std::string::npos){
        if(idx_i!=std::string::npos){
            buf.pop_back();//把i给pop掉
            flag_i=1;
        }
        if(idx_ch!= std::string::npos){//如果存在的话{
            buf[idx_ch]='-';
        }
        string base = s.substr(0, idx_e); //获取小数部分
        int exp = stoi(s.substr(idx_e + 1)); //获取指数部分
        buf=AdjustResult(exp,base);
        if(flag_i){
            buf.append("i");
        }
    }else {
        return ;
    }
}
bool CheckImga(string &buf)
{
    for(int i=0;i<buf.length();i++){
        if(buf[i]=='i')
            return true;
    }
    return false;
}


void BlankFliter(string &str)//使用C++string类的方法快速删除空格
{
    //std::remove_if函数用于将所有空白字符移动到字符串的末尾，
    //然后使用erase函数将它们从字符串中删除。
    //::isspace函数是一个标准库函数，用于检查一个字符是否为空白字符。
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end()); 
}

int WhoFirst(char op)
{
	if(op=='(')
	return 1;
	else if(op=='+'||op=='-')
	return 2;
	else if(op=='*'||op=='/')
	return 3;
    if(op==')')
    return 1;
    else
    cerr<<"错误的字符输入"<<endl;
}

string AdjustResult(int exp,string &buf){
    string adjust_result;
    if (exp < 0) {
        // 计算小数点需要向左移动的位数
        int move = -exp;
        adjust_result = "0." + std::string(move, '0') + buf;
    } else {
        // 在整数部分后面拼接exp个"0"
        adjust_result = buf + std::string(exp, '0');
    }
    return adjust_result;
}
string MpfAdjustResult(int exp,string &buf){
    int len=buf.length();
    if (exp <= 0) {
        if(len){
                    // 计算小数点需要向左移动的位数
            int move = -exp;
            buf = "0." + std::string(move, '0') + buf;
        }
    } else if(exp>0){
        if(len>exp){//对于长度大于exp的小数
            buf.insert(exp,1,'.');
            return buf;
        }else if(len==exp){//对于小数点刚好在其数字后面的整数
            return buf;
        }
        exp=exp-len;
        buf = buf + std::string(exp, '0');
    }else{
        return buf;
    }
    return buf;
}