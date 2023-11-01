#include "../include/MySql.h"
MySql::MySql()
{
    this->handle=mysql_init(NULL);
    if(this->handle==NULL) exit(0);
    mysql_options(this->handle,MYSQL_SET_CHARSET_NAME,"utf8mb4");
}

string MySql::Create_Query(string tb_name,vector<string>&& names,vector<string>&& types)
{
    if(tb_name.empty()||(names.size()!=types.size()))
    {
        spdlog::info("File:{} Line:{} {} create error",__FILE__,__LINE__);
        return "";
    }    
    string query="CREATE TABLE IF NOT EXISTS " + tb_name+"(";
    vector<string> contexts;
    for(int i=0;i<names.size();i++){
        string context=names[i]+" "+types[i];
        contexts.push_back(context);
    }
    string res = Comma_Compose(contexts);
    query = query + res + ");";
    return query;
}

string MySql::Drop_Query(string tb_name)
{
    if(tb_name.empty()){
        spdlog::info("File:{} Line:{} drop empty",__FILE__,__LINE__);
        return "";
    }
    return "DROP TABLE IF EXISTS "+tb_name+";";
}

string MySql::Select_Query(vector<string>&& field,string table,string condition)
{
    if(field.empty()||table.empty()||condition.empty())
    {
        spdlog::info("File:{} Line:{} select empty",__FILE__,__LINE__);
        return "";
    }
    string query = "SELECT " ;
    for(int i=0;i<field.size();i++)
    {
        query+=field[i];
        if(i!=field.size()-1){
            query+=",";
        }
    }
    query = query + " FROM " + table;
    if (condition != "NULL")
    {
        query += " WHERE " + condition;
    }
    query += ";";
    return query;
}

string MySql::Insert_Query(string table,vector<string> &&columns , vector<string> &&values)
{
    if (columns.empty() || values.empty())
    {
        spdlog::info("File:{} Line:{} {} insert empty",__FILE__,__LINE__);
        return "";
    }
    string col = Comma_Compose(columns);
    string val = Comma_Compose(values);
    string query = "INSERT INTO " + table + "(" + col + ") values (" + val + ");";
    return query;
}

string MySql::Update_Query(string table,vector<string> &&columns,vector<string> &&values,string condition)
{
    if (columns.size() != values.size())
    {
        spdlog::info("File:{} Line:{} update error",__FILE__,__LINE__);
        return "";
    }
    string query = "UPDATE " + table + " SET ";
    int n = columns.size();
    for (int i = 0; i < n; i++)
    {
        string part = columns[i] + "=" + values[i];
        if (i != n - 1)
            part += ",";
        query += part;
    }
    query = query + " WHERE " + condition + ";";
    return query;
}

string MySql::Delete_Query(string table,string condition)
{
    if(table.empty()||condition.empty())
    {
        spdlog::info("File:{} Line:{} delete empty",__FILE__,__LINE__);
        return "";
    }
    return "DELETE FROM " + table + " WHERE " + condition + ";";
}

string MySql::Alter_Query(string table,Alter_Type type,string condition)
{
    if(table.empty()||condition.empty())
    {
        spdlog::info("File:{} Line:{} alter empty",__FILE__,__LINE__);
        return "";
    }
    string query="ALTER TABLE "+table+" ";
    switch(type)
    {
        case ADD:
            query+="ADD "+condition+";";
            break;
        case DROP:
            query+="DROP "+condition+";";
            break;
        case CHANGE:
            query+="CHANGE "+condition+";";
            break;
    };
    return query;
}

int MySql::Connect(string remote, string usrname, string passwd, string db_name,int32_t port)
{
    if (!mysql_real_connect(this->handle, remote.c_str(), usrname.c_str(), passwd.c_str(), db_name.c_str(), port, NULL, 0))
    {
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return mysql_errno(this->handle);
    }
    return 0;
}

int MySql::Create_Table(string query)
{
    if(query.empty()) return -2;
    if (mysql_ping(this->handle))
    {
        return -1;
    }
    Cmd_Add(query);
    int ret = mysql_real_query(this->handle,query.c_str(),query.size());
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return mysql_errno(this->handle);
    }
    return 0;
}

int MySql::Drop_Table(string query)
{
    if(query.empty()) return -2;
    if (mysql_ping(this->handle))
    {
        return -1;
    }
    Cmd_Add(query);
    int ret=mysql_real_query(this->handle,query.c_str(),query.size());
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return mysql_errno(this->handle);
    }
    return ret;
}

int MySql::Select(string query,vector<vector<string>>& results)
{
    if(query.empty())
        return -2;
    if (mysql_ping(this->handle))
    {
        return -1;
    }
    // res.assign(0,"");
    Cmd_Add(query); // 加入历史命令
    int ret = mysql_real_query(this->handle, query.c_str(),query.size());
    if (ret)
    {
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return mysql_errno(this->handle);
    }
    MYSQL_RES *m_res = mysql_store_result(this->handle);
    MYSQL_FIELD *fd; // 列名
    vector<string> names;
    while ((fd = mysql_fetch_field(m_res)) != NULL)
    {
        names.push_back(string(fd->name));
    }
    results.push_back(names);
    int field_num = mysql_num_fields(m_res); // 列数
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(m_res)) != NULL)
    {
        vector<string> result;
        for (int i = 0; i < field_num; i++)
        {
            if (row[i] == NULL)
            {
                result.push_back("NULL");
            }
            else
            {
                result.push_back(row[i]);
            }
        }
        results.push_back(result);
    }
    mysql_free_result(m_res);
    return 0;
}

