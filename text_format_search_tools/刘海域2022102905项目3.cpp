#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <stack>
#include <set>
#include <algorithm>

using namespace std;
#define MAX 1024
unordered_map<string,string>umap{

};
string IsNameReturn(string &buf);
string filename;
bool filesearch=0;
bool FileExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}
//返回的结果类
class QueryResult{
    
    private:
        string inputword;//需要查询的单词
        shared_ptr<set<unsigned>> idx;//出现的行号
        shared_ptr<vector<string>> file;//查找的文件
    public:
    QueryResult(string s,shared_ptr<set<unsigned>> i,shared_ptr<vector<string>> f):inputword(s),idx(i),file(f){

    }
    void print()
    {
        cout<<"\""<<this->inputword<<"\"出现了"<<this->idx->size()<<"次"<<endl;
        //打印出现的每一行
        for(auto num:*(this->idx)){
            cout<<" (line "<<num+1<<") ";
            cout<<*(this->file->begin()+num)<<endl;
        }
    }
    void write(ofstream& ofs){
        ofs<<"\""<<this->inputword<<"\"出现了"<<this->idx->size()<<"次"<<endl;
        for(auto num:*(this->idx)){
            ofs<<" (line "<<num+1<<") ";
            ofs<<*(this->file->begin()+num)<<endl;
        }
        // for(const string &s:newbuf){
        //     ofs<<s<<endl;
        // }
    }
    shared_ptr<vector<string>> getfile(){//返回file的共享指针
        return file;
    }
    shared_ptr<set<unsigned>> getset(){//返回行号的共享指针
        return idx;
    }
    shared_ptr<set<unsigned>> getfileset(){//返回整体行号的共享指针
        shared_ptr<set<unsigned>> fileset =make_shared<set<unsigned>>();;
        for(int i=0;i<file->size();i++){
            fileset->insert(i);
        }
        return fileset;
    }

};
class TextQuery{
    private:
    shared_ptr<vector<string>> file;//存储文章的向量只能得指针
    map<string,shared_ptr<set<unsigned>>> wordmap;//用unsigned存储行号
    public:
    //初始化需要一个文件流。
    TextQuery(ifstream& is):file(new vector<string>)//对文件流的单词句子进行解读
    {
        string text;
        while(getline(is,text))//获取每一行
        {//file是一个vector对象
            file->push_back(text);
            int n=file->size()-1;//也就是行号？下标？
            istringstream line(text);
            string word;
            while(line >> word)//用string这个流分解单词
            {//匹配每一个单词
                shared_ptr<set<unsigned>> lines;//map的值是一个(set对象的)智能指针
                if(!wordmap.count(word)){//如果不存在，说明是新单词，开辟一个set空间，并初始化
                    lines = make_shared<set<unsigned>>();
                    wordmap[word] = lines;
                }
                wordmap[word]->insert(n);
            }
        }
    }
    QueryResult query(const string& inputword)const//返回查找结果，查找结果是一个queryRE对象
    {//如果未找到，就直接返回一个指向空的set的set共享指针
        static shared_ptr<set<unsigned>> nodata(new set<unsigned>());//返回智能指针指向空set
        auto loc=wordmap.find(inputword);
        if(loc == wordmap.end()){
            return QueryResult(inputword,nodata,file);
        }else{
            return QueryResult(inputword,loc->second,file);
        }
    }

};



class QueryBase{
    friend class Query;
protected:
    unsigned idx;
    virtual ~QueryBase(){};
private:
    virtual QueryResult eval(const TextQuery&)const{}
    virtual string rep()const{}//生成用于查询的rep文本
};

class WordQuery : public QueryBase{
    friend class Query;
public:
    ~WordQuery() override{
        
    }
private:
    //构造函数，接收string
    WordQuery(const string &s):query_word(s){}
    //重载虚构函数
    QueryResult eval(const TextQuery &t)const{
        return t.query(query_word);//wordquery返回的结果就是根据单词本身返回的QueryRE
    }
    string rep() const{
        return query_word;
    }
    string query_word;
    
};

