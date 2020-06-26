#include "mysqlpool.h"
#include "include/mysql/mysql.h"
#include <string>

MysqlPool* MysqlPool::mysqlpool_object = NULL;
std::mutex MysqlPool::objectlock;
std::mutex MysqlPool::poollock;

MysqlPool::MysqlPool() {}

/*
 *配置数据库参数
 */
void MysqlPool::setParameter( const char*   mysqlhost,
                              const char*   mysqluser,
                              const char*   mysqlpwd,
                              const char*   databasename,
                              unsigned int  port,
                              const char*   socket,
                              unsigned long client_flag,
                              unsigned int  max_connect ) 
{
  _mysqlhost    = mysqlhost;
  _mysqluser    = mysqluser;
  _mysqlpwd     = mysqlpwd;
  _databasename = databasename;
  _port         = port;
  _socket       = socket;
  _client_flag  = client_flag;
  MAX_CONNECT   = max_connect;
  connect_count = 0;
}
  
/*
 *有参的单例函数，用于第一次获取连接池对象，初始化数据库信息。
 */
MysqlPool* MysqlPool::getMysqlPoolObject() 
{
  if (mysqlpool_object == NULL) 
  { 
    objectlock.lock();
    if (mysqlpool_object == NULL) 
    {
      mysqlpool_object = new MysqlPool();
    }
    objectlock.unlock();
  }
  return mysqlpool_object;
}
                                                 
/*
 *创建一个连接对象
 */
MYSQL* MysqlPool::createOneConnect() 
{
  MYSQL* conn = NULL;
  conn = mysql_init(conn);
  if (conn != NULL) 
  {
    if (mysql_real_connect(conn,
                          _mysqlhost,
                          _mysqluser,
                          _mysqlpwd,
                          _databasename,
                          _port,
                          _socket,
                          _client_flag)) 
    {
      connect_count++;
      return conn;   
    } else 
    {
      std::cout << mysql_error(conn) << std::endl;
      return NULL;
    }
  } else 
  {
    std::cerr << "init failed" << std::endl;
    return NULL;
  }
}

/*
 *判断当前MySQL连接池的是否空
 */
bool MysqlPool::isEmpty() 
{
  return mysqlpool.empty();
}
/*
 *获取当前连接池队列的队头
 */
MYSQL* MysqlPool::poolFront() 
{
  return mysqlpool.front();
}
/*
 *
 */
unsigned int MysqlPool::poolSize() 
{
  return mysqlpool.size();
}
/*
 *弹出当前连接池队列的队头
 */
void MysqlPool::poolPop() 
{
  mysqlpool.pop();
}


MYSQL* MysqlPool::getOneConnect() 
{
  poollock.lock();
  MYSQL *conn = NULL;
  if (!isEmpty()) 
  {
    while (!isEmpty() && mysql_ping(poolFront())) 
    {
      mysql_close(poolFront());
      poolPop();
      connect_count--;
    }
    if (!isEmpty()) 
    {
      conn = poolFront();
      poolPop();
    } else 
    {
      if (connect_count < MAX_CONNECT)
        conn = createOneConnect(); 
      else 
        std::cerr << "the number of mysql connections is too much!" << std::endl;
    }
  } else 
  {
    if (connect_count < MAX_CONNECT)
      conn = createOneConnect(); 
    else 
      std::cerr << "the number of mysql connections is too much!" << std::endl;
  }
  poollock.unlock();
  return conn;
}
/*
 *将有效的链接对象放回链接池队列中，以待下次的取用。
 */
void MysqlPool::close(MYSQL* conn) 
{
  if (conn != NULL) 
  {
    poollock.lock();
    mysqlpool.push(conn);
    poollock.unlock();
  }
}

std::map<const std::string,std::vector<const char*> >  MysqlPool::executeSql(const char* sql) 
{
  MYSQL* conn = getOneConnect();
  std::map<const std::string,std::vector<const char*> > results;
  if (conn) 
  {
    if (mysql_query(conn,sql) == 0) 
    {
      MYSQL_RES *res = mysql_store_result(conn);
      if (res) 
      {
        MYSQL_FIELD *field;
        while ((field = mysql_fetch_field(res))) 
        {
          results.insert(make_pair(field->name,std::vector<const char*>()));
        }
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res))) 
        {
          unsigned int i = 0;
          for (std::map<const std::string,std::vector<const char*> >::iterator it = results.begin();
              it != results.end(); ++it) 
          {
            (it->second).push_back(row[i++]);
          }     
        }
        mysql_free_result(res);
      } else 
      {
        if (mysql_field_count(conn) != 0)
          std::cerr << mysql_error(conn) << std::endl;
      }
    } else 
    {
      std::cerr << mysql_error(conn) <<std::endl;
    }
    close(conn);
  } else 
  {
    std::cerr << mysql_error(conn) << std::endl;
  }
  return results;
}
/*
 * 析构函数，将连接池队列中的连接全部关闭
 */
MysqlPool::~MysqlPool() 
{
  while (poolSize() != 0) 
  {
    mysql_close(poolFront());
    poolPop();
    connect_count--;
  }
}

bool MysqlPool::GetUserInfoByUserId(const char* userId, std::vector<stUserLogin> &result)
{
  char selSql[200] = {'\0'};
  sprintf(selSql, "SELECT user_id, password FROM user_info WHERE user_id ='%s'", userId);

  MYSQL* conn = getOneConnect();
  if(mysql_query(conn, selSql))
  {
      std::cout << "selSql user_info was Error:" << mysql_error(conn) << std::endl;
      return false;
  } else 
  {
    MYSQL_RES *res = mysql_store_result(conn); // 获取结果集
    MYSQL_ROW row;
    while((row = mysql_fetch_row(res)) != NULL)
    {
        if (mysql_num_fields(res) > 0) 
        {
          stUserLogin userLogin;
          userLogin.userId = row[0];
          userLogin.passwordChar = row[1];
          result.push_back(userLogin);
        }
    }
    if (res != NULL)
    {
      mysql_free_result(res);
    }
  }
  return true;
}

bool MysqlPool::AddUserInfo(const stUserLogin* userlogin)
{
  MYSQL* conn = getOneConnect();

  char intSql[200] = {'\0'};
  sprintf(intSql, "insert into user_info (user_id,password_char) values (%s, %s)", userlogin->userId, userlogin->passwordChar);
 
  int ret = mysql_query(conn, intSql);
  if (ret != 0)
	{
		std::cout << "insert user_info was Error:" << mysql_error(conn) << std::endl;
    return false;
  }
  return true;
}