#include "stdafx.hpp"

Handler::Handler(utility::string_t url, http_listener_config config, SQL_info myDB, 
    std::vector<utility::string_t> mytable_list) : m_listener(url, config), list(mytable_list), table_name(myDB.table_name){

    Connect_maria = mysql_connection_setup(myDB);
    m_listener.support(methods::GET, std::bind(&Handler::handle_get, this, std::placeholders::_1));
    m_listener.support(methods::PUT, std::bind(&Handler::handle_put, this, std::placeholders::_1));
    m_listener.support(methods::POST, std::bind(&Handler::handle_post, this, std::placeholders::_1));
    m_listener.support(methods::DEL, std::bind(&Handler::handle_del, this, std::placeholders::_1));
}

MYSQL* Handler::mysql_connection_setup(SQL_info myDB){  //DB connection

    MYSQL *connection = mysql_init(NULL);
 
    if(!mysql_real_connect(connection, myDB.server, myDB.user, myDB.password, myDB.database, 0, NULL, 0)) {
 
        printf("Connection error : %s\n", mysql_error(connection));
        exit(1);
    }
    
    return connection;
}

MYSQL_RES* Handler::mysql_perform_query_select(MYSQL *connection, std::string select_cmd, http_request& req, utility::string_t error_msg) {
 
    if(mysql_query(connection, select_cmd.c_str())) {   //DB connection error handler

        printf("MYSQL query error : %s\n", mysql_error(connection)); 
        req.reply(status_codes::BadRequest, U(error_msg)); 
        return NULL;
    }
        
    return mysql_use_result(connection);
}

bool Handler::mysql_perform_query_input(MYSQL *connection, std::string select_cmd, http_request& req, utility::string_t error_msg) {
 
    if(mysql_query(connection, select_cmd.c_str())) {   //DB connection error handler
        printf("MYSQL query error : %s\n", mysql_error(connection)); 
        req.reply(status_codes::BadRequest, U(error_msg)); 
        return false;
    }
        
    return true;
}

void Handler::handle_get(http_request request){     //Processing as json data when requesting GET.
    
    std::cout << "handle_get request" << std::endl;

    int index = 1;
    auto j = json::value::object();
    json::value j_list;
    MYSQL_RES *res;
    MYSQL_ROW row;
    
    std::string select_cmd = "select * from " + table_name;
    res = mysql_perform_query_select(Connect_maria, select_cmd, request, "Failed to GET data.");
    if(res == NULL) return;

    while((row = mysql_fetch_row(res)) != NULL){    

        utility::string_t key = std::to_string(index++);
        for(size_t i = 0; i < list.size(); i++)
            j[U(list[i])] = json::value::string(row[i]);
        
        j_list[U(key)] = j;
    }

	request.reply(status_codes::OK, j_list);                     
	mysql_free_result(res);
}

void Handler::handle_del(http_request request){     //DB Delete Request
    
std::cout << "handle_del request" << std::endl;

    std::string key = list[0];
    auto j = request.extract_json().get(); 
    
    std::string id = j[U(key)].serialize();
    id.erase(remove(id.begin(), id.end(), '"'), id.end());     //"" erase
    

    std::string delete_cmd = "DELETE FROM " + table_name + " WHERE " + key + " = " + id + ";";
    
    if(mysql_perform_query_input(Connect_maria, delete_cmd.c_str(), request, "Database DELETE failed.")){
    
        std::cout << "database DELETE complete" << std::endl;
    	request.reply(status_codes::OK, U("DELETE complete"));           
    } 
}

void Handler::handle_put(http_request request){    //DB Update Request

    std::cout << "handle_put request" << std::endl;

    std::vector<std::string> update_list;
    std::vector<std::string> key_list;
    update_list.reserve(list.size()); key_list.reserve(list.size());
    
    auto j = request.extract_json().get(); 

    for(size_t i = 0; i < list.size(); i++){      //Key extraction from Json.
        
        if(j.has_field(U(list[i]))){    //key find

            key_list.emplace_back(list[i]);
            std::string element = j[U(list[i])].serialize();
            element.erase(std::remove(element.begin(), element.end(), '"'), element.end());
            update_list.emplace_back(element);
        }
    }

    std::string update_cmd = "UPDATE " + table_name + " SET ";
    for(size_t i = 1; i < key_list.size(); i++)
        update_cmd += key_list[i] + " = " + "'" + update_list[i] + "', ";
    
    update_cmd.pop_back(); update_cmd.pop_back();
    update_cmd += " WHERE " + key_list[0] + " = " + update_list[0];

    if(mysql_perform_query_input(Connect_maria, update_cmd.c_str(), request, "Database modification failed.")){
    
        std::cout << "database update complete" << std::endl;
    	request.reply(status_codes::OK, U("update complete"));           
    }
}

void Handler::handle_post(http_request request){     //Serialize the json data received at the time of post request.
    
    std::cout << "handle_post request" << std::endl;
   
    std::vector<std::string> input_list;
    input_list.reserve(list.size());

    auto j = request.extract_json().get();

    for(size_t i = 0; i < list.size(); i++){

        std::string element = j[U(list[i]]).serialize();
        element.erase(std::remove(element.begin(), element.end(), '"'), element.end());
        input_list.emplace_back(element);
    }

    std::string insert_cmd = "INSERT INTO " + table_name + "(id, name, start_year, end_year, img, text) VALUES (" + input_list[0]+ ",";
    for(size_t i = 1; i < list.size(); i++)
        insert_cmd += "'" + input_list[i] + "', ";

    insert_cmd.pop_back(); insert_cmd.pop_back();
    insert_cmd += ")";

    if(mysql_perform_query_input(Connect_maria, insert_cmd.c_str(), request, "This is duplicate data. (fail)")){

        std::cout << "database insert complete" << std::endl;
    	request.reply(status_codes::OK, U("insert complete"));                     
    }else
        std::cout << "fail" << std::endl;
}