class Query{
    //对三个运算符友元
    friend Query operator ~(const Query&);
    friend Query operator |(const Query& ,const Query &);
    friend Query operator &(const Query& ,const Query &);
private:
    //构造函数shared_ptr
    //传进来是 指向QueryBase类型的 共享指针
    shared_ptr<QueryBase> q;//指向基类的指针
public:
    Query(shared_ptr<QueryBase> query):q(query){};
    Query& operator =(const Query& que){
        q=que.q;
        return *this;
    }
    Query(const string &s){
        shared_ptr<WordQuery> ptr(new WordQuery(s));
        q=ptr;
    }//传进来是string类型的 用来构建WordQuery,来创建Query的q成员
    QueryResult eval(const TextQuery &t)const{
        return q->eval(t);
    }
    string rep ()const{
        return q->rep();
    }
    friend ostream & operator<<(ostream &os,const Query& query)
    {
        return os << query.rep();
    }
};

class NotQuery : public QueryBase{
    friend Query operator ~(const Query &);
public:
    ~NotQuery() override{
        
    }
private:
    NotQuery(const Query &q):query(q){}
    string rep()const{
        return "~("+query.rep()+")";
    }
    QueryResult eval(const TextQuery & text)const {
        auto sml=query.eval(text);
        auto smlset=sml.getset();
        auto allset=sml.getfileset();
        set<unsigned> resultset;
        //包含关系，差集即是非集，大减小即可。集合本身因为迭代特性所以本身就有序。
        set_difference(allset->begin(),allset->end(),smlset->begin(),smlset->end(),inserter(resultset,resultset.begin()));
        shared_ptr<set<unsigned>> resultptr=make_shared<set<unsigned>>(resultset);
        return QueryResult(rep(),resultptr,sml.getfile());
    }
    Query query;
};
inline Query operator~ (const Query &operand){
    return shared_ptr<QueryBase>(new NotQuery(operand));
}
class BinaryQuery : public QueryBase{
    friend Query operator ~(const Query &);
    protected:
    BinaryQuery(const Query &l,const Query &r,string s):lhs(l),rhs(r),opSym(s){}
    string rep()const{
        return "("+lhs.rep()+" "+opSym+" "+rhs.rep()+")";
    }
    Query lhs,rhs;
    string opSym;
};

