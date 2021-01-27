// Kurs INF20A
// Bearbeiter: Jan Fröhlich, Gabriel Nill, Fabian Weller

// TODO: Offene Fragen:
// - Soll die Datei, in die gespeichert und von der gelesen wird, immer mit .txt enden?

#include <ctype.h> // iscntrl() isspace() Funktionen für ASCII-Zeichen
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>  // Ein- und Ausgabefunktionen
#include <stdlib.h> // Stringkonvertierung, Zufallszahlen, Speicherallokation, Sortieren u.a.
#include <string.h> // prototype for strtok() because gcc expects int as return type

#ifdef _WIN32
#include <windows.h>
#define COLOR_PRIMARY 9
#define COLOR_SUCCESS 10
#define COLOR_INFO 11
#define COLOR_ERROR 12
#define COLOR_WARNING 6
#define COLOR_DEFAULT 15

#else
#define COLOR_PRIMARY 34
#define COLOR_SUCCESS 32
#define COLOR_INFO 36
#define COLOR_ERROR 31
#define COLOR_WARNING 33
#define COLOR_DEFAULT 0
#endif

typedef struct {
  int from;
  int to;
  int dist;
} Distance;

typedef struct {
  int n;
  char **cities;
  Distance *distances;
} DistanceTable;

typedef struct {
  char key;
  char *description;
} Menu;

// Portable function for changing the console color
void setConsoleColor(int color) {
#ifdef _WIN32
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#else
  printf("\033[%dm", color);
#endif
}

// Return amount of digits in integer (0 => 1, 53 => 2, 1234 => 4)
int intDigits(int n) {
  if (n < 0)
    return intDigits(-n);
  if (n < 10)
    return 1;
  return 1 + intDigits(n / 10);
}

// Return length of string with support for UTF-8 (ö counts as one character)
int strlen_utf8(char *s) {
  int i = 0, j = 0;
  while (s[i]) {
    if ((s[i] & 0xc0) != 0x80)
      j++;
    i++;
  }
  return j;
}

// Norm a string to a specific minimum/maximum length with support for UTF-8
// If string is smaller than minimum, fill the rest with the filler char. If it's larger, crop
char *substr_utf8(const char *src, size_t min, size_t max, char filler) {
  size_t i = 0, j = 0;
  while (j < max && src[i]) {
    if ((src[i] & 0xc0) != 0x80)
      j++;
    i++;
  }

  char *substr = "";
  substr = calloc((i > min ? i : min) + 1, sizeof(char)); // TODO free()
  memcpy(substr, &src[0], i);
  substr[i] = '\0';

  for (int k = 0; min > k + j; k++) {
    strncat(substr, &filler, 1);
  }
  return substr;
}

// Scan console for a file path and return it
char *scanFilePath() {
  setConsoleColor(COLOR_PRIMARY);
  char *path = calloc(PATH_MAX, sizeof(char));
  char fmt[64];
  snprintf(fmt, sizeof fmt, "%%%ds", PATH_MAX - 1);
  scanf(fmt, &path[0]);
  printf("\n");
  return path;
}

