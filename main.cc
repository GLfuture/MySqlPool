#include"include/MySqlPool.h"
#include"include/ThreadPool.h"
#include <unistd.h>
#include <sys/time.h>
static uint64_t get_tick_count()
{
    struct timeval tval;
    uint64_t ret_tick;

    gettimeofday(&tval, NULL);

    ret_tick = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
    return ret_tick;
}
int query_task(MySqlPool &sql_pool,string name)
{
    MySqlConn* conn = sql_pool.Get_Conn(0);
    int ret = conn->Insert(conn->Insert_Query("user",MySqlConn::Arg_List("username","password"),MySqlConn::Arg_List(name,"20")));
    sql_pool.Ret_Conn(conn);//用完连接需要归还
    return ret;
}

int main()
{
    
    MySqlPool sql_pool("pool1","8.130.92.28","web","gong","123456",3306,4,8);
    ThreadPool th_pool(4);
    std::queue<std::future<int>> q;
    for(int i=0;i<100;i++){
        string name="'"+std::to_string(i)+"'";
        std::future<int> res = th_pool.exec(query_task,std::ref(sql_pool),name);//注意引用需要ref
        q.push(move(res));
    }
    for(int i=0;i<100;i++){
        std::future<int> res = std::move(q.front());
        q.pop();
        std::cout<<res.get()<<" ";
        if(i%5==0) std::cout<<std::endl;
    }
    th_pool.Destroy();
    sql_pool.Destory();
    return 0;
}