class OrQuery : public BinaryQuery{
    friend Query operator |(const Query &,const Query &);
public:
    ~OrQuery() override{
        
    }
private:
    OrQuery(const Query &l,const Query & r):BinaryQuery(l,r,"|"){}
    QueryResult eval(const TextQuery &text)const {
        //两个成员分别lhs,rhs。求并集不妨找他们返回的set的共享指针，然后用算法求
        //lhs和rhs都是WordQuery
        auto r=rhs.eval(text),l=lhs.eval(text);
        auto retlines=l.getset();
        retlines->insert(r.getset()->begin(),r.getset()->end());//左侧的插入右侧形成并集
        return QueryResult(rep(),retlines,l.getfile());//两个file都一样
    }

};
inline Query operator| (const Query &lhs,const Query &rhs){
    return shared_ptr<QueryBase>(new OrQuery(lhs,rhs));
}
class AndQuery : public BinaryQuery{
    friend Query operator &(const Query &,const Query &);
public:
    ~AndQuery() override{
        
    }
private:
    AndQuery(const Query &l,const Query & r):BinaryQuery(l,r,"&"){}
    QueryResult eval(const TextQuery &text)const {
        //两个成员分别lhs,rhs。求交集不妨找他们返回的set的共享指针，然后用算法求
        //lhs和rhs都是WordQuery
        auto r=rhs.eval(text),l=lhs.eval(text);//R和L是ResultQE
        auto lset=l.getset(),rset=r.getset();
        set<unsigned> resultset;
        set_intersection(lset->begin(),lset->end(),rset->begin(),rset->end(),inserter(resultset,resultset.begin()));
        shared_ptr<set<unsigned>> resultptr=make_shared<set<unsigned>>(resultset);
        return QueryResult(rep(),resultptr,l.getfile());
    }
};
inline Query operator& (const Query &lhs,const Query &rhs){
    return shared_ptr<QueryBase>(new AndQuery(lhs,rhs));
}
//文件打开写入操作流
class OFileStream{
    private:
    string FilePath;
    ofstream ofile;
    public:
    OFileStream(const string &name){
        ofile.close();
        FilePath=name;
        if(FileExists(name)){
            cout<<"文件存在，是否进行覆写？(Y/N):";
            char response;
            cin>>response;
            cin.ignore();
            if(response=='Y'||response=='y'){
                ofile.close();
                ofile.open(name,ios::out);
                if(ofile.is_open()){//判断文件是否已经创建流
                    cout<<"文件"<<name<<"成功打开，请写入 :)"<<endl;
                }else{
                    cerr<<"文件"<<name<<"打开出现异常 :("<<endl;
                }
            }else{
                cout<<"保留原有文件，不进行操作"<<endl;
            }
        }else{
            //文件不存在的情况
            ofile.close();
            ofile.open(name,ios::out);
            if(ofile.is_open()){//判断文件是否已经创建流
                cout<<"文件"<<name<<"成功打开，请写入 :)"<<endl;
            }else{
                char ch='0';
                throw ch;
            }
        }
    }
    ~OFileStream(){
        FilePath="";
        if(ofile.is_open()){
            ofile.close();
        }
    }
    ofstream& getofsteram(){
        return ofile;
    }
};
//文件读取操作流
class IFileStream{
    private:
    string FilePath;
    ifstream ifile;
    public:
    IFileStream(const string &name){
        FilePath=name;
        if(ifile.is_open()){//判断文件是否已经创建流
            cout<<"文件"<<name<<"已被打开过... :|"<<endl;
        }
        else{
            ifile.open(name);
            if(ifile.is_open()){//判断文件是否已经创建流
                cout<<"文件"<<name<<"可以读取 :)"<<endl;
            }else{
                string something="文件"+name+"读取出现异常,不存在的路径 :(";
                throw something;
            }
        }
    }
    ~IFileStream(){
        FilePath="";
        if(ifile.is_open()){
            ifile.close();
        }
    }