// Load the distance table from a file
DistanceTable *loadData() {
  DistanceTable *distanceTable = malloc(sizeof(DistanceTable));
  distanceTable->n = 0;
  distanceTable->cities = NULL;

  // Retrieve path to file
  setConsoleColor(COLOR_DEFAULT);
  printf("Bitte geben Sie den Pfad zu der Textdatei an, die geladen werden soll.\n");
  char *path = scanFilePath();
  if (path[0] <= 0) { // No input, program might have been terminated (ctrl + c)
    setConsoleColor(COLOR_DEFAULT);
    return NULL;
  }

  FILE *fpointer = fopen(path, "r");
  if (fpointer == NULL) {
    setConsoleColor(COLOR_ERROR);
    printf("Fehler: Die Datei konnte nicht geöffnet werden. (%s)\n", path);
    return NULL;

  } else {
    char *line = NULL;
    size_t len = 0;
    int memcycle = 8;

    // Read city names from first line and store them in distanceTable->cities
    if (getline(&line, &len, fpointer) > 0) {
      for (char *city = strtok(line, "\n "); city != NULL; city = strtok(NULL, "\n ")) {
        // Allocate memory for char pointers every (memcycle)th step/word
        if (distanceTable->n % memcycle == 0) {
          char **tmpCities;
          tmpCities = realloc(distanceTable->cities, (distanceTable->n + memcycle) * sizeof(char *));
          if (tmpCities == NULL) {
            setConsoleColor(COLOR_ERROR);
            printf("Fehler: Die Zuweisung von Arbeitsspeicher ist fehlgeschlagen.\n");
            return NULL;
          } else {
            distanceTable->cities = tmpCities;
          }
        }

        // Store the actual string/city
        distanceTable->cities[distanceTable->n] = calloc(strlen(city) + 1, sizeof(char));
        strcpy(distanceTable->cities[distanceTable->n++], city);
      }
    } else {
      setConsoleColor(COLOR_ERROR);
      printf("Fehler beim Einlesen: Die Datei konnte nicht gelesen werden. Stellen Sie sicher, dass keine anderen Prozese darauf zugreifen und das Format eingehalten wurde.\n");
      return NULL;
    }

    // Read distances between cities from second to (n+1)th line and store them in distanceTable->distances
    distanceTable->distances = calloc(distanceTable->n * distanceTable->n, sizeof(Distance));
    for (int i = 0; i < distanceTable->n; i++) {
      if (getline(&line, &len, fpointer) > 0) {
        char *distString = strtok(line, " \n");
        for (int j = 0; j < distanceTable->n; j++) {
          int dist = strtol(distString, NULL, 0);

          // Check if dist is valid
          if (i == j) { // start city == destination
            if (dist != 0) {
              setConsoleColor(COLOR_ERROR);
              printf("Fehler beim Einlesen: Die Entfernung einer Stadt zu sich selbst muss immer 0 sein, in der Datei steht jedoch \"%s\" (%u. Zeile, %u. Wort).\n", distString, i + 2, j + 1);
              return NULL;
            } else {
              Distance distance = {.from = i, .to = j, .dist = dist};
              distanceTable->distances[i * distanceTable->n + j] = distance;
            }
          } else {
            if (dist > 0) {
              Distance distance = {.from = i, .to = j, .dist = dist};
              distanceTable->distances[i * distanceTable->n + j] = distance;
            } else {
              setConsoleColor(COLOR_ERROR);
              printf("Fehler beim Einlesen: Die Entfernung zwischen verschiedenen Städten muss größer als 0 sein, in der Datei steht jedoch \"%s\" (%u. Zeile, %u. Wort).\n", distString, i + 2, j + 1);
              return NULL;
            }
          }
          distString = strtok(NULL, "\n ");
        }
      } else {
        setConsoleColor(COLOR_ERROR);
        printf("Fehler beim Einlesen: Zeile %u ist leer. Stellen Sie sicher, dass es gleich viele Zeilen wie Städte gibt.\n", i);
        return NULL;
      }
    }

    fclose(fpointer);

    setConsoleColor(COLOR_SUCCESS);
    printf("Die Entfernungstabelle wurde erfolgreich geladen! (%s)\n", path);
    return distanceTable;
  }
}

int saveData(DistanceTable *distanceTable) {
  if (distanceTable) {
    setConsoleColor(COLOR_DEFAULT);
    printf("Unter welchem Namen soll die Datei gespeichert werden?\n");

    char *path = scanFilePath();
    if (path[0] <= 0) {
      setConsoleColor(COLOR_DEFAULT);
      return 0;
    }

    FILE *fpointer = fopen(path, "w");
    if (fpointer) {

      for (int i = 0; i < distanceTable->n; i++) {
        fprintf(fpointer, "%s ", distanceTable->cities[i]);
      }
      fprintf(fpointer, "\n");

      for (int i = 0; i < distanceTable->n; i++) {
        for (int j = 0; j < distanceTable->n; j++) {
          fprintf(fpointer, "%d ", distanceTable->distances[distanceTable->n * i + j].dist);
        }
        fprintf(fpointer, "\n");
      }
      fclose(fpointer);

      setConsoleColor(COLOR_SUCCESS);
      printf("Die Entfernungstabelle wurde erfolgreich gespeichert! (%s)\n", path);

    } else {
      setConsoleColor(COLOR_ERROR);
      printf("Fehler: Die Datei konnte unter diesem Pfad nicht gespeichert werden. (%s)\n", path);
    }

    return 1;
  }
  setConsoleColor(COLOR_ERROR);
  printf("Fehler: Bitte laden Sie zunächst eine Entfernungstabelle.\n");
  return 0;
}

