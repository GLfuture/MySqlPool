#pragma once
#ifndef MYSQLPOOL_H
#define MYSQLPOOL_H
#endif
#include"MySql.h"
#include<condition_variable>
#include<list>
#define DEFAULT_MAX_CONN 8
#define DEFAULT_MIN_CONN 4
#define log_info(x) std::cout<<x<<std::endl
class MySqlPool;

class MySqlConn:public MySql{
public:
    /// @brief 
    MySqlConn(MySqlPool* ptr) : pool(ptr){}
    MySqlConn(MySqlConn* mysql_conn)=delete;
private:
    MySqlPool* pool;
};


class MySqlPool//继承该类以返回this指针的share_ptr
{
public:
    MySqlPool(){};
    MySqlPool(string pool_name, string host, string db_name, string user_name, string password, uint32_t port,uint32_t min_conn=DEFAULT_MIN_CONN,uint32_t max_conn=DEFAULT_MAX_CONN)
        : Pool_Name(pool_name), _remote(host), DB_Name(db_name), User_Name(user_name), Password(password),db_cur_conn_cnt(min_conn),db_max_conn_cnt(max_conn),
          _port(port), Is_terminate(false), wait_cnt(0)
    {
        Init_Conn();
    }

    //获取连接
    MySqlConn* Get_Conn(const int timeout_ms);
    //归还连接
    int32_t Ret_Conn(MySqlConn* mysql_conn);
    //销毁连接池
    void Destory();
    //获取连接池的姓名
    const char * Get_Pool_Name(){return Pool_Name.c_str();}
    const char* Get_DBServer_IP() { return _remote.c_str(); }
	uint32_t 	Get_DBServer_Port() { return _port; }
	const char* Get_Username() { return User_Name.c_str(); }
	const char* Get_Passwrod() { return Password.c_str(); }
	const char* Get_DBName() { return DB_Name.c_str(); }
private:
    //初始化连接
    uint32_t Init_Conn();

private:

    string Pool_Name;//连接池名
    string _remote;//远程主机
    uint32_t _port;//端口
    string DB_Name;//database
    string User_Name;//用户名
    string Password;//密码


    uint32_t db_cur_conn_cnt;//当前连接数
    uint32_t db_max_conn_cnt;//最大连接数
    
    std::list<MySqlConn*> conn_list;

    std::mutex mtx;
    std::condition_variable cond;

    bool Is_terminate;
    uint32_t wait_cnt;//正在等待的连接数
};