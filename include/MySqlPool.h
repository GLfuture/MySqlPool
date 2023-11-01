#pragma once
#ifndef MYSQLPOOL_H
#define MYSQLPOOL_H
#endif
#include"MySql.h"
#include<condition_variable>
#include<list>
class MySqlPool;

class MySqlConn:public MySql{
public:
    /// @brief 
    MySqlConn(MySqlPool* ptr) : pool(ptr){}
    MySqlConn(MySqlConn* mysql_conn)=delete;
private:
    MySqlPool* pool;
};


class MySqlPool
{
public:
    MySqlPool(){}

    MySqlPool(const MySqlPool& pool)
    {
        this->conn_list=pool.conn_list;
        this->db_cur_conn_cnt=pool.db_cur_conn_cnt;
        this->db_max_conn_cnt=pool.db_max_conn_cnt;
        this->DB_Name=pool.DB_Name;
        this->Is_terminate=pool.Is_terminate;
        this->Password=pool.Password;
        this->Pool_Name=pool.Pool_Name;
        this->User_Name=pool.User_Name;
        this->wait_cnt=pool.wait_cnt;
        this->_port=pool._port;
        this->_remote=pool._remote;
    }


    MySqlPool(string pool_name, string host, string db_name, string user_name, string password, uint32_t port,uint32_t min_conn,uint32_t max_conn)
        : Pool_Name(pool_name), _remote(host), DB_Name(db_name), User_Name(user_name), Password(password),db_cur_conn_cnt(min_conn),db_max_conn_cnt(max_conn),
          _port(port), Is_terminate(false), wait_cnt(0)
    {
        uint16_t ret = Init_Conn();
        if(ret!=0) throw ret;
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