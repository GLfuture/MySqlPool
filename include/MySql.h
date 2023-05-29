#pragma once
#ifndef MYSQL_H
#define MYSQL_H
#include<iostream>
#include<mysql/mysql.h>
#include<string>
#include<vector>
#include<span>
#include<typeinfo>
#include<cxxabi.h>
#include<exception>
#include<string.h>

#define log_err(x) std::cout<<x<<std::endl
#define OK 0
#define EMPTY 1
#define ERROR 2
using std::string;
using std::vector;
using std::span;
using namespace abi;

#endif
class Type_Exception:public std::exception{
public:
    const char* what() {
        return "type error";
    }
};

class MySql//:public std::exception
{
public:
    enum Alter_Type{
        ADD,
        DROP,
        CHANGE,
    };
    MySql();
    MySql(const MySql&) = delete;
    MySql(MySql&&)=delete;
    MySql& operator=(const MySql&)=delete;
    MySql& operator=(const MySql&&)=delete;

    string Create_Query(string tb_name,vector<string>&& words,vector<string>&& types);

    string Drop_Query(string tb_name);

    string Select_Query(vector<string>&& field,string table,string condition="NULL");

    string Insert_Query(string tb_name,vector<string> &&fields , vector<string> &&values);

    string Update_Query(string tb_name,vector<string> &&fields,vector<string> &&values,string condition);

    string Delete_Query(string tb_name,string condition);
    
    string Alter_Query(string tb_name,Alter_Type type,string condition);

    
    uint32_t Connect(string remote,string usrname,string passwd,string db_name,int32_t port);

    uint32_t Create_Table(string query);

    uint32_t Drop_Table(string query);

    //inline uint32_t Create_DB(string db_name);

    vector<vector<string>> Select(string query);

    uint32_t Insert(string query);

    uint32_t Update(string query);

    uint32_t Delete(string query);

    uint32_t Alter(string query);
    //发送二进制数据
    uint32_t Param_Send_Binary(string param_query,const char* buffer,int32_t len);
    //接收二进制数据
    uint32_t Param_Recv_Binary(string param_query,char *&buffer,int32_t len);
    // 开启事务
	bool StartTransaction();
	// 提交事务
	bool Commit();
	// 回滚事务
	bool Rollback();
    
    


    template<typename ...Str>
    static inline vector<string> Arg_List(Str...colmuns){
        vector<string> ans;
        try{
            vector<string> res;
            arg_all(res,colmuns...);
            ans=res;
        }
        catch(Type_Exception* &e){
            log_err(e->what());
            delete e;
        }
        return ans;
    }

    void  History(int num=0);

    ~MySql(){
        mysql_close(this->handle);//关闭数据库连接
    }

private:
    
    string Comma_Compose(vector<string>& args);
    
    static inline void arg_sub(vector<string>& sub){
    }

    template<typename Arg,typename ... Args>
    static inline void arg_sub(vector<string>& sub,Arg value,Args...values){
        if(!IsString(value)) throw new Type_Exception;
        sub.push_back(value);
        arg_sub(sub,values...);
    }

    template<typename ... Args>
    static inline void arg_all(vector<string>& sub,Args...args){
        arg_sub(sub,args...);
    }

    template<typename T>
    static inline bool IsString(T& value){
        //string stringtype(__cxa_demangle(typeid(string).name(),NULL,NULL,NULL));
        string type(__cxa_demangle(typeid(value).name(),NULL,NULL,NULL));
        if(type.find("char")!=string::npos||type.find("string")!=string::npos) return true;
        return false;
    }
    inline span<string> Cmd_History(){
        return cmd_history;
    }

    inline uint32_t  Cmd_Num(){
        return cmd_history.size();
    }

    inline void Cmd_Add(string cmd){
        cmd_history.push_back(cmd);
    }

    MYSQL* handle;//mysql连接句柄结构体
    vector<string> cmd_history;//历史mysql查询语句
};
