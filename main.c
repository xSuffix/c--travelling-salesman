#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <unistd.h> //access()

// #pragma execution_character_set("utf-8")

typedef struct
{
  int from;
  int to;
  int dist;
} Distance;

typedef struct
{
  int n;
  char **cities;
  Distance *distances;
} DistanceTable;

typedef struct
{
  char key;
  char *description;
  void (*fun_ptr)();
} Menu;

DistanceTable loadData()
{
  DistanceTable distanceTable = {
      .n = 0};

  printf("Wie heißt die Datei, in der die Daten gespeichert wurden?\n");
  char path[100];
  scanf("%99s", &path[0]);
  printf("\n");

  FILE *fpointer = fopen(path, "r");
  if (fpointer == NULL)
  {
    if (access(path, F_OK) == 0)
    {
      printf("Error loading data: file is blocked by a program");
    }
    else
    {
      printf("Error loading data: file not found");
    }
    return distanceTable;
  }
  else
  {
    ssize_t read;
    char *line = NULL;
    size_t len = 10;
    int memcycle = 4;

    if ((read = getline(&line, &len, fpointer)) != -1)
    {
      char *city = strtok(line, "\n ");
      while (city != NULL)
      {
        if (distanceTable.n % memcycle == 0)
        { // Allocate memory every (memcycle)th step/word
          char **tmpCities;
          tmpCities = realloc(distanceTable.cities, (distanceTable.n + memcycle) * sizeof(char *));
          if (tmpCities == NULL)
          {
            printf("Error: Out of memory");
            exit(1);
          }
          else
          {
            distanceTable.cities = tmpCities;
          }
        }

        distanceTable.cities[distanceTable.n] = malloc((strlen(city) + 1) * sizeof(char)); // TODO maybe use calloc instead

        strcpy(distanceTable.cities[distanceTable.n], city);

        city = strtok(NULL, "\n ");
        distanceTable.n++;
      }

      distanceTable.distances = malloc(distanceTable.n * distanceTable.n * sizeof(Distance));
      for (int i = 0; i < distanceTable.n; i++)
      {
        read = getline(&line, &len, fpointer);
        if (read > 0)
        {
          char *distString = strtok(line, " \n");
          for (int j = 0; j < distanceTable.n; j++)
          {
            long dist = strtol(distString, NULL, 0);

            if (i == j) // start is destination
            {
              if (dist != 0)
              {
                printf("Error reading from file: Expected '0', got '%s' in line %u word %u", distString, i + 2, j + 1);
                i = j = distanceTable.n;
              }
              else
              {
                Distance distance = {
                    .from = i,
                    .to = j,
                    .dist = dist};
                distanceTable.distances[i * distanceTable.n + j] = distance;
              }
            }
            else
            {
              if (dist > 0)
              {
                Distance distance = {
                    .from = i,
                    .to = j,
                    .dist = dist};
                distanceTable.distances[i * distanceTable.n + j] = distance;
              }
              else
              {
                printf("Error reading from file: Expected number greater than 0, got '%s' in line %u word %u", distString, i + 2, j + 1);
                i = j = distanceTable.n;
              }
            }
            distString = strtok(NULL, "\n ");
          }
        }
        else
        {
          printf("Error reading from file: Line %u is empty\n", i);
          i = distanceTable.n;
        }
      }
    }

    fclose(fpointer);
    if (line)
    {
      free(line);
    }
    printf("\x1B[32mDie Entfernungstabelle wurde erfolgreich eingelesen (%s).\x1B[0m\n", path);
    return distanceTable;
  }
}

void saveData()
{
  printf("save");
}

void showData(DistanceTable distanceTable) //TODO Print if there are unsaved changes
{
  if (distanceTable.n)
  {
    for (int i = 0; i < distanceTable.n * distanceTable.n; i++)
    {
      printf("[%2d] - from: %d | to: %d | dist: %3d\n", i, distanceTable.distances[i].from, distanceTable.distances[i].to, distanceTable.distances[i].dist);
    }
  }
  else
  {
    printf("Die Entfernungstabelle ist leer\n");
  }
}

void changeDistanceBetweenCities()
{
  printf("change distance");
}

void calculateShortestRoute()
{
  printf("calculate");
}

void exitProgram()
{
  printf("exit");
}

void printMenu(Menu *menu, int length)
{
  printf("\n");
  for (int i = 0; i < length; i++)
  {
    printf("(%c) %s\n", menu[i].key, menu[i].description);
  }
}

int main()
{
  SetConsoleOutputCP(65001); // utf-8

  printf("--main\n");
  DistanceTable distanceTable = {
      .n = 0};

  Menu startMenu[] = {
      {'a', "Entfernungstabelle laden"},
      {'b', "Entfernungstabelle speichern"},
      {'c', "Entfernungstabelle anzeigen"},
      {'d', "Entfernung zwischen zwei Städten ändern"},
      {'e', "Kürzeste Route berechnen"},
      {'f', "Programm beenden"},
  };
  int startMenuLength = sizeof(startMenu) / sizeof(startMenu[0]);

  char c;
  do
  {
    printMenu(startMenu, startMenuLength);
    do
    {
      c = getchar();
    } while (c == '\n');
    printf("\n");
    switch (c)
    {
    case 'a':
      distanceTable = loadData();
      break;
    case 'b':
      saveData();
      break;
    case 'c':
      showData(distanceTable);
      break;
    case 'd':
      changeDistanceBetweenCities();
      break;
    case 'e':
      calculateShortestRoute();
      break;
    case 'f':
      exitProgram();
      break;
    default:
      printf("'%c' is not valid input", c);
    }
  } while (c != 'f');
  return 0;
}