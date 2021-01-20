#ifndef _STDAFX_HPP_
#define _STDAFX_HPP_

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <mysql.h>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace utility;
using namespace web::http::experimental::listener;

typedef struct connection_details {  
    const char *server;
    const char *user;
    const char *password;
    const char *database;
    std::string table_name;
}SQL_info;

class Handler{
public:
    Handler() {};
    Handler(utility::string_t url, http_listener_config config, SQL_info myDB, std::vector<utility::string_t> mytable_list);
    ~Handler() { mysql_close(Connect_maria); };

    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }

private:
    void handle_get(http_request request);
    void handle_put(http_request request);
    void handle_post(http_request request);
    void handle_del(http_request request);
    MYSQL* mysql_connection_setup(SQL_info myDB);
    MYSQL_RES* mysql_perform_query_select(MYSQL *connection, std::string select_cmd, http_request& req, utility::string_t error_msg);
    bool mysql_perform_query_input(MYSQL *connection, std::string select_cmd, http_request& req, utility::string_t error_msg);

private:
    http_listener m_listener;
    MYSQL *Connect_maria;
    std::vector<std::string> table_Structure;
    const std::string table_name;
};


#endif