    ifstream& getifsteram(){
        return ifile;
    }
};
class FileFormatter{
    private:
    OFileStream ost;
    IFileStream ist;
    vector<string> vecbuf;
    vector<string> newbuf;
    unsigned num;
    public:
    FileFormatter(const string FilePath,const string newFilePath,unsigned n):ist(FilePath),ost(newFilePath),num(n),vecbuf(),newbuf(){
    }
    void readFromFile()//读取源文件
    {//ist是读取文件的留，我们利用它把文件读取到指定的容器vector中。
    //然后再对其进行格式化。所以此函数的作用就是将ist的内容读取出来放到vector中。
        //如果文件不存在，初始化的时候会报错
        //该函数作用域结束后会调用析构函数，不会造成资源浪费。
        stringstream ss;
        string temp,t;
        char c[num+10];
        while (getline(ist.getifsteram(),temp)){
           //temp=c;
           ss<<temp;
           while(ss >> t){
                vecbuf.push_back(t);//把每个单词放在容器里。
           }
           ss.clear();//提取完一行清除字符串流
        }
    }
    void formatText()//格式化字符句子
    {//如果单词字符数大于n，则单独独占一行
        //外层循环的结束条件是vector是否用完
        while(vecbuf.size()){
            string str="";
            if(vecbuf.front().length()>=num){
                newbuf.push_back(vecbuf.front());
                vecbuf.erase(vecbuf.begin());//删除第一个元素
            }else{
                while(str.length()<num){//对于一个句子，当前句子长度+单词长度大于num就不执行了
                bool flag=vecbuf.front()=="."||vecbuf.front()==";"||vecbuf.front()=="!"||vecbuf.front()=="\""||vecbuf.front()==","||vecbuf.front()==":"||vecbuf.front()=="?";
                    if(str.length()==0){ 
                        //检测巨首是不是标点
                        if(flag){//如果是标点符号，进行处理。
                            cerr<<"句首出现标点符号"<<endl;
                        }
                        if(!vecbuf.empty()){
                            str=vecbuf.front();
                            vecbuf.erase(vecbuf.begin());
                        }
                    }//如果小于，就继续做
                    else if((str.length()+vecbuf.front().length()+1)<=num&&!flag){//句子加单词长度大于num，而不是加符号
                        if(!vecbuf.empty()){
                            str=str+" "+vecbuf.front();
                            vecbuf.erase(vecbuf.begin());
                        }
                    }
                    else if(flag){
                        while(flag){
                            if(!vecbuf.empty()){//如果句尾是标点连着的话，就连续处理
                                str=str+" "+vecbuf.front();
                                vecbuf.erase(vecbuf.begin());
                                flag=vecbuf.front()=="."||vecbuf.front()==";"||vecbuf.front()=="!"||vecbuf.front()=="\""||vecbuf.front()==","||vecbuf.front()==":"||vecbuf.front()=="?";
                            }else{
                                flag=false;
                            }
                        }
                        flag=false;
                    }
                    else{
                        break;
                    }
                    if(vecbuf.empty()){
                        break;
                    }
                }
                newbuf.push_back(str);
            }
        }

    }
    void saveToFile()//将格式化后的文本内容保存到新的位置和文件名。
    {   
        ofstream& outfile=ost.getofsteram();
        for(const string &s:newbuf){
            outfile<<s<<endl;
        }
    }
};
shared_ptr<IFileStream> ifs;
shared_ptr<FileFormatter> ff;
void BlankFliter(string &str)//使用C++string类的方法快速删除空格
{
    //std::remove_if函数用于将所有空白字符移动到字符串的末尾，
    //然后使用erase函数将它们从字符串中删除。
    //::isspace函数是一个标准库函数，用于检查一个字符是否为空白字符。
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end()); 
}
void ExpTraslate(string &buf);
string IsNameReturn(string &buf){//针对一个单词的一次性检查
    string result;
    auto iter = umap.find(buf);
    if(iter!=umap.end()){
        result=iter->second;
        return result;
    }
    return buf;
}
void StringTraslate(string &buf)//处理用户输入的字符串
{//首先判断是否执行 查询操作
    if(buf[0]=='='){
        buf.erase(buf.begin());
    }
//然后判断是否执行 赋值 操作
    int temp=0;
    int times=0;
    string name,value;


    for(int i=0;i<buf.size()-1;i++){
        if(buf[i]=='>'&&buf[i+1]=='>'){  
            filename=buf.substr(i+2,buf.size()-i-2);
            buf=buf.substr(0,i);//前面是查询的单词
            filesearch=1;
            break;
        }
    }
    for(int i=1;i<buf.length();i++){//跳过第一个字符，检查是否有赋值操作

        if(buf[i]=='='){
            for(int j=i;j<buf.length();j++){
                if(buf[j]=='\"'&&times==0){//找到"说明是常量
                    temp=j+1;
                    times++;
                }else if(buf[j]=='\"'&&times==1){
                    value=buf.substr(temp,j-temp);
                    times++;
                    break;
                }
                if(j==buf.length()-1){//找到最后一位的时候，说明这可能是个变量
                    string qwq=buf.substr(temp,j-temp);
                    auto iter = umap.find(qwq);
                    if(iter!=umap.end()){
                        value=iter->second;
                        break;
                    }
                    else{
                        cerr<<"输入了一个不存在的变量，如果需要输入常量abcd，请用\"abcd\"包裹常量abcd :("<<endl;
                    }
                }
            }
            if(times%2){
                cerr<<"请确保\"abcd\"包裹常量abcd :("<<endl;
            }
            name=buf.substr(0,i);
            umap[name]=value;
            return ;
        }
    }
    //执行表达式处理操作 操作分为分割和计算
    ExpTraslate(buf);
}
bool isOperater(string str){
    if(str=="|"||str=="&"||str=="~"){
        return true;
    }
    return false;
}
int Precedence(char op) {
    if (op == '~') return 3;
    if (op == '&') return 2;
    if (op == '|') return 1;
    else return 0;
}
void ExpTraslate(string &buf){//可能出现错误，可以尝试异常处理
    stack<shared_ptr<Query>> qstack;
    stack<char> opstack;
    char op;
    vector<string> backexp;
    int word_start=0;
    bool last_right=buf[buf.size()-1]==')';//看在最后一个是不是右边括号
    bool Had_op=0;
    int priority1;
    for(int i=0;i<buf.size();i++){//分割字符串并转成后缀表达式
        if(buf[i]=='~'||buf[i]=='|'||buf[i]=='&'||buf[i]=='('||buf[i]==')'){
            Had_op=1;
            op=buf[i];
            priority1=Precedence(op);
            if (opstack.empty()||op=='('){//或者左括号直接压栈
                opstack.push(op);
                if(i-word_start>0){
                    backexp.push_back(buf.substr(word_start,i-word_start));
                }
                word_start=i+1;
            }
            else if(op ==')'){//如果当前位置是右括号
                if(i-word_start>0){//排除()遇到)的情况
                    backexp.push_back(buf.substr(word_start,i-word_start));
                }
                while(!opstack.empty()&&opstack.top()!='('){// 不断将栈中的元素弹出，直到遇到左括号
                    string tempop(1,opstack.top());
                    opstack.pop();
                    backexp.push_back(tempop);
                }
                if(!opstack.empty()&&opstack.top()=='('){//遇到左括号就将他弹出
                    opstack.pop();
                }
                else{
                    cerr<<"表达式括号不匹配，请检查并重启程序 :("<<endl;
                    exit(-1);
                }
                word_start=i+1;
            }
            else if(op=='~'||op=='|'||op=='&')
            {
                if(i-word_start>0){//排除(~a)遇到~的情况
                    backexp.push_back(buf.substr(word_start,i-word_start));
                }
                while((!opstack.empty())&&(Precedence(opstack.top())>=priority1)){
                    // 如果栈不空，栈顶为运算符，并且栈顶运算符的优先级大于等于当前运算符的优先级
                    string tempop(1,opstack.top());
                    opstack.pop();
                    backexp.push_back(tempop);
                }
                //再将操作符压入栈
                opstack.push(op);
                word_start=i+1;
            }
        }
        
    }//清空栈，放到后缀表达式
    if(!last_right){
        backexp.push_back(buf.substr(word_start,buf.size()-word_start));
    }
    while(!opstack.empty()){//如果op栈不空，弹出所有元素到后缀表达式
        string tempop(1,opstack.top());
        opstack.pop();
        backexp.push_back(tempop);
    }
    if(!Had_op){//没有运算符，直接走捷径
        Query onetime(IsNameReturn(buf));
        if(filesearch){
            OFileStream ofi(filename);
            cout<<onetime.rep()<<endl;
            
            onetime.eval(TextQuery(ifs->getifsteram())).write(ofi.getofsteram());
            cout<<"写入成功"<<endl;
        }else{
            cout<<onetime.rep()<<endl;
            onetime.eval(TextQuery(ifs->getifsteram())).print();
            return;
        }
        
    }
    //如果有运算符，都在栈里面
    while(!backexp.empty()){
        if(isOperater(backexp.front()))//判断向量backexp里面的是不是运算符
        {
            string opstr=backexp.front();
            if(opstr=="|"){
                if(qstack.size()<2){
                    cerr<<"表达式所需要的操作数不够... :("<<endl;
                    return ;
                }
                shared_ptr<Query> temp1=qstack.top();
                //temp1=qstack.top();
                qstack.pop();
                shared_ptr<Query> temp2=qstack.top(); 
                qstack.pop();
                shared_ptr<Query> result=make_shared<Query>(*temp1.get()|*temp2.get());
                qstack.push(result);
            }else if(opstr=="&"){
                if(qstack.size()<2){
                    cerr<<"表达式所需要的操作数不够... :("<<endl;
                    return ;  
                }
                shared_ptr<Query> temp1=qstack.top();
                //temp1=qstack.top();
                qstack.pop();
                shared_ptr<Query> temp2=qstack.top();
                qstack.pop();
                shared_ptr<Query> result=make_shared<Query>(*temp1.get()&*temp2.get());
                qstack.push(result);
            }else if(opstr=="~"){
                if(qstack.size()<1){
                    cerr<<"表达式所需要的操作数不够... :("<<endl ; 
                    return ;
                }
                shared_ptr<Query> temp=qstack.top();
                qstack.pop();
                shared_ptr<Query> result=make_shared<Query>(~(*temp.get()));
                qstack.push(result);
            }
            backexp.erase(backexp.begin());
        }//如果不是则执行下面，将Query对象入栈
        else{
            qstack.push(make_shared<Query>(IsNameReturn(backexp.front())));//栈里面的都是共享指针
            backexp.erase(backexp.begin());
        }
    }
    //最后所有运算操作完成
    if(qstack.size()==1){
        if(filesearch){
            OFileStream ofi(filename);
            qstack.top()->eval(TextQuery(ifs->getifsteram())).write(ofi.getofsteram());
            cout<<"写入成功"<<endl;
        }else{
            qstack.top()->eval(TextQuery(ifs->getifsteram())).print();
        }
    }else{
        cerr<<"表达式可能出现错误，无法运算出唯一结果"<<endl;
        exit(-1);
    }
}
void menu(){
    cout<<"选择需要的功能："<<endl;
    cout<<"1.文本格式化"<<endl;
    cout<<"2.文本查询"<<endl;
    cout<<"3.退出"<<endl;
    cout<<"Your Choice: ";
}