void showData(DistanceTable *distanceTable) { // TODO Print if there are unsaved changes
  if (distanceTable) {
    int largestCityNameLength = 0;

    // Initialize length of column (in chars) with minimum value of 3 // TODO change to array
    int *columnLengths;
    columnLengths = calloc(distanceTable->n, sizeof(int));
    for (int i = 0; i < distanceTable->n; i++) {
      columnLengths[i] = 3;
    }

    for (int i = 0; i < distanceTable->n; i++) {
      // Find out how many characters the longest city name has
      int cityNameLength = strlen_utf8(distanceTable->cities[i]);
      if (cityNameLength > largestCityNameLength)
        largestCityNameLength = cityNameLength;

      for (int j = 0; j < distanceTable->n; j++) {
        // Find out how many digits the largest distance has
        int distanceLength = intDigits(distanceTable->distances[i * 5 + j].dist);
        if (distanceLength > columnLengths[j])
          columnLengths[j] = distanceLength;
      }
    }

    // Print title row
    printf("%*s", largestCityNameLength + 1, ""); // Margin for the first line
    setConsoleColor(COLOR_INFO);
    for (int i = 0; i < distanceTable->n; i++) {
      // Print titles of columns
      printf("%s ", substr_utf8(distanceTable->cities[i], columnLengths[i], columnLengths[i], ' '));
    }
    printf("\n");

    // Print rows except for title row
    for (int i = 0; i < distanceTable->n; i++) {
      setConsoleColor(COLOR_INFO);
      // Print titles of rows with margin, so that the entire row has the same width
      printf("%s ", substr_utf8(distanceTable->cities[i], largestCityNameLength, largestCityNameLength, ' '));
      setConsoleColor(COLOR_DEFAULT);
      for (int j = 0; j < distanceTable->n; j++) {
        // Print digits (distances)
        printf("%*d ", columnLengths[j], distanceTable->distances[i * distanceTable->n + j].dist);
      }
      printf("\n");
    }
  } else {
    setConsoleColor(COLOR_WARNING);
    printf("Die Entfernungstabelle ist leer.\n");
  }
}

int getCityNumber(DistanceTable *distanceTable, char city[100]) {
  for (int i = 0; i < distanceTable->n; i++) {
    if (strcmp(distanceTable->cities[i], city) == 0) {
      return i;
    }
  }
  return -1; // City was not found
}

Distance *getDistanceStructBetweenCities(DistanceTable *distanceTable, int from, int to) {
  for (int i = 0; i < distanceTable->n * distanceTable->n - 1; i++) {
    if (from == distanceTable->distances[i].from && to == distanceTable->distances[i].to)
      return &distanceTable->distances[i];
  }

  return 0;
}

// This checks, if the entered distance is positive and fits into an Integer. So the Long can be safely typecasted to an Int.
bool checkForInvalidDistance(long num) {
  if (num < 1) {
    setConsoleColor(COLOR_ERROR);
    printf("Die Entfernung muss größer als 0 sein.\n\n");
    return true; 
  } else if (num > INT_MAX) {
    setConsoleColor(COLOR_ERROR);
    printf("Die Entfernung ist zu groß.\n\n");
    return true;
  }
  return false;
}

