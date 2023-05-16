#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>

#define SQL_FILE "testDB.db"


// 除了第一个参数是可自己定义的， 另外三个参数是sql调用callback时 自动追加的？
// 如： (*callback)(slef, xx, xx, xx);    ?
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)NotUsed);     // no
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int main(int argc, char* argv[])
{
   sqlite3 *db;
   char *zErrMsg = 0;
   int  rc;
   char *sql, *sql_create, *sql_show;
   const char* data = "Callback function called";     // SELECT * FROM： printf title

   /* Open database */
   rc = sqlite3_open("test.db", &db);           // SQL1：打开sql文件
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stdout, "Opened database successfully\n");
   }

    /* Create SQL statement */      // 创建一个新表
   sql_create = "CREATE TABLE COMPANY("  \
         "ID INT PRIMARY KEY     NOT NULL," \
         "NAME           TEXT    NOT NULL," \
         "AGE            INT     NOT NULL," \
         "ADDRESS        CHAR(50)," \
         "SALARY         REAL );";

    /* Create SQL statement */      // 在表中添加表项
   sql = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
         "VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
         "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
         "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" \
         "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
         "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";

   sql_show = "SELECT * from COMPANY";    // printf COMPANY table;   // SQL: add shell cmd

   sql_show = "DELETE from COMPANY where ID=2; " \          // SQL: RUN more shellCmd....
            "SELECT * from COMPANY";

   //rc = sqlite3_exec(db, sql_create, callback, 0, &zErrMsg);      // SQL2：use set option;
   //rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql_show, callback, (void*)0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Operation done successfully\n");
   }


   sqlite3_close(db);                                                // SQL3：close connect;
   return 0;
}







// sqlite关键函数：
//1、sqlite3_open(const char* filename, sqlite3 **ppDb);		// 打开filename的SQL文件
//2、int sqlite3_exec(
//       sqlite3*,                                  /* An open database */
//       const char *sql,                           /* SQL to be evaluated */
//       int (*callback)(void*,int,char**,char**),  /* Callback function */
//       void *,                                    /* 1st argument to callback */
//       char **errmsg                              /* Error msg written here */
//    );
//3、sqlite3_close(sqlite3*);


