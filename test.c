#include <argp.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Todo {
  char *task;
  char id[3];
} Todo;

// void printAllTodos(Table *table) {
//   Entry *entries = ht_getAll(table);
//   printf("\n===== TODOS =====\n\n");
//   for (int i = 0; i < table->length; i++) {
//     Todo *todo = (Todo *)entries[i].val;
//     printf("->  %s\n", todo->task);
//   }
// }

time_t parseDateTime(char *dateTime) {

  time_t now = time(NULL);
  if (dateTime == NULL)
    return now;
  int i = 0;
  struct tm *formatted = localtime(&now);
  int dateTimeFormatFlag = 0;

  while (dateTime[i + 1] != '\0') {
    if (dateTime[i] == '-') {
      dateTimeFormatFlag = 1;
    }
    if (dateTime[i] == ':') {
      dateTimeFormatFlag = 1;
    }
    i++;
  }

  if (i == 0) {
    return now;
  }

  int offset = atoi(dateTime);

  if (dateTimeFormatFlag == 0) {
    switch (dateTime[i]) {
    case 'h': {
      now += (offset * 60 * 60);
      break;
    }
    case 'm': {
      now += (offset * 60);
      break;
    }
    case 'd': {
      now += (offset * 60 * 60 * 24);
      break;
    }
    case 'w': {
      now += (offset * 60 * 60 * 24 * 7);
      break;
    }
    case 'M': {
      if (offset > 11) {
        return -1;
      }
      if (formatted->tm_mon + offset > 11) {
        formatted->tm_year += 1;
        formatted->tm_mon += offset - 11;
      } else {
        formatted->tm_mon += offset;
      }
      now = mktime(formatted);
      break;
    }
    case 'Y': {
      formatted->tm_year += 1;
      now = mktime(formatted);
      break;
    }
    default: {
      return -1;
    }
    }
  } else {
    int dd, mm, yy, sec, min, hour;
    int parsedAmount =
        sscanf(dateTime, "%d-%d-%d %d:%d:%d", &dd, &mm, &yy, &hour, &min, &sec);
    if (parsedAmount >= 3) {
      formatted->tm_mday = dd;
      formatted->tm_mon = mm - 1;
      formatted->tm_year = yy + 100;
    }
    if (parsedAmount == 6) {
      formatted->tm_hour = hour;
      formatted->tm_min = min;
      formatted->tm_sec = sec;
    }
    now = mktime(formatted);
  }

  printf("Parsed: %s\n", ctime(&now));
  return now;
}

int createTableTodos(sqlite3 *db) {
  char sql[] = "CREATE TABLE todos (id INTEGER PRIMARY KEY, title VARCHAR(50), "
               " dueDate INTEGER, createdAt INTEGER);";
  char *err_msg;
  int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
  if (err_msg != NULL) {
    return 1;
  }
  free(err_msg);
  return 0;
}

char *formatDueTime(time_t dueTime) {
  time_t now = time(NULL);
  int diff = (int)dueTime - now;
  int isNeg = 0;
  char *out = calloc(50, sizeof(char));

  if (diff < 0) {
    isNeg = 1;
    diff *= -1;
  }

  int oneMin = 60;
  int oneHour = 60 * oneMin;
  int oneDay = oneHour * 24;
  int oneWeek = 7 * oneDay;
  float oneMonth = 30.4375 * oneDay;
  float oneYear = 365.25 * oneDay;

  if (diff > oneYear) {
    int years = diff / oneYear;
    float months = (float)(diff - years) / (float)oneMonth;
    isNeg ? sprintf(out, "Was %d Year, %0.2f Months ago.", years, months)
          : sprintf(out, "In %d Year, %0.2f Months.", years, months);
    return out;
  }
  if (diff > oneMonth) {
    int months = diff / oneMonth;
    int days = (diff - months) / oneDay;
    isNeg ? sprintf(out, "Was %d Months, %d days ago.", months, days)
          : sprintf(out, "In %d Months and %d days.", months, days);
    return out;
  }
  if (diff > oneDay) {
    int days = diff / oneDay;
    isNeg ? sprintf(out, "Was %d days ago.", days)
          : sprintf(out, "In %d days.", days);
    return out;
  }
  isNeg ? sprintf(out, "Was %0.2f hours ago.", ((float)diff / (float)oneHour))
        : sprintf(out, "In %0.2f hours.", ((float)diff / (float)oneHour));
  return out;
}