int MySql::Insert(string query)
{
    if(query.empty()) return -2;
    if (mysql_ping(this->handle))
    {
        return -1;
    }
    Cmd_Add(query);
    int ret = mysql_real_query(this->handle, query.c_str(),query.size());
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return mysql_errno(this->handle);
    }
    return 0;
}

int MySql::Update(string query)
{
    if(query.empty()) return -2;
    if (mysql_ping(this->handle))
    {
        return -1;
    }
    Cmd_Add(query);
    int ret = mysql_real_query(this->handle, query.c_str(),query.size());
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return mysql_errno(this->handle);
    }
    return 0;
}

int MySql::Delete(string query)
{
    if(query.empty()) return -2;
    if (mysql_ping(this->handle))
    {
        return -1;
    }
    Cmd_Add(query);
    int ret = mysql_real_query(this->handle, query.c_str(),query.size());
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return mysql_errno(this->handle);
    }
    return 0;
}

int MySql::Alter(string query)
{
    if(query.empty()) return -2;
    if (mysql_ping(this->handle))
    {
        return -1;
    }
    Cmd_Add(query);
    int ret=mysql_real_query(this->handle,query.c_str(),query.size());
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return mysql_errno(this->handle);
    }
    return 0;
}

// 发送二进制数据
int MySql::Param_Send_Binary(string param_query,const char* buffer,int32_t len)
{
    if(buffer==NULL||param_query.empty()||len<=0){
        spdlog::info("File:{} Line:{} param empty",__FILE__,__LINE__);
        return -1;
    }
    if (mysql_ping(this->handle))
    {
        return -1;
    }
    MYSQL_STMT *stmt=mysql_stmt_init(this->handle);
    Cmd_Add(param_query);
    int ret=mysql_stmt_prepare(stmt,param_query.c_str(),param_query.length());
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return -1;
    }
    MYSQL_BIND param={0};
    param.buffer_type=MYSQL_TYPE_LONG_BLOB;
    param.buffer=NULL;
    param.is_null=0;
    param.length=NULL;
    ret = mysql_stmt_bind_param(stmt,&param);
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return -1;
    }
    ret=mysql_stmt_send_long_data(stmt,0,buffer,len);
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return -1;
    }
    ret=mysql_stmt_execute(stmt);
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return -1;
    }
    ret=mysql_stmt_close(stmt);
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return -1;
    }
    return 0;

}
// 接收二进制数据
int MySql::Param_Recv_Binary(string param_query,char *&buffer,int32_t len)
{
    if(param_query.empty()||buffer==NULL){
        spdlog::info("File:{} Line:{} empty",__FILE__,__LINE__);
        return -1;
    }
    if (mysql_ping(this->handle))
    {
        return -1;
    }
    memset(buffer,0,len);
    MYSQL_STMT *stmt=mysql_stmt_init(this->handle);
    Cmd_Add(param_query);
    int ret = mysql_stmt_prepare(stmt,param_query.c_str(),param_query.length());
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return -1;
    }
    MYSQL_BIND result={0};
    result.buffer_type = MYSQL_TYPE_LONG_BLOB;
    uint64_t total_length =0 ;
    result.length=&total_length;

    ret=mysql_stmt_bind_result(stmt,&result);
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return -1;
    }

    ret=mysql_stmt_execute(stmt);
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return -1;
    }

    ret=mysql_stmt_store_result(stmt);
    if(ret){
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return -1;
    }

    while(1){

        ret=mysql_stmt_fetch(stmt);
        if (ret != 0 && ret != MYSQL_DATA_TRUNCATED) break;//异常
        int start=0;
        while (start < (int)total_length) {
			result.buffer = buffer + start;
			result.buffer_length = 1;
			mysql_stmt_fetch_column(stmt, &result, 0, start);
			start += result.buffer_length;
		}
    }
    mysql_stmt_close(stmt);
    return total_length;
}

void MySql::History(int num)
{
    vector<string> cmd = Cmd_History();
    if (num == 0 || num > Cmd_Num())
    {
        for (int i = 0; i < cmd.size(); i++)
        {
            std::cout << cmd[i] << std::endl;
        }
    }
    else
    {
        for (int i = 0; i < num; i++)
        {
            std::cout << cmd[i] << std::endl;
        }
    }
}

string MySql::Comma_Compose(vector<string> &args)
{
    string res = "";
    int n = args.size();
    for (int i = 0; i < n; i++)
    {
        res += args[i];
        if (i != n - 1)
        {
            res += ",";
        }
    }
    return res;
}


bool MySql::StartTransaction()
{
    if (mysql_ping(this->handle))
    {
        return -1;
    }

    if(mysql_real_query(this->handle,"start transaction;",18))
    {
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return false;
    }
    return true;
}

bool MySql::Commit()
{
    if (mysql_ping(this->handle))
    {
        return -1;
    }
    if(mysql_real_query(this->handle,"commit;",7))
    {
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return false;
    }
    return true;
}

bool MySql::Rollback()
{
    if (mysql_ping(this->handle))
    {
        return -1;
    }
    if(mysql_real_query(this->handle,"rollback;",9))
    {
        spdlog::info("File:{} Line:{} {}",__FILE__,__LINE__,mysql_error(this->handle));
        return false;
    }
    return true;
}