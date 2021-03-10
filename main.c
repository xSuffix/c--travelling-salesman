// Kurs INF20A
// Bearbeiter: Jan Fröhlich, Gabriel Nill, Fabian Weller

// TODO: Offene Fragen:
// - Soll die Datei, in die gespeichert und von der gelesen wird, immer mit .txt enden?

#include <ctype.h> // iscntrl() isspace() tolower() Funktionen für ASCII-Zeichen
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

typedef struct {
  int index;
  int length;
} Way;

void setConsoleColor(int color);

void freeDistanceTable(DistanceTable *distanceTable);

int intDigits(int n);

int strlen_utf8(char *s);

char *substr_utf8(const char *src, size_t min, size_t max, char filler);

char *scanFilePath();

int scanBoolean();

DistanceTable *loadData();

int saveData(DistanceTable *distanceTable, int *unsavedChanges);

void showData(DistanceTable *distanceTable);

int getCityNumber(DistanceTable *distanceTable, char city[100]);

Distance *getDistanceStructBetweenCities(DistanceTable *distanceTable, int from, int to);

bool checkForInvalidDistance(long num);

void changeDistanceBetweenCities(DistanceTable *distanceTable, int *unsavedChanges);

int addDistancesOfRoute(int *route, int stops, DistanceTable *distanceTable);

void printRoute(DistanceTable *distanceTable, int *route, int routeLength, int stops);

Way shortestWay(int stops, int *distances);

void swap(int *a, int *b);

void permutationsOf(int *route, int startIndex, int endIndex, int *collectionIndex, int **routeCollection);

int memberOfRoute(int *route, int cityIndex, int stops);

void calculateShortestRoute(DistanceTable *distanceTable, int start);

void guessShortestRoute(DistanceTable *distanceTable, int start);

void shortestRouteInit(DistanceTable *distanceTable);

int exitProgram(int *unsavedChanges);

void printMenu(Menu *menu, int length);

int main();

// Portable function for changing the console color
void setConsoleColor(int color) {
#ifdef _WIN32
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#else
  printf("\033[%dm", color);
#endif
}