typedef enum { strikethrough, normal } printFormats;

void printWithSpaces(char *str, int length, printFormats format) {
  int i = 0;
  int ended = 0;
  while (i < length) {
    if (ended != 1) {
      format == strikethrough ? printf("%c\u0336", str[i])
                              : printf("%c", str[i]);
    }
    if (str[i] == '\0') {
      ended = 1;
    }
    if (ended == 1) {
      printf(" ");
    }
    i++;
  }
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  for (int i = 0; i < argc; i++) {
    switch (i) {
    case 0: {
      printf("#");
      printWithSpaces(argv[i], 5, normal);
      break;
    }
    case 1: {
      printWithSpaces(argv[i], 50, normal);
      break;
    }
    case 2: {
      time_t dueTime = atoi(argv[i]);
      argv[i] = formatDueTime(dueTime);
      argv[i][strcspn(argv[i], "\n")] = 0;
      printWithSpaces(argv[i], 35, normal);
      break;
    }
    }
    // if (i == 2) {
    //   time_t dueTime = atoi(argv[i]);
    //   argv[i] = formatDueTime(dueTime);
    //   argv[i][strcspn(argv[i], "\n")] = 0;
    // }
    // if (i == 0) {
    //   printf("#");
    // }
    // printf("%s\t", argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void printQuery(sqlite3 *db, char *query) {
  char *err_msg;
  if (query == NULL) {
    query = "SELECT * FROM TODOS";
  }

  int i = sqlite3_exec(db, query, callback, 0, &err_msg);
}

// puts("==========TODOS==========");

int main(int argc, char *argv[]) {
  sqlite3 *db;
  char *sql;
  char *err_msg;
  int rc = sqlite3_open("sqlite.db", &db);

  // strike through
  //  printf("%c\u0336", 'i');

  if (rc) {
    fprintf(stderr, "Can't connect to DB: %s", sqlite3_errmsg(db));
    return 1;
  }

  for (int i = 0; i < argc; i++) {

    if (strcmp(argv[i], "-a") == 0) {
      char *title = calloc(50, sizeof(char));
      char *dueDate = calloc(20, sizeof(char));
      time_t createdAt = parseDateTime(NULL);

      if (argc >= i + 1 && argv[i + 1] != NULL && argv[i + 1][0] != '-') {
        strcpy(title, argv[i + 1]);
      } else {
        printf("\nEnter Todo: ");
        fgets(title, 50, stdin);
        title[strcspn(title, "\n")] = 0;
      }
      if (argc >= i + 2 && argv[i + 2] != NULL && argv[i + 2][0] != '-') {
        strcpy(dueDate, argv[i + 2]);
      } else {
        printf("\nEnter Due Date (accepted formats: 'YYYY-MM-DD HH:MM:SS', "
               "'YYYY-MM-DD', or offsets like '1h', '30m', '2d', '1w', '2M', "
               "'1Y'):");
        fgets(dueDate, 20, stdin);
        dueDate[strcspn(dueDate, "\n")] = 0;
      }

      time_t parsedDate = parseDateTime(dueDate);

      sql = sqlite3_mprintf("INSERT INTO Todos (title, dueDate, "
                            "createdAt) VALUES (%Q,%lld,%lld);",
                            title, parsedDate, createdAt);
      puts("Created subject");
      rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
      if (rc != 0) {
        createTableTodos(db);
        rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
      }
    }
  }

  time_t currTime = time(NULL);
  char *query = calloc(100, sizeof(char));
  sprintf(query, "SELECT id, title, dueDate FROM TODOS WHERE dueDate >=%ld ",
          currTime);
  printQuery(db, query);

  return 0;
}
