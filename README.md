# MHD Postgres API

a simple RESTful API built with microhttpd that communicates with a Postgres server

comments are simply my annotations since this project is mainly for educational purposes

prerequisites: libmicrohttpd, postgresql, cJSON

compile with:
```
gcc -I./include src/*.c src/actions/*.c -o a.out -lmicrohttpd -lpq -lcjson
```