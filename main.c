#include <stdio.h>
#include <windows.h>

#pragma execution_character_set("utf-8")

struct menu
{
  char key;
  char *description;
};

struct menu startMenu[] = {
    {'a', "Entfernungstabelle laden"},
    {'b', "Entfernungstabelle speichern"},
    {'c', "Entfernungstabelle anzeigen"},
    {'d', "Entfernung zwischen zwei Städten ändern"},
    {'e', "Kürzeste Route berechnen"},
    {'f', "Programm beenden"},
};

int main()
{
  SetConsoleOutputCP(65001); // utf-8

  int startMenuLength = sizeof(startMenu) / sizeof startMenu[0];
  for (int i = 0; i < startMenuLength; i++) {
    printf("(%c) %s\n", startMenu[i].key, startMenu[i].description);
  }

  char c;
  scanf("%c", &c);

  
  return 0;
}