#include"MySql.h"
#include<fstream>
char* read_image(string filename)
{
    std::ifstream in(filename.c_str(),std::ios::binary);
    if(!in.is_open()){
        std::cout<<"open failed"<<std::endl;
        return 0;
    }
    in.seekg(0,in.end);
    int total_len=in.tellg();
    in.seekg(0,in.beg);
    char *buffer=new char[total_len];
    memset(buffer,0,sizeof(buffer));
    in.read(buffer,total_len);
    //std::cout<<in.gcount()<<"\n";
    in.close();
    return buffer;
}

void write_image(string filename,char*buffer,int len)
{
    std::ofstream out(filename.c_str(),std::ios::binary);
    out.write(buffer,len);
    out.close();
}

int main()
{
    MySql mysql;
    int ret = mysql.Connect("8.130.92.28","gong","123456","web",3306);
    //mysql.Create_Table(mysql.Create_Query("manager",MySql::Arg_List("name","age"),MySql::Arg_List("varchar(10) primary key","tinyint")));
    //mysql.Insert(mysql.Insert_Query("manager",MySql::Arg_List("name","age"),MySql::Arg_List("'zhang'","10")));
    //mysql.Update(mysql.Update_Query("manager",MySql::Arg_List("age"),MySql::Arg_List("30"),"name='zhang'"));
    //mysql.Insert(mysql.Insert_Query("manager",MySql::Arg_List("name","age"),MySql::Arg_List("'li'","10")));
    //mysql.Delete(mysql.Delete_Query("manager","name='li'"));
    vector<vector<string>> res;
    ret = mysql.Select(mysql.Select_Query(MySql::Arg_List("username","password"),"user"),res);
    //mysql.Alter(mysql.Alter_Query("manager",MySql::ADD,"image LONGBLOB"));
    //mysql.Alter(mysql.Alter_Query("manager",MySql::CHANGE,"address address varchar(10)"));
   // mysql.Alter("manager",MySql::DROP,"address");
    //mysql.Alter("manager",MySql::ADD,"name varchar(20)");
    //mysql.Drop_Table(mysql.Drop_Query("manager"));
    //mysql.Create_Table("",MySql::Arg_List(""),MySql::Arg_List(""));
    //mysql.Alter("",MySql::ADD,"");
    //mysql.Delete("","");
    //插入图片
    // char *buffer = read_image("a.jpg");
    //std::cout<<"file size:"<<len<<"\n";
    // int len=sizeof(buffer);
    // mysql.Param_Send_Binary(mysql.Insert_Query("manager",MySql::Arg_List("name","image"),MySql::Arg_List("'gong'","?")),buffer,len);
    // mysql.Param_Recv_Binary(mysql.Select_Query(MySql::Arg_List("image"),"manager","name='gong'"),buffer,len);
    // write_image("b.jpg",buffer,len);
    mysql.History();
    for(auto &s:res){
        for(auto &v:s){
            std::cout<<v<<" ";
        }
        std::cout<<std::endl;
    }
    return 0;
}