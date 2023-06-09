#include "../include/MySqlPool.h"
MySqlConn *MySqlPool::Get_Conn(const int timeout_ms)
{
    std::unique_lock<std::mutex> lock(this->mtx);
    if (Is_terminate)
    {
        log_err("Pool is terminate");
        return NULL;
    }
    if (conn_list.empty())
    {
        if (db_cur_conn_cnt >= db_max_conn_cnt) // 达到最大连接数
        {
            if (timeout_ms <= 0) // 死等
            {
                log_info("wait ms:" + timeout_ms);
                cond.wait(lock, [this]()
                          {
                        //log_info("wait:"<<(wait_cnt++));
                        return (!conn_list.empty()) | Is_terminate; });
            }
            else
            { // 超时等待
                cond.wait_for(lock, std::chrono::milliseconds(timeout_ms), [this]()
                              {
                        //log_info("wait_for:"<<(wait_cnt++));
                        return (!conn_list.empty()) | Is_terminate; });

                if (conn_list.empty()) // 超时队列为空则退出
                    return NULL;
            }
            if (Is_terminate)
            {
                log_info("Pool is terminate");
            }
        }
        else // 还没达到最大连接数
        {
            MySqlConn *new_conn = new MySqlConn(this);
            int ret = new_conn->Connect(this->_remote, this->User_Name, this->Password, this->DB_Name, this->_port);
            if (ret != OK)
            {
                delete new_conn;
                log_err("new conn failed");
                return NULL;
            }
            conn_list.push_back(new_conn);
            db_cur_conn_cnt++;
        }
    }
    MySqlConn *conn_ptr = conn_list.front();
    conn_list.pop_front();
    lock.unlock();
    return conn_ptr;
}

int32_t MySqlPool::Ret_Conn(MySqlConn *mysql_conn)
{
    std::list<MySqlConn *>::iterator it = conn_list.begin();
    for (; it != conn_list.end(); it++)
    {
        if (*it == mysql_conn)
        {
            log_err("connection return reapted");
            return ERROR;
        }
    }
    std::unique_lock<std::mutex> lock(this->mtx);
    conn_list.push_back(mysql_conn);
    cond.notify_one();
    return OK;
}

void MySqlPool::Destory()
{
    int n=conn_list.size();
    for(int i=0;i<n;i++){
        MySqlConn* conn=conn_list.front();
        conn_list.pop_front();
        delete conn;
    }
}

uint32_t MySqlPool::Init_Conn()
{
    for (int i = 0; i < DEFAULT_MIN_CONN; i++)
    {
        MySqlConn *conn_ptr = new MySqlConn(this);
        int ret = conn_ptr->Connect(this->_remote, this->User_Name, this->Password, this->DB_Name, this->_port);
        if (ret != OK)
        {
            delete conn_ptr;
            return ret;
        }
        conn_list.push_back(conn_ptr);
    }
    return OK;
}