void freeDistanceTable(DistanceTable *distanceTable) {
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
// Free substr after using this function!
char *substr_utf8(const char *src, size_t min, size_t max, char filler) {
  size_t i = 0, j = 0;
  while (j < max && src[i]) {
    if ((src[i] & 0xc0) != 0x80)
      j++;
    i++;
  }

  size_t strlen = j > min ? i : min + i - j;
  char *substr = calloc(strlen + 1, sizeof(char));
  memcpy(substr, &src[0], i);
  substr[strlen] = '\0';

  for (int k = 0; min > k + j; k++) {
    strncat(substr, &filler, 1);
  }
  return substr;
}

// Scan console for a file path and return it
// Free path after using this function!
char *scanFilePath() {
  setConsoleColor(COLOR_PRIMARY);
  char *path = calloc(PATH_MAX, sizeof(char));
  char fmt[64];
  snprintf(fmt, sizeof fmt, "%%%ds", PATH_MAX - 1);
  scanf(fmt, path);
  printf("\n");
  return path;
}

//Scan console for y/n input
int scanBoolean() {
  setConsoleColor(COLOR_PRIMARY);
  char c;
  while (1) {
    c = tolower(getchar());
    switch (c) {
    case 'y':
      return 1;
    case 'n':
      return 0;
    }
  }
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
    free(path);
    return NULL;
  }

  FILE *fpointer = fopen(path, "r");
  if (fpointer == NULL) {
    setConsoleColor(COLOR_ERROR);
    printf("Fehler: Die Datei konnte nicht geöffnet werden. (%s)\n", path);
    free(path);
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
            free(path);
            free(line);
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
      free(path);
      free(line);
      return NULL;
    }

    // Read distances between cities from second to (n+1)th line and store them in distanceTable->distances
    distanceTable->distances = calloc(distanceTable->n * distanceTable->n, sizeof(Distance));
    for (int from = 0; from < distanceTable->n; from++) {
      if (getline(&line, &len, fpointer) > 0) {
        char *distString = strtok(line, " \n");
        for (int to = 0; to < distanceTable->n; to++) {
          if (distString == NULL) {
            setConsoleColor(COLOR_ERROR);
            printf("Fehler beim Einlesen: In Zeile %u stehen nur %u Werte, es wurden aber %u Städte eingelesen.\n", from + 2, to, distanceTable->n);
            free(path);
            free(line);
            return NULL;
          }
          // Check if distance is valid
          int dist = strtol(distString, NULL, 0); // TODO A character will be interpreted as 0 - is that okay?
          if (from == to) {                       // Start city == destination (diagonal line in file)
            if (dist != 0) {
              setConsoleColor(COLOR_ERROR);
              printf("Fehler beim Einlesen: Die Entfernung einer Stadt zu sich selbst muss immer 0 sein, in der Datei steht jedoch \"%s\" (%u. Zeile, %u. Wort).\n", distString, from + 2, to + 1);
              free(path);
              free(line);
              return NULL;
            } else {
              Distance distance = {.from = from, .to = to, .dist = dist};
              distanceTable->distances[from * distanceTable->n + to] = distance;
            }
          } else { // Start city != destination
            if (dist > 0) {
              Distance distance = {.from = from, .to = to, .dist = dist};
              distanceTable->distances[from * distanceTable->n + to] = distance;
            } else {
              setConsoleColor(COLOR_ERROR);
              printf("Fehler beim Einlesen: Die Entfernung zwischen verschiedenen Städten muss größer als 0 sein, in der Datei steht jedoch \"%s\" (%u. Zeile, %u. Wort).\n", distString, from + 2, to + 1);
              free(path);
              free(line);
              return NULL;
            }
          }
          distString = strtok(NULL, "\n ");
        }
      } else {
        setConsoleColor(COLOR_ERROR);
        printf("Fehler beim Einlesen: Zeile %u ist leer. Stellen Sie sicher, dass es gleich viele Zeilen wie Städte gibt.\n", from);
        free(path);
        free(line);
        return NULL;
      }
    }

    fclose(fpointer);
    setConsoleColor(COLOR_SUCCESS);
    printf("Die Entfernungstabelle wurde erfolgreich geladen! (%s)\n", path);
    free(path);
    free(line);
    return distanceTable;
  }
}

int saveData(DistanceTable *distanceTable, int *unsavedChanges) {
  if (distanceTable) {
    setConsoleColor(COLOR_DEFAULT);
    printf("Unter welchem Namen soll die Datei gespeichert werden?\n");

    char *path = scanFilePath();
    if (path[0] <= 0) {
      setConsoleColor(COLOR_DEFAULT);
      free(path);
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
      *unsavedChanges = 0;
      setConsoleColor(COLOR_SUCCESS);
      printf("Die Entfernungstabelle wurde erfolgreich gespeichert! (%s)\n", path);
      free(path);
      return 1;
    }

    setConsoleColor(COLOR_ERROR);
    printf("Fehler: Die Datei konnte unter diesem Pfad nicht gespeichert werden. (%s)\n", path);
    free(path);
    return 0;
  }

  setConsoleColor(COLOR_ERROR);
  printf("Fehler: Bitte laden Sie zunächst eine Entfernungstabelle.\n");
  return 0;
}

void showData(DistanceTable *distanceTable) {
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
      char *colTitle = substr_utf8(distanceTable->cities[i], columnLengths[i], columnLengths[i], ' ');
      printf("%s ", colTitle);
      free(colTitle);
    }
    printf("\n");

    // Print rows except for title row
    for (int i = 0; i < distanceTable->n; i++) {
      setConsoleColor(COLOR_INFO);
      // Print titles of rows with margin, so that the entire row has the same width
      char *rowTitle = substr_utf8(distanceTable->cities[i], largestCityNameLength, largestCityNameLength, ' ');
      printf("%s ", rowTitle);
      free(rowTitle);
      setConsoleColor(COLOR_DEFAULT);
      for (int j = 0; j < distanceTable->n; j++) {
        // Print distances (numbers)
        printf("%*d ", columnLengths[j], distanceTable->distances[i * distanceTable->n + j].dist);
      }
      printf("\n");
    }
    free(columnLengths);
  } else {
    setConsoleColor(COLOR_WARNING);
    printf("Die Entfernungstabelle ist leer.\n");
  }
}

