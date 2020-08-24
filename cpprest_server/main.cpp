#include "stdafx.hpp"

std::unique_ptr<Handler> listener;

void start_server(utility::string_t& url, http_listener_config config, SQL_info myDB, std::vector<utility::string_t>& mytable_list){

	listener = std::unique_ptr<Handler>(new Handler(url, config, myDB, mytable_list));
	std::cout << "start server" << std::endl;
	listener->open().wait();
}

int main(){
	
	http_listener_config listen_config;
	listen_config.set_ssl_context_callback([](boost::asio::ssl::context &ctx)
	{

		ctx.set_options(
			boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::no_tlsv1
			| boost::asio::ssl::context::no_tlsv1_1
			| boost::asio::ssl::context::single_dh_use);

		ctx.set_password_callback([](std::size_t, 
			boost::asio::ssl::context::password_purpose)
       	{ return "test"; });

		ctx.use_certificate_chain_file("rootca.crt");
		ctx.use_private_key_file("rootca.key", boost::asio::ssl::context::pem);
		ctx.use_tmp_dh_file("dh2048.pem");

	});

    SQL_info mariaID;
    mariaID.server = "localhost";
    mariaID.user = "root";
    mariaID.password = "root";
    mariaID.database = "test";
	
	std::vector<utility::string_t> mytable_list;
	mytable_list.emplace_back("id");
	mytable_list.emplace_back("name");
	mytable_list.emplace_back("start_year");
	mytable_list.emplace_back("end_year");
	mytable_list.emplace_back("img");
	mytable_list.emplace_back("text");

  
	listen_config.set_timeout(utility::seconds(10));
	utility::string_t url = U("http://0.0.0.0:10022");        

	start_server(url, listen_config, mariaID, mytable_list);
	while(true);
	listener->close().wait();
    return 0;
}
