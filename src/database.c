#include <libpq-fe.h>
#include <stdlib.h>
#include "database.h"

PGconn* initialize_db(){
    PGconn *conn = PQconnectdb(CONN_STR);

    if (PQstatus(conn) != CONNECTION_OK) {
        exit_with_error(conn, "connecting to database");
    }

    prepare_statement(conn, "get_all_monkeys", "SELECT id, name, price, type FROM monkeys ORDER BY id asc", 0);

    prepare_statement(conn, "get_monkey_by_id", "SELECT id, name, price, type FROM monkeys WHERE id = $1", 1);

    prepare_statement(conn, "create_monkey", "insert into monkeys(name, price, type) values ($1, $2, $3) returning id", 3);

    prepare_statement(conn, "update_monkey", "update monkeys set name = $1, price = $2, type = $3 where id = $4 returning id", 4);

    prepare_statement(conn, "delete_monkey", "DELETE FROM monkeys WHERE id = $1 returning id", 1);

    return conn;
}

void prepare_statement(PGconn *conn, char *name, char *query, int param_amount){
    PGresult *res;

    res = PQprepare(conn,name, query, param_amount, NULL);
                    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        PQclear(res);
        exit_with_error(conn, "preparing statement");
    }

    PQclear(res);
}

void exit_with_error(PGconn *conn, char *operation) {
    fprintf(stderr, "database error while %s: %s\n", operation, PQerrorMessage(conn));
    PQfinish(conn);
    exit(EXIT_FAILURE);
}