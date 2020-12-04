#include <stdio.h>
#include <windows.h>

#pragma execution_character_set("utf-8")

void loadData() {
  printf("load");
}

void saveData() {
  printf("save");
}

void showData() {
  printf("show");
}

void changeDistanceBetweenCities() {
  printf("change distance");
}

void calculateShortestRoute() {
  printf("calculate");
}

void exitProgram() {
  printf("exit");
}

struct menu
{
  char key;
  char *description;
  void (*fun_ptr)();
};

struct menu startMenu[] = {
    {'a', "Entfernungstabelle laden", &loadData},
    {'b', "Entfernungstabelle speichern", &saveData},
    {'c', "Entfernungstabelle anzeigen", &showData},
    {'d', "Entfernung zwischen zwei Städten ändern", &changeDistanceBetweenCities},
    {'e', "Kürzeste Route berechnen", &calculateShortestRoute},
    {'f', "Programm beenden", &exitProgram},
};

int main()
{
  SetConsoleOutputCP(65001); // utf-8

  int startMenuLength = sizeof(startMenu) / sizeof startMenu[0];
  for (int i = 0; i < startMenuLength; i++)
  {
    printf("(%c) %s\n", startMenu[i].key, startMenu[i].description);
  }

  char c;
  scanf("%c", &c);
  printf("\n");

  for (int i = 0; i < startMenuLength; i++) {
    if (c == startMenu[i].key) {
      startMenu[i].fun_ptr();
    }
  }

  return 0;
}