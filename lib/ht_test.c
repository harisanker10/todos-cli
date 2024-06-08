#include "hashmap.h"
#include "stdio.h"

typedef struct {
  char *name;
  int age;
} person;

int main(int argc, char *argv[]) {

  Table *table = ht_create();
  int test_size = 1000 * 100 * 10;

  for (int i = 1; i <= test_size; i++) {
    char buf[20];
    sprintf(buf, "%d", i);
    char key[60] = "key";
    strcat(key, buf);
    person *hari = malloc(sizeof(person));
    hari->age = i;
    hari->name = key;
    // printf("setting name:%s, age:%d\n", hari->name, hari->age);
    ht_set(table, key, hari);
  }

  int count = 0;

  for (int i = 1; i <= test_size; i++) {
    char buf[20];
    sprintf(buf, "%d", i);
    char key[60] = "key";
    strcat(key, buf);
    person *guy = (person *)ht_get(table, key);
    if (guy != NULL) {
      if (strcmp(guy->name, key) == 0 && guy->age == i) {
        count++;
        if (i == test_size - 1) {
          printf("\nTest passed for %d cases\n", test_size);
        }
      } else {
        printf("Test Failed at %d\n", i);
        printf("Got name:%s, age:%d\n", guy->name, guy->age);
      }
    } else {
      printf("Test failed, value not found for key%d\n", i);
    }
  }

  printf("Passed: %d tests \n", count);

  return 0;
}
