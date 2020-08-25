# C++ Restful server

Hi, This code is based on the Microsoft open source project cpprestSDK.
It works with MariaDB and communicates with the client in a Json way.
It also supports Https and was created using boost's ssl library. Certificate generation was created using openssl.


Here is my sample code. [My samplecode](https://hwan-shell.tistory.com/category/%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%98%EB%B0%8D/C%2B%2B%20rest%28Casablanca%29)
If you want a reference to cpprestSDK here. [Cpprestful reference](https://microsoft.github.io/cpprestsdk/namespaces.html)

## The development environment is as follows.

Platfrom : Docker

OS : Ubuntu 20.04

Compiler : GCC/G++

DB : MariaDB

Code editor : Visual Studio Code


## Install

Install cpprestSDK if you want to use that code.

With apt-get on Debian/Ubuntu

```$ sudo apt-get install libcpprest-dev```

Install MariaDB

```$ sudo apt-get install libmariadb-dev```

```$ sudo apt-get install libmysqlclient-dev```

## How to build my project code.

**<p>1. MariaDB service start.</p>**

``` $ service mysql restart```

**<p>2. stdafx.hpp compile</p>**

```$ sudo ./build_hpp.sh```

**<p>3. Modify the main.cpp code.</p>**

In the main.cpp code, you need to set it according to your DB table environment.
Part Part is ```mariaID``` and elements of ```mytable_list```

**<p>4. MakeFile </p>**

```$ make```

**<p>5. Start server!! </p>**

```$ sudo ./start.exe```


## Use **curl** for a simple communication check.

GET request

```$ curl -X GET https://localhost:10022```

PUT request

```$ curl -d '{"id":"1", "name":"hwan", "start_year":"2020-08-23", "end_year":"2020-08-23", "img":" ", "text":"test"}' -H "Content-Type: application/json" -X PUT http://localhost:1002```

POST request

```curl -d '{"id":"1", "name":"hwan", "img":" ", "text":"test"}' -H "Content-Type: application/json" -X POST http://localhost:10022```

**Thanks!!**
