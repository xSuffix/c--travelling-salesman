#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// #pragma execution_character_set("utf-8")

void loadData() // TODO Rückgabewert soll Pointer sein
{
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

  DistanceTable distanceTable = {
      .n = 0
  };

  printf("Wie heißt die Datei, in der die Daten gespeichert wurden? (Bei leerer Eingabe wird DistanceTable.txt verwendet.)\n");
  // TODO Nutzereingabe
  FILE *fpointer = fopen("DistanceTable.txt", "r");
  if (fpointer == NULL)
  {
    printf("Error opening file");
    exit(1); // TODO Funktionalität prüfen
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
          { // TODO handle failed realloc
            printf("error");
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
                // TODO Distance struct
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

    for (int k = 0; k < distanceTable.n * distanceTable.n; k++)
    { // TODO In showData() auslagern und evtl. überarbeiten
      Distance d = distanceTable.distances[k];
      printf("\n[%2d] - from: %d | to: %d | dist: %3d", k, d.from, d.to, d.dist);
    }

    fclose(fpointer);
    if (line)
    {
      free(line);
    }
  }
}

void saveData()
{
  printf("save");
}

void showData()
{
  printf("show");
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

typedef struct
{
  char key;
  char *description;
  void (*fun_ptr)();
} Menu;

Menu startMenu[] = {
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

  for (int i = 0; i < startMenuLength; i++)
  {
    if (c == startMenu[i].key)
    {
      startMenu[i].fun_ptr();
    }
  }

  return 0;
}