/**
 * @brief Get the index of the city in the distanceTable (if it's in the distanceTable)
 * 
 * @param distanceTable 
 * @param city
 * @return int  Returns the index of the city in the distanceTable. Returns -1 if city was not found.
 */
int getCityNumber(DistanceTable *distanceTable, char city[100]) {
  for (int i = 0; i < distanceTable->n; i++) {
    if (strcmp(distanceTable->cities[i], city) == 0) {
      return i;
    }
  }
  return -1; // City was not found
}

/**
 * @brief Get the Pointer to the Distance Struct Between "from" and "to"
 * 
 * @param distanceTable 
 * @param from          The index of the city to go from
 * @param to            The index of the city to go to
 * @return Distance* 
 */
Distance *getDistanceStructBetweenCities(DistanceTable *distanceTable, int from, int to) {
  for (int i = 0; i < distanceTable->n * distanceTable->n - 1; i++) {
    if (from == distanceTable->distances[i].from && to == distanceTable->distances[i].to)
      return &distanceTable->distances[i];
  }

  return 0;
}

/**
 * 
 * @brief This checks, if the entered distance is positive and fits into an Integer. 
 *        So the Long can be safely typecasted to an Int.
 * 
 * @param num     Distance
 * @return true   The provided distance is invalid.
 * @return false  The provided distance is valid.
 */
bool checkForInvalidDistance(long num) {
  if (num <= 0) {
    setConsoleColor(COLOR_ERROR);
    printf("Bitte geben Sie eine Zahl ein, die größer als 0 ist.\n\n");
    return true;
  } else if (num > INT_MAX) { // this doesn't work on windows, because the size of int and long is identical
    setConsoleColor(COLOR_ERROR);
    printf("Die Entfernung ist zu groß.\n\n");
    return true;
  }
  return false;
}

// TODO: Handling for double values? Currently the decimal places are simply cut off.
/**
 * @brief This function changes the distance between to cities. 
 *        The new distances are provided from the user.
 * 
 * @param distanceTable 
 * @param unsavedChanges increments if a distance is changed
 */
