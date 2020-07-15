#include <iostream>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

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
		ctx.set_password_callback([](std::size_t max_length, boost::asio::ssl::context::password_purpose purpose)
       	{
           return "password";
       	});
		ctx.use_certificate_chain_file("rootca.crt");
		ctx.use_private_key_file("rootca.key", boost::asio::ssl::context::pem);
		ctx.use_tmp_dh_file("dh2048.pem");
	});


	listen_config.set_timeout(utility::seconds(10));
	http_listener listener(U("https://0.0.0.0:10022"), listen_config);        //Server URL, Port 지정.
		listener.support(methods::GET, [](http_request req){
		auto j = json::value::object();
		j[U("one")] = json::value::string(U("two"));
		req.reply(status_codes::OK, j);                      //Lamda방식으로 간단하게 구현.
		});
	
	listener.open().then([&listener](){std::cout << (U("\n start!!\n"));}).wait();    //Server open
	while(true);
	listener.close();
    return 0;
	
}
