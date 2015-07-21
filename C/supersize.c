#include <string.h>
#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>

#define CHUNK_SIZE 100000000

int
main (int argc, char *argv[])
{
  static char *opt_hostname = "::1";
  static char *opt_username = "msandbox";
  static char *opt_password = "msandbox";
  static unsigned int opt_portnr = 18675;
  static char *opt_sockname = NULL;
  static char *opt_database = "test";
  static unsigned int opt_flags = 0; 

  MYSQL_STMT *stmt;
  char *insert_stmt = "INSERT INTO supersize(data) VALUES(?)";
  MYSQL_BIND param[1];

  static MYSQL *conn;

  MY_INIT(argv[0]);
  if (mysql_library_init(0, NULL, NULL))
  {
    fprintf(stderr, "mysql_library_init() failed.\n");
    exit(1);
  }

  conn = mysql_init(NULL);
  if (conn == NULL)
  {
    fprintf(stderr, "mysql_init() failed.\n");
    exit(2);
  }

  if (mysql_real_connect(conn, opt_hostname, opt_username, opt_password,
                         opt_database, opt_portnr, opt_sockname, opt_flags) == NULL)
  {
    fprintf(stderr, "mysql_real_connect() failed.\n");
    mysql_close(conn);
    exit(3);
  }

  if (mysql_query(conn, "DROP TABLE IF EXISTS supersize") != 0)
  {
    fprintf(stderr, "Failed to drop supersize table.\n"); 
  }
  
  if (mysql_query(conn, "CREATE TABLE supersize(\
id int unsigned auto_increment primary key,\
data longblob) DEFAULT CHARSET latin1 ENGINE=BLACKHOLE") != 0)
  {
    fprintf(stderr, "Failed to create supersize table.\n"); 
  }

  if ((stmt = mysql_stmt_init(conn)) == NULL)
  {
    fprintf(stderr, "mysql_stmt_init() failed.\n");
    mysql_close(conn);
    exit(4);
  }

  if (mysql_stmt_prepare(stmt, insert_stmt, strlen(insert_stmt)) != 0)
  {
    fprintf(stderr, mysql_stmt_error(stmt));
    fprintf(stderr, "mysql_stmt_prepare() failed.\n");
    mysql_close(conn);
    exit(5);
  }

  memset((void *) param, 0, sizeof(param));

  param[0].buffer_type = MYSQL_TYPE_STRING;
  param[0].is_null = 0;

  if (mysql_stmt_bind_param(stmt, param) != 0)
  {
    fprintf(stderr, "mysql_stmt_bind_param() failed.\n");
    mysql_close(conn);
    exit(6);
  }

  int i;
  while (i < 27) {
    char *chunk = malloc(CHUNK_SIZE);
    
    printf("\nSending %d chucks of %d chars (Total: %i MiB).\n", i, CHUNK_SIZE, (CHUNK_SIZE * i)/1024/1024);

    int j;
    for (j = 0; j<i; j++)
    {
      printf(" %d", j);
      fflush(stdout);

      memset(chunk, 97 + j, CHUNK_SIZE);
      chunk[CHUNK_SIZE] = '\n';

      if (mysql_stmt_send_long_data(stmt,0,chunk,CHUNK_SIZE))
      {
        fprintf(stderr, "\n send_long_data failed\n");
        fprintf(stderr, "\n %s\n", mysql_stmt_error(stmt));
        exit(7);
      } 
    }

    free(chunk);

    if (mysql_stmt_execute(stmt))
    {
      fprintf(stderr, "\n mysql_stmt_execute failed.\n");
      fprintf(stderr, "\n %s\n", mysql_stmt_error(stmt));
      exit(8);
    }

    if (mysql_query(conn, "TRUNCATE TABLE supersize") != 0)
    {
      fprintf(stderr, "Failed to drop supersize table.\n"); 
    }

    i++;
  }

  mysql_close(conn);
  mysql_library_end();
  exit(0); 
}