void changeDistanceBetweenCities(DistanceTable *distanceTable, int *unsavedChanges) {
  if (distanceTable == NULL) {
    setConsoleColor(COLOR_ERROR);
    printf("Bitte laden Sie zuerst eine Entfernungstabelle.\n");
    return;
  }

  bool invalid;
  char firstCity[100];
  char secondCity[100];
  int firstCityNumber;
  int secondCityNumber;

  // Get the city names from the user and check them.
  do {
    do {
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

    do {
      invalid = false;
      setConsoleColor(COLOR_DEFAULT);
      printf("Bitte geben Sie den Namen der zweiten Stadt ein:\n");
      setConsoleColor(COLOR_PRIMARY);
      scanf("%s", secondCity);

      secondCityNumber = getCityNumber(distanceTable, secondCity);
      if (secondCityNumber == -1) {
        setConsoleColor(COLOR_ERROR);
        printf("Diese Stadt konnte in der Entfernungstabelle nicht gefunden werden.\n");
        printf("Bitte versuchen Sie es erneut.\n\n");
        invalid = true;
      }
    } while (invalid);

    if (firstCityNumber == secondCityNumber) {
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
  long newDistance;
  char input[15];

  // Get the new distances from the user and check them.
  do {
    invalid = false;
    setConsoleColor(COLOR_DEFAULT);
    printf("Bitte geben Sie eine neue Entfernung für die Strecke von %s nach %s ein:\n", firstCity, secondCity);
    setConsoleColor(COLOR_PRIMARY);
    scanf("%14s", input);
    newDistance = strtol(input, NULL, 10);

    invalid = checkForInvalidDistance(newDistance);

    int c;
    while ((c = fgetc(stdin)) != '\n' && c != EOF)
      ; // Flush stdin
  } while (invalid);
  firstDist = (int)newDistance;

  printf("\n");

  do {
    invalid = false;
    setConsoleColor(COLOR_DEFAULT);
    printf("Bitte geben Sie eine neue Entfernung für die Strecke von %s nach %s ein:\n", secondCity, firstCity);
    setConsoleColor(COLOR_PRIMARY);
    scanf("%14s", input);
    newDistance = strtol(input, NULL, 10);

    invalid = checkForInvalidDistance(newDistance);

    int c;
    while ((c = fgetc(stdin)) != '\n' && c != EOF)
      ; // Flush stdin
  } while (invalid);
  secondDist = (int)newDistance;

  // Replace distances
  if (firstToSecond->dist != firstDist) {
    firstToSecond->dist = firstDist;
    *unsavedChanges += 1;
  }

  if (secondToFirst->dist != secondDist) {
    secondToFirst->dist = secondDist;
    *unsavedChanges += 1;
  }

  setConsoleColor(COLOR_SUCCESS);
  printf("\n");
  printf("Die Entfernung wurde erfolgreich geändert!\n");
}

int addDistancesOfRoute(int *route, int stops, DistanceTable *distanceTable) {
  /*
    Sums up the distances between all stops of a route
    --------------------------------------------------
   # Returns a route's length
   # Used by both functions to find a shortest way
   
  */

  int sum = 0;
  if (stops > 1) {
    for (int i = 0; i < stops - 1; i++) {
      sum += getDistanceStructBetweenCities(distanceTable, *(route + i), *(route + i + 1))->dist;
    }
  }
  return sum;
}

void printRoute(DistanceTable *distanceTable, int *route, int routeLength, int stops) {
  /*
    Outputs the shortest route and its length
    -----------------------------------------
   # Used by both functions to find a shortest way

  */

  setConsoleColor(COLOR_INFO);
  printf("Route: ");
  setConsoleColor(COLOR_DEFAULT);
  for (int j = 0; j < stops - 1; j++) {
    printf("%s - ", distanceTable->cities[route[j]]);
  }
  printf("%s\n", distanceTable->cities[route[stops - 1]]);
  setConsoleColor(COLOR_INFO);
  printf("Länge: ");
  setConsoleColor(COLOR_DEFAULT);
  printf("%dkm\n\n", routeLength);
  printf("Press any key to continue...");
  getchar();
  getchar();
  printf("\r");
}

Way shortestWay(int stops, int *distances) {
  /*
    Returns Way struct containing the shortest route and its length
    ---------------------------------------------------------------
   # Used by calculateShortestRoute(...)

  */

  Way min = {0, distances[0]};
  for (int i = 0; i < stops; i++) {
    if (distances[i] < min.length) {
      min.index = i;
      min.length = distances[i];
    }
  }
  return min;
}

void swap(int *a, int *b) {
  /*
    Swap to Integer fields' values
    ------------------------------
   # Used by permutationsOf(...) to create permutations

  */
  int temp;
  temp = *a;
  *a = *b;
  *b = temp;
}

void permutationsOf(int *route, int startIndex, int endIndex, int *collectionIndex, int **routeCollection) {
  /*
    Creates all permutations of a given route
    -----------------------------------------
   # Recursive algorithm to create all permutation of a given part of a given array
   # Recursive approach is fixing a first member and switch the others behind and rotate through the array
   # Created arrays are directly saved to the passed routeCollection [collection from calculateShortestRoute]
     at pointed at position *collectionIndex which is incremented when a new route is saved.

  */

  if (startIndex == endIndex) {
    for (int j = 0; j < endIndex + 2; j++) {
      routeCollection[*collectionIndex][j] = route[j];
    }
    *collectionIndex += 1;
    return;
  }
  int i;
  for (i = startIndex; i <= endIndex; i++) {
    swap((route + i), (route + startIndex));
    permutationsOf(route, startIndex + 1, endIndex, collectionIndex, routeCollection);
    swap((route + i), (route + startIndex));
  }
}

int memberOfRoute(int *route, int cityIndex, int stops) {
  /*
    Checks whether cityIndex is in *route
    -------------------------------------
   # Used by guessShortestRoute (Nearest-Neighbor-Estimation)
   # As described there, the array members' values are increased by 1 to prevent interactions with initialization zeros
   # Hence the cityIndex needs to be increased by 1 for proper results
   # returns 1 (true) when cityIndex is NOT in route

  */

  for (int i = 0; i < stops; i++) {
    if (route[i] == cityIndex + 1) {
      return 0;
    }
  }
  return 1;
}

void calculateShortestRoute(DistanceTable *distanceTable, int start) {
  /*
    Method: Exact Calculation
    -------------------------
   # Calculates all routes, then chooses the shortest one
   # Initialization of 2-dimensional array to collect all routes
   # Initialization of first route then fill collection with all permutations of that route
     - 1st route of structure: (chosen city index)-{all route indices w/o chosen one in ascending order}-(chosen city index)
     - Permutations are all combinations of the inner part, the first and last city (chosen city) remain unchanged
   # Calculate all routes distances and save them to *allRouteLengths
   # Determine shortest route and print it out

  */

  int possibleRoutesCount = 1;
  for (int i = 1; i < distanceTable->n; i++) {
    possibleRoutesCount *= i;
  }

  int **allRoutes = malloc(possibleRoutesCount * sizeof(int *)), *allRouteLengths = malloc(possibleRoutesCount * sizeof(int));
  for (int i = 0; i < possibleRoutesCount; i++) {
    allRoutes[i] = malloc(((distanceTable->n) + 1) * sizeof(int));
  }

  allRoutes[0][0] = start, allRoutes[0][distanceTable->n] = start;

  for (int arrayIndex = 1, city = 0; arrayIndex < distanceTable->n; arrayIndex++, city++) {
    if (city != start) {
      allRoutes[0][arrayIndex] = city;
    } else
      allRoutes[0][arrayIndex] = ++city;
  }

  int permutationCollectionIndex = 0;
  permutationsOf(allRoutes[0], 1, distanceTable->n - 1, &permutationCollectionIndex, allRoutes);

  for (int i = 0; i < possibleRoutesCount; i++) {
    allRouteLengths[i] = addDistancesOfRoute(allRoutes[i], distanceTable->n + 1, distanceTable);
  }

  Way shortest = shortestWay(possibleRoutesCount, allRouteLengths);

  printRoute(distanceTable, allRoutes[shortest.index], shortest.length, distanceTable->n + 1);
}

void guessShortestRoute(DistanceTable *distanceTable, int start) {
  /*
    Method: Nearest Neighbor
    ------------------------
   # Nearest neighbor that is neither the same city nor has been previously visited is picked
   # City Indexes are put in the array with an increment of 1 to avoid undefined interactions
     - int array initialized with 0
     - seeking for index 0 in route-array might return falsely positive
     -> solution is incrementing all inserted numbers, so the seek is for numbers x >=1
     -> no undefined interactions
   # After route completion: decrement of city indexes and calculation of sum of distances
   # Printing of route and distance
     
  */

  int *route = malloc((distanceTable->n + 1) * sizeof(int));
  route[0] = start + 1;
  route[distanceTable->n] = start + 1;

  for (int i = 1; i < distanceTable->n; i++) {
    int distance = 0, index = 0;
    for (int j = 0; j < distanceTable->n; j++) {
      if ((distance == 0) && ((route[i - 1] - 1) != j) && (memberOfRoute(route, j, distanceTable->n)) && (getDistanceStructBetweenCities(distanceTable, route[i - 1] - 1, j)->dist > 0)) {
        distance = getDistanceStructBetweenCities(distanceTable, route[i - 1] - 1, j)->dist;
        index = j;
      } else if (((route[i - 1] - 1) != j) && (memberOfRoute(route, j, distanceTable->n)) && (getDistanceStructBetweenCities(distanceTable, route[i - 1] - 1, j)->dist < distance) && (getDistanceStructBetweenCities(distanceTable, route[i - 1] - 1, j)->dist > 0)) {
        distance = getDistanceStructBetweenCities(distanceTable, route[i - 1] - 1, j)->dist;
        index = j;
      }
    }
    route[i] = index + 1;
  }
  for (int x = 0; x < distanceTable->n + 1; x++) {
    route[x] -= 1;
  }

  int length = addDistancesOfRoute(route, distanceTable->n + 1, distanceTable);

  printRoute(distanceTable, route, length, distanceTable->n + 1);
}

void shortestRouteInit(DistanceTable *distanceTable) {
  /*
    Initialization for Route Calculation
    ------------------------------------
   # Error Handling
     - No or empty DistanceTable
     - Just one city
   # Check for calculation method (nearest-neighbor-estimation / exact calculation)
   # Take user input for starting city
     - should only matter for nearest-neighbor-calculation
   # Calculates distance and route in the requested way, passes starting city

  */

  if (distanceTable->n == 0) {
    setConsoleColor(COLOR_ERROR);
    printf("Bitte laden Sie zuerst eine Entfernungstabelle.\n");
    return;
  }
  if (distanceTable->n == 1) {
    setConsoleColor(COLOR_WARNING);
    printf("Die Entfernungstabelle enthält nur eine Stadt, es ist keine Rundreise möglich.\n");
    printf("Laden Sie eine neue Entfernungstabelle, um eine Route zu berechnen.\n");
    return;
  }

  //Auswahl: heuristischer oder exakter Ansatz
  printf("Genaue Berechnung? (Y/n)\n");
  int yesno = scanBoolean();

  //Festlegung Startpunkt->startCityNumber: startstadt
  bool invalid;
  char startCity[100];
  int startCityNumber;

  do {
    invalid = false;
    setConsoleColor(COLOR_DEFAULT);
    printf("Bitte geben Sie den Namen der Start-Stadt ein:\n");
    setConsoleColor(COLOR_PRIMARY);
    scanf("%s", startCity);

    startCityNumber = getCityNumber(distanceTable, startCity);
    if (startCityNumber == -1) {
      setConsoleColor(COLOR_ERROR);
      printf("Diese Stadt konnte in der Entfernungstabelle nicht gefunden werden.\n");
      printf("Bitte versuchen Sie es erneut.\n\n");
      invalid = true;
    }
  } while (invalid);

  printf("\n");

  //ausgewählter Berechnungsansatz
  if (yesno != 0)
    calculateShortestRoute(distanceTable, startCityNumber);
  else
    guessShortestRoute(distanceTable, startCityNumber);
}

int exitProgram(int *unsavedChanges) {
  if (*unsavedChanges > 0) {
    setConsoleColor(COLOR_WARNING);
    printf("An der Entfernungstabelle wurden %d ungesicherte Änderungen vorgenommen. Soll das Programm dennoch beendet werden? (y/n)\n", *unsavedChanges);
    if (!scanBoolean()) {
      return 0;
    }
    printf("\n");
  }
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

  int unsavedChanges = 0;
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

  // Build menu
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
      if (tmpDistanceTable) {
        freeDistanceTable(distanceTable);
        distanceTable = tmpDistanceTable;
      }
      break;
    case 'b':
      saveData(distanceTable, &unsavedChanges);
      break;
    case 'c':
      showData(distanceTable);
      break;
    case 'd':
      changeDistanceBetweenCities(distanceTable, &unsavedChanges);
      break;
    case 'e':
      //calculateShortestRoute(distanceTable);
      shortestRouteInit(distanceTable);
      break;
    case 'f': {
      if (exitProgram(&unsavedChanges)) {
        freeDistanceTable(distanceTable);
        return 0;
      }
      break;
    }
    case -1:
      break;
    default:
      setConsoleColor(COLOR_ERROR);
      printf("\"%c\" ist keine gültige Eingabe.\n", c);
    }
  } while (1);
}
