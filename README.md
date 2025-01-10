# MHD Postgres API

a simple RESTful API built with microhttpd that communicates with a Postgres server

prerequisites: libmicrohttpd, postgresql, cJSON
compile with: gcc server.c -o a.out -lmicrohttpd -lpq -lcjson