void changeDistanceBetweenCities(DistanceTable *distanceTable) {
  if (distanceTable->n == 0) {
    setConsoleColor(COLOR_ERROR);
    printf("Bitte laden Sie zuerst eine Entfernungstabelle.\n");
    return;
  }

  bool invalid;
  char firstCity[100];
  char secondCity[100];
  int firstCityNumber;
  int secondCityNumber;

  do
  {
    do
    {
      invalid = false;
      setConsoleColor(COLOR_DEFAULT);
      printf("Bitte geben Sie den Namen der ersten Stadt ein:\n");
      setConsoleColor(COLOR_PRIMARY);
      scanf("%s", firstCity);

      firstCityNumber = getCityNumber(distanceTable, firstCity);
      if (firstCityNumber == -1) {
        setConsoleColor(COLOR_ERROR);
        printf("Diese Stadt konnte in der Entferungstabelle nicht gefunden werden.\n");
        printf("Bitte versuchen Sie es erneut.\n\n");
        invalid = true;
        }
      } while (invalid);

    printf("\n");

    do
    {
      setConsoleColor(COLOR_DEFAULT);
      printf("Bitte geben Sie den Namen der zweiten Stadt ein:\n");
      setConsoleColor(COLOR_PRIMARY);
      scanf("%s", secondCity);

      secondCityNumber = getCityNumber(distanceTable, secondCity);
      if (secondCityNumber == -1) {
        setConsoleColor(COLOR_ERROR);
        printf("Diese Stadt konnte in der Entferungstabelle nicht gefunden werden.\n");
        printf("Bitte versuchen Sie es erneut.\n\n");
        invalid = true;
      }
    } while (invalid);

    if (firstCityNumber == secondCityNumber)
    {
      setConsoleColor(COLOR_ERROR);
      printf("Die Städte sind identisch. Bitte geben Sie verschiedene Städte an.\n\n");
      invalid = true;
    }
    
  } while (invalid);
  
  printf("\n");

  Distance *firstToSecond = getDistanceStructBetweenCities(distanceTable, firstCityNumber, secondCityNumber);
  Distance *secondToFirst = getDistanceStructBetweenCities(distanceTable, secondCityNumber, firstCityNumber);
  setConsoleColor(COLOR_DEFAULT);
  printf("Aktuelle Entfernungen:\n");
  printf("%s nach %s: %d\n", firstCity, secondCity, firstToSecond->dist);
  printf("%s nach %s: %d\n", secondCity, firstCity, secondToFirst->dist);
  printf("\n");

  int firstDist;
  int secondDist;
  char input[100];
  long newDistance;

  do {
    invalid = false;
    printf("Bitte geben Sie eine neue Entfernung für die Strecke von %s nach %s ein:\n", firstCity, secondCity);
    setConsoleColor(COLOR_PRIMARY);
    scanf("%s", input);
    newDistance = strtol(input, NULL, 10);

    invalid = checkForInvalidDistance(newDistance);

  } while (invalid);
  firstDist = (int)newDistance;

  printf("\n");

  do {
    invalid = false;
    setConsoleColor(COLOR_DEFAULT);
    printf("Bitte gib eine neue Entfernung für die Strecke von %s nach %s ein:\n", secondCity, firstCity);
    setConsoleColor(COLOR_PRIMARY);
    scanf("%s", input);
    newDistance = strtol(input, NULL, 10);

    invalid = checkForInvalidDistance(newDistance);
    
  } while (invalid);
  secondDist = (int)newDistance;

  // Replace distances
  firstToSecond->dist = firstDist;
  secondToFirst->dist = secondDist;
  
  setConsoleColor(COLOR_SUCCESS);
  printf("\n");
  printf("Die Entfernung wurde erfolgreich geändert!\n");
}

void calculateShortestRoute() {
  printf("calculate");
}

int exitProgram() {
  setConsoleColor(COLOR_DEFAULT);
  printf("Das Programm wurde beendet.\n");
  return 1;
}

// Print all options of a menu
void printMenu(Menu *menu, int length) {
  printf("\n");
  for (int i = 0; i < length; i++) {
    printf("(%c) %s\n", menu[i].key, menu[i].description);
  }
}

int main() {
#ifdef _WIN32
  SetConsoleOutputCP(65001); // Charset UTF-8
#endif

  DistanceTable *distanceTable = NULL;
  DistanceTable *tmpDistanceTable = NULL;

  Menu startMenu[] = {
      {'a', "Entfernungstabelle laden"},
      {'b', "Entfernungstabelle speichern"},
      {'c', "Entfernungstabelle anzeigen"},
      {'d', "Entfernung zwischen zwei Städten ändern"},
      {'e', "Kürzeste Route berechnen"},
      {'f', "Programm beenden"},
  };
  int startMenuLength = sizeof(startMenu) / sizeof(startMenu[0]);

  // Menu
  char c;
  do {
    setConsoleColor(COLOR_DEFAULT);
    printMenu(startMenu, startMenuLength);

    setConsoleColor(COLOR_PRIMARY);
    do {
      c = getchar();
    } while (isspace(c));
    printf("\n");

    switch (c) {
    case 'a':
      tmpDistanceTable = loadData();
      if (tmpDistanceTable)
        distanceTable = tmpDistanceTable;
      break;
    case 'b':
      saveData(distanceTable);
      break;
    case 'c':
      showData(distanceTable);
      break;
    case 'd':
      changeDistanceBetweenCities(distanceTable);
      break;
    case 'e':
      calculateShortestRoute();
      break;
    case 'f': {
      if (exitProgram()) {
        // Free memory
        if (distanceTable) {
          if (distanceTable->cities) {
            for (int i = 0; i < distanceTable->n; i++) {
              free(distanceTable->cities[i]);
            }
            free(distanceTable->cities);
          }
          if (distanceTable->distances) {
            free(distanceTable->distances);
          }
          free(distanceTable);
        }
      }
      break;
    }
    case -1:
      break;
    default:
      setConsoleColor(COLOR_ERROR);
      printf("\"%c\" ist keine gültige Eingabe.\n", c);
    }
  } while (c != 'f');
  return 0;
}
