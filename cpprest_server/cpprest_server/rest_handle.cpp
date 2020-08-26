#include "stdafx.hpp"

Handler::Handler(utility::string_t url, http_listener_config config, SQL_info myDB, 
    std::vector<utility::string_t> mytable_list) : m_listener(url, config), list(mytable_list){

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

MYSQL_RES* Handler::mysql_perform_query(MYSQL *connection, std::string select_cmd, http_request req) {
 
    if(mysql_query(connection, select_cmd.c_str())) {   //DB connection error handler
        
        printf("MYSQL query error : %s\n", mysql_error(connection)); 
        req.reply(status_codes::OK, U("mysql error")); 
        return NULL;
    }
    
    return mysql_use_result(connection);
}

void Handler::handle_get(http_request request){     //Processing as json data when requesting GET.
    
    std::cout << "handle_get request" << std::endl;

    int i = 1;
    auto j = json::value::object();
    json::value j_list;
    MYSQL_RES *res;
    MYSQL_ROW row;
    
    std::string select_cmd = "select * from table1";
    res = mysql_perform_query(Connect_maria, select_cmd, request);
    if(res == NULL) return;

    while((row = mysql_fetch_row(res)) != NULL){    

        std::string s = std::to_string(i); i++;
        j[U("id")] = json::value::string(row[0]);
        j[U("name")] = json::value::string(row[1]);
        j[U("start_year")] = json::value::string(row[2]);
        j[U("end_year")] = json::value::string(row[3]);
        j[U("img")] = json::value::string(row[4]);
        j[U("text")] = json::value::string(row[5]);
        printf("%s %s %s %s %s %s\n", row[0], row[1], row[2], row[3], row[4], row[5]);
        j_list[U(s.c_str())] = j;
    }

	request.reply(status_codes::OK, j_list);                     
	mysql_free_result(res);
}

void Handler::handle_del(http_request request){
    
    std::cout << request.to_string() << std::endl;
    request.reply(status_codes::OK); 
}

void Handler::handle_post(http_request request){

    std::cout << "handle_put request" << std::endl;

    std::vector<std::string> update_list;
    std::vector<std::string> key;
    auto j = request.extract_json().get(); 

    for(size_t i = 0; i < 6; i++){
        
        if(j.has_field(U(list[i]))){

            key.emplace_back(list[i]);
            update_list.emplace_back(j[U(list[i]]).serialize());
            update_list.back().erase(0, 1);
            update_list.back().pop_back();
        }
    }

    std::string update_cmd = "UPDATE table1 SET ";
    for(size_t i = 1; i < key.size(); i++)
        update_cmd += key[i] + " = " + "'" + update_list[i] + "', ";
    
    update_cmd.pop_back(); update_cmd.pop_back();
    update_cmd += " WHERE " + key[0] + " = " + update_list[0];

    if(!mysql_query(Connect_maria, update_cmd.c_str())){
    
        std::cout << "database update complete" << std::endl;
    	request.reply(status_codes::OK, U("update complete"));           

    } else {

        printf("MYSQL query error : %s\n", mysql_error(Connect_maria));
        request.reply(status_codes::OK, U("Database modification failed."));
    }
}

void Handler::handle_put(http_request request){     //Serialize the json data received at the time of PUT request.
    
    std::cout << "handle_put request" << std::endl;
   
    std::vector<std::string> input_list(list.size());
    auto j = request.extract_json().get();

    for(size_t i = 0; i < j.size(); i++){
        
        input_list[i] = j[U(list[i]]).serialize();
        input_list[i].erase(0, 1);
        input_list[i].pop_back();
    }

    std::string insert_cmd = "INSERT INTO table1(id, name, start_year, end_year, img, text) VALUES (" + input_list[0]+ ",";
    for(size_t i = 1; i < list.size(); i++)
        insert_cmd += "'" + input_list[i] + "', ";

    insert_cmd.pop_back(); insert_cmd.pop_back();
    insert_cmd += ")";
    
    if(!mysql_query(Connect_maria, insert_cmd.c_str())){
    
        std::cout << "database insert complete" << std::endl;
    	request.reply(status_codes::OK, U("insert complete"));                     

    } else {

        printf("MYSQL query error : %s\n", mysql_error(Connect_maria));
        request.reply(status_codes::OK, U("This is duplicate data."));
    }
}