## Programmentwurf

### Problemstellung

Bei dem Travelling-Salesman-Problem sind mehrere Stadte und die Entfernungen zwischen den Stadten vorgegeben. Gesucht ist eine kürzeste Rundreise, bei der jede Stadt genau einmal besucht wird und sich der Handlungsreisende am Ende der Rundreise wieder in die Anfangsstadt befindet. In dem Programmentwurf soll ein C-Programm zur Losung dieses Problems implementiert werden.

### Implementierung

Das Programm zeigt nach dem Start zunächst ein Auswahlmenü mit folgenden Optionen an:

- (a) Entfernungstabelle laden
- (b) Entfernungstabelle speichern
- (c) Entfernungstabelle anzeigen
- (d) Entfernung zwischen zwei Städten ändern
- (e) Kürzeste Route berechnen
- (f) Programm beenden

Zu jedem der sechs Menüpunkte wird eine Funktion implementiert, dafür sollen die folgenden Strukturen verwendet werden:

```c
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
```

Die erste Struktur speichert die Enfernung zwischen zwei Städten, die Variablen `from` und `to` geben den Index der Start- und Zielstadt an und `dist` die Enferntung zwischen den beiden Städten.  
Die zweite Struktur speichert die Entfernungstabelle, dabei ist `n` die Anzahl der Städte, `cities` ein Pointer auf ein Feld mit den Städtenamen und `distances` ein Zeiger auf ein Feld mit den Entfernungen zwischen den Städten.

Die Funktionen zu den sechs Menüpunkten sowie die `main`-Funktion sollen wie folgt implementiert werden:

- (a) Die Funktion zum Laden fragt den Anwender nach dem Namen einer Textdatei, in der die Entfernungen gespeichert sind. Die Datei hat den folgenden Aufbau: in der ersten Zeile stehen die Namen der Stadte, jeweils getrennt durch mindestens ein Leerzeichen. Anschließend folgt für jede Stadt eine Zeile mit den Entfernungen, wieder durch Leerzeichen getrennt. Die Entfernung zu der Stadt in der Zeile muss dabei immer 0, die Entfernungen zu allen anderen Stadten immer positiv sein. Beispiel:

  Berlin Hamburg Muenchen Koeln Frankfurt  
  0 150 750 500 400  
  150 0 800 350 450  
  740 780 0 620 400  
  490 350 630 0 230  
  400 450 400 220 0

  Die Funktion liest die Datei ein, prüft, ob das vorgegebene Format eingehalten wurde und speichert die Daten in einer Struktur vom Typ DistanceTable. Der Rückgabewert ist ein Pointer auf die Struktur, bei einem Fehler wird ein NULL-Pointer zurückgegeben.

- (b) Die Funktion zum Speichern der Tabelle darf nur aufgerufen werden, wenn zuvor eine Entfernungstabelle geladen wurde. Sie erhält eine Struktur vom Typ DistanceTable, fragt den Anwender nach einem Dateinamen und speichert die Daten in dem oben angegebenen Format in der Datei.

- (c) Die Ausgabefunktion gibt die Entfernungen als zweidimensionale Tabelle auf der Konsole aus. Am linken und oberen Rand der Tabelle werden jeweils die Stadtenamen aufgelistet.

- (d) Die Funktion zur Änderung der Entfernung fragt den Anwender nach zwei Städtenamen. Falls die Namen gültig sind, werden die aktuellen Entfernungen zwischen den beiden Städten angezeigt und der Anwender aufgefordert zwei neue Werte einzugeben. Die Eingabe muss gepruft werden, d. h. beide Werte müssen positiv sein und bei einem Eingabefehler wiederholt werden.

- (e) Die Funktion zur Berechnung einer kürzesten Route fragt den Anwender nach der Startstadt und nach dem Berechnungsverfahren. Als Verfahren werden zwei Alternativen angeboten: ein exaktes Verfahren und ein heuristisches Verfahren, das in jedem Schritt immer die Stadt mit der kürzesten Entfernung besucht. Nach der Eingabe wird die Route berechnet und dann zusammen mit der Gesamtlänge auf der Konsole angezeigt.

- (f) Die Funktion zum Beenden des Programms soll zunächst prüfen, ob Änderungen an der Tabelle gemacht wurden, die noch nicht gespeichert wurden. In diesem Fall wird der Anwender gefragt, ob er das Programm ohne Speichern beenden oder wieder zum Menü zurückkehren möchte.

- (g) Die main-Funktion zeigt das Auswahlmenu an und verarbeitet die Benutzereingaben. Vor dem Beenden des Programms muss die main-Funktion alle angeforderten Ressourcen wieder freigeben.

### Bedingungen

- Eine Bearbeitung in Gruppen zu 3 Studierenden ist möglich. Alle Studierenden einer Gruppe erhalten die gleiche Bewertung.
- Das Programm muss unter Linux mit dem GCC-Compiler unter Verwendung der Optionen -Wall und -pedantic-errors fehlerfrei und ohne Warnungen kompilierbar sein.
- Die vorgegebenen Strukturen müssen wie oben beschrieben zur Implementierung eingesetzt werden. Es durfen nach Bedarf weitere Strukturen und Funktionen erganzt werden, globalen Variablen sind nicht erlaubt.
- Abgabe: Source-Code als c-File oder zip-File auf Moodle hochladen. Das File mit den Namen der Gruppenmitglieder benennen. Abgabetermin ist 14.03.2021.

### Bewertungskriterien

| Kriterium                           | Gewichtung |
| ----------------------------------- | ---------: |
| Dateifunktionen                     |  10 Punkte |
| Ausgabe- und Bearbeitungsfunktionen |  10 Punkte |
| Berechnung der Route                |  10 Punkte |
| Auswahlmenü und Programm beenden    |  10 Punkte |
| Kompilierbarkeit                    |   5 Punkte |
| Strukturierung und Kommentierung    |   5 Punkte |

## Set up C for Visual Studio Code (Windows)

### 1. [Download](https://code.visualstudio.com/) and install Visual Studio Code

### 2. [Download](https://mingw.osdn.io) and install MinGW ([Video](https://youtu.be/guM4XS43m4I?t=29))

You will need the following packages:

- mingw32-base-bin (A Basic MinGW Installation)
- mingw32-gcc-g++-bin (The GNU C++ Compiler)

### 3. Setup Path Variable ([Video](https://youtu.be/guM4XS43m4I?t=338))

Navigate to environment variables and add the path of the bin folder inside the installation folder of MinGW (by default it's C:\MinGW\bin) to the path variable.
You will need to restart Visual Studio Code before you can compile inside the internal terminal.

### 4. Install the following extensions for Visual Studio Code ([Video](https://youtu.be/77v-Poud_io?t=51))

- [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [Code Runner](https://marketplace.visualstudio.com/items?itemName=formulahendry.code-runner)

### 5. Adjust preferences for Visual Studio Code

File -> Preferences -> Settings

#### 5.1 Code Runner

Run code in integraded VS Code terminal ([Video](https://youtu.be/77v-Poud_io?t=349))

- Search for "Code runner: Run In Terminal" and make sure it's checked.

Enable all warnings relevant for this project

- Search for "Code runner: Executor Map" and change the command for c code to include the parameters -Wall -pedantic-errors:
  `"c": "cd $dir && gcc $fileName -Wall -pedantic-errors -o $fileNameWithoutExt && $dir$fileNameWithoutExt",`

#### 5.2 Adjust preferences for C/C++

Format according to style guide:

- Search for "C_Cpp: Clang_format_fallback Style" and set the following value: `{ BasedOnStyle: LLVM, ColumnLimit: 0 }`