int main()
{//对输入的buf进行判别
    int cho;
    string buf;

    string newbuf;
    while(1){
        menu();
        cin>>cho;
        cin.ignore();
        switch (cho)
        {
            case 1://让用户输入路径
                cout<<"请输入读取的文件路径"<<endl;
                cout<<">>> ";
                getline(cin,buf);
                cout<<"请指定每行最大的字符数n :)"<<endl;
                cout<<">>>";
                int num;
                cin>>num;
                cin.ignore();
                cout<<"请指定新文件保存的路径 :)"<<endl;
                cout<<">>>";
                getline(cin,newbuf);
                try{
                    ff=make_shared<FileFormatter>(buf,newbuf,num);
                    ff->readFromFile();
                    ff->formatText();
                    ff->saveToFile();
                }catch(string str){
                    cerr<<"错误的文件路径"<<buf<<"导致打开异常 :("<<endl;
                }catch(char ch){
                    cerr<<"错误的文件路径"<<buf<<"导致写入异常 :("<<endl;
                }
                break;
            case 2:
                cout<<"请输入读取的文件路径"<<endl;
                cout<<">>> ";
                getline(cin,newbuf);//buf就是路径
                try{
                    ifs=make_shared<IFileStream>(newbuf);
                    cout<<">>> ";
                    getline(cin,buf);//从输入流中读取一行数据到buf
                    BlankFliter(buf);//首先排除空格的影响
                    StringTraslate(buf);
                }catch(string str){
                    cerr<<str;
                }
                break;
            case 3:
                exit(0);
                break;
            default:
                cout<<"无效的选项:("<<endl;
                break;
        }
    }
    return 0;
}
