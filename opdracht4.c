/*
* Naam : Krulknul
* Studie : BSc Informatica
*
* Opdracht4.c:
* - Dit is de geweldige DNA Matcher v0.2!
* - Deze door Dr. Quackerjack (en zijn assistent!) uitgevonden CLI applicatie
*   maakt gebruik van een database om DNA-sequenties te vergelijken.
*
* - Features:
*     - Maakt gebruik van een database om strings in op te slaan
*     - Vergelijkt strings met behulp van het Levenshtein algoritme
*     - Kan strings uitlezen uit tekstbestanden
*     - Maakt het simpel om strings toe te voegen en te verwijderen
*/

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_DB_SIZE 100
#define MAX_INPUT_SIZE 100
char **DATABASE;
int DB_SIZE = 0;

// Berekent de kleinste van twee waarden.
//
// In: 2 integers "a" en "b".
//
// Uit: De grootste waarde van de twee invoerwaarden.
int min(int a, int b) {
    if (a > b || a == b) {
        return b;
    } else {
        return a;
    }
}

// Print een lijst van commandos en hun functionaliteit.
//
// In: GEEN INVOER.
//
// Side effects: Print de lijst naar stdout.
//
// Uit: GEEN UITVOER.
void help(void) {
    printf("LIST OF COMMANDS\n");
    printf("  add ...          add to database\n");
    printf("  compare ... ...  compare two strings\n");
    printf("  help             print this text\n");
    printf("  list             print database\n");
    printf("  read ...         read from file and add to database\n");
    printf("  remove ...       remove from database\n");
    printf("  retrieve ...     find in database\n");
    printf("  size             print number of items in database\n");
    printf("  quit             stop\n");
}

// Alloceert geheugen voor een grid en
// past het Levenshtein algoritme toe op 2 strings.
//
// In: 2 te vergelijken char pointers "s1" en "s2".
//
// Uit: De int ** "grid". Dit is de resulterende Levenshtein grid.
int **levenshtein(char *s1, char *s2) {
    int length1 = (int) strlen(s1);
    int length2 = (int) strlen(s2);

    // Alloceer geheugen voor de pointers die naar de rijen wijzen.
    int **grid = calloc((size_t)(length1 + 1),  sizeof(int *));
    // Alloceer geheugen voor de integers in die pointers.
    for (int i = 0; i <= length1; i++) {
        grid[i] = calloc((size_t)(length2 + 1),  sizeof(int));
    }

    // Maak van rij 0: 1, 2, 3, 4, 5, etc...
    for (int i = 1; i <= length1; i++) {
        grid[i][0] = i;
    }
    // Maak van kolom 0: 1, 2, 3, 4, 5, etc...
    for (int i = 1; i <= length2; i++) {
        grid[0][i] = i;
    }

    // Uitvoering van het Levenshtein algoritme.
    int extra;
    for (int i = 1; i <= length1; i++) {
        for (int j = 1; j <= length2; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                extra = 0;
            } else {
                extra = 1;
            }
            int minimum = min(grid[i - 1][j] + 1, grid[i][j - 1] +1);
            grid[i][j] = min(minimum, grid[i - 1][j - 1] + extra);
        }
    }
    return grid;
}

// Maakt het geheugen vrij dat gealloceerd is voor
// de grid van het Levenshtein algoritme.
//
// In:
// - De int ** "grid". (de gebruikte Levenshtein grid.)
// - de int "rows". (Het aantal rijen in de Levenshtein grid.)
//
// Side effects: Het geheugen van de grid komt weer vrij.
//
// Uit: GEEN UITVOERWAARDE.
void clean_levenshtein(int **grid, int rows) {
    for (int i = 0; i < rows; i++) {
        free(grid[i]);
    }
    free(grid);
}

// Print een Levenshtein grid van 2 strings en print de Levenshteinafstand.
//
// In: 2 te vergelijken char pointers "s1" en "s2".
//
// Side effects: Print de grid en afstand naar stdout.
//
// Uit: GEEN UITVOER.
void compare(char *s1, char *s2) {
    int length1 = (int) strlen(s1);
    int length2 = (int) strlen(s2);
    int **grid = levenshtein(s1, s2);

    // Print alle getallen in een grid.
    for (int rows = 0; rows <= length1; rows++) {
        for (int cs = 0; cs <= length2; cs++) {
            printf("%i ", grid[rows][cs]);
        }
        printf("\n");
    }
    // Print de Levenshtein afstand, Dit is het getal helemaal rechtsonder.
    printf("Difference = %d\n", grid[length1][length2]);
    // Maak het geheugen vrij. Het is niet meer nodig.
    clean_levenshtein(grid, length1);
}

// Zoekt een string in de database.
// Wanneer er geen perfecte match is, worden de 3 strings
// afgedrukt die het meest lijken op de gezochte string.
//
// In: 1 char * "str". Dit is de gezochte string.
//
// Uit: GEEN UITVOER.
void retrieve(char *str) {
    // Foutmelding als de database leeg is.
    if (DB_SIZE == 0) {
        printf("No match found; database is empty\n");
        return;
    }

    // Maak een nieuwe array en zet hierin voor iedere string van de database
    // wat het verschil is met de gegeven string.
    int diff[DB_SIZE];
    for (int i = 0; i < DB_SIZE; i++) {
        // Als de gegeven string wordt gevonden in de database,
        // geef dit aan.
        if (strcmp(str, DATABASE[i]) == 0) {
            printf("Perfect match found; \"%s\" is in database\n", str);
            return;
        }
        // Roept levenshtein() aan op iedere string en maakt de grid weer vrij.
        int **grid = levenshtein(str, DATABASE[i]);
        diff[i] = grid[strlen(str)][strlen(DATABASE[i])];
        clean_levenshtein(grid, (int) strlen(str));
    }

    // Maakt een kopie van de database en kopieert alle strings erin.
    char *DB_copy[DB_SIZE];
    for (int i = 0; i < DB_SIZE; i++) {
        DB_copy[i] = DATABASE[i];
    }

    // Implementatie van gnome sort om de strings in DB_copy op afstand
    // van klein naar groot te sorteren.
    for (int i = 0; i < DB_SIZE; i++) {
        for (int j = i; j > 0; j--) {
            if (diff[j] < diff[j - 1]) {
                // Sorteert de verschilwaarden.
                int temp = diff[j - 1];
                diff[j - 1] = diff[j];
                diff[j] = temp;

                // Sorteert de strings.
                char *temp_ptr = DB_copy[j - 1];
                DB_copy[j - 1] = DB_copy[j];
                DB_copy[j] = temp_ptr;
            }
        }
    }

    // Print de 3 strings met de kleinste afstand tot de invoerstring.
    printf("No perfect match found; \"%s\" is not in database\n", str);
    printf("Best matches:\nDistance\tString\n");
    for (int i = 0; i < min(3, DB_SIZE); i++) {
        printf("%i\t\t%s\n", diff[i], DB_copy[i]);
    }
}

// Voegt een string toe aan de database.
//
// In: 1 char * "str". Deze string wordt toegevoegd.
//
// Side effects:
// - Er wordt een string toegevoegd aan de database.
// - Er wordt geheugen gealloceerd voor de string.
// - De DB_SIZE wordt met 1 verhoogd.
//
// Uit: GEEN UITVOER.
void add(char *str) {
    // Als de database vol is, wordt dit aangegeven.
    if (DB_SIZE == MAX_DB_SIZE) {
        printf("\"%s\" not added; database is full\n", str);
        return;
    }
    // Als de string al in de database zit, wordt dit aangegeven.
    for (int i = 0; i < DB_SIZE; i++) {
        if (strcmp(DATABASE[i], str) == 0) {
            printf("\"%s\" not added; already in database\n", str);
            return;
        }
    }

    // Geheugen wordt gealloceerd voor de string
    // en de string wordt in de database gezet.
    DATABASE[DB_SIZE] = malloc(strlen(str) + 1);
    strcpy(DATABASE[DB_SIZE], str);
    DB_SIZE++;
    printf("\"%s\" added to database\n", str);
}

// Verwijdert een string uit de database.
//
// In: 1 char * "str". Deze string wordt verwijderd.
//
// Side effects:
// - alle strings na de verwijderde string schuiven 1 naar links.
//   in de database.
// - Het geheugen van de string wordt vrijgegeven.
// - De DB_SIZE wordt met 1 verlaagd.
//
// Uit: GEEN UITVOER.
void delete(char *str) {
    int location = 0;

    // Loopt door de database heen en zoekt de gegeven string.
    for (int i = 0; i < DB_SIZE; i++) {
        // Als de gegeven string op deze index staat, verwijderen we die.
        if (strcmp(str, DATABASE[i]) == 0) {
            location = i;
            free(DATABASE[location]);
            // Schuif iedere entry in de database 1 plek op.
            for (int j = location; j < MAX_DB_SIZE - 1; j++) {
                DATABASE[j] = DATABASE[j + 1];
            }
            DB_SIZE--;
            printf("\"%s\" removed from database\n", str);
            return;
        }
    }
    printf("\"%s\" not removed; not in database\n", str);
}

// Leest 1 of meerdere strings uit een .txt bestand.
// Deze strings worden toegevoegd aan de database.
//
// In: 1 char * "filename". Dit is de naam van het tekstbestand.
//
// Side effects:
// - Er worden 1 of meerdere strings toegevoegd aan de database.
// - Er wordt geheugen gealloceerd voor de strings.
// - De DB_SIZE wordt verhoogd.
//
// Uit: GEEN UITVOER.
void readfile(char *filename) {
    // Als er geen filename wordt meegegeven, geven we een foutmelding.
    if (filename == NULL) {
        printf("Please enter a valid comand\n");
        return;
    }
    // Als het bestand niet bestaat, geven we een foutmelding.
    if (access(filename, F_OK) == -1) {
        printf("\"%s\" not found\n", filename);
        return;
    }
    // Open een stream voor het bestand.
    FILE *file = fopen(filename, "r");
    // Maakt een grote buffer om de strings in te stoppen.
    // Er wordt gecheckt of de strings niet te groot zijn, en de strings
    // worden toegevoegd aan de database.
    char buffer[1000];
    for (int i = 1; fscanf(file, "%s", buffer) != EOF; i++) {
        if (strlen(buffer) > MAX_INPUT_SIZE) {
            printf("Line error; input of line %d was longer than maximum allowed input\n", i);
            return;
        }
        add(buffer);
    }
    // De stream wordt gesloten.
    fclose(file);
}

// Print een lijst met alle strings in de database.
//
// In: GEEN INVOER.
//
// Side effects: Print de lijst naar stdout.
//
// Uit: GEEN UITVOER.
void list(void) {
    // Als de grootte van de database 0 is, geven we dat aan.
    if (DB_SIZE == 0) {
        printf("Database is empty\n");
    } else {
        // Zo niet, tellen we de strings in de database.
        for (int i = 0; i < DB_SIZE; i++) {
            printf("%s\n", DATABASE[i]);
        }
    }
}

// Print de hoeveelheid strings aanwezig in de database.
//
// In: GEEN INVOER.
//
// Side effects: Print de grootte naar stdout.
//
// Uit: GEEN UITVOER.
void size(void) {
    printf("%d items in database", DB_SIZE);
}

// Stript de input van de gebruiker door onjuiste tekens te verwijderen.
//
// In: 1 char * "str". Dit is de directe input van fgets in de main functie.
//
// Uit: 1 char * "str". Dit is het resultaat van de gestripte input.
char *strip(char *str) {
    // Als de lengte van de input groter is dan de maximale input,
    // wordt er een foutmelding gegeven.
    if (strlen(str) > MAX_INPUT_SIZE) {
        printf("Please enter a valid comand\n");
        return (char *) 1;
    }
    // Ieder teken wordt lowercase gemaakt.
    // Als het teken geen letter is dan verwijderen we het uit de string.
    for (int i = 0; i < (int) strlen(str); i++) {
        str[i] = (char) tolower(str[i]);
        // Tekens moeten alfabetisch zijn. spaties en punten zijn toegestaan.
        if (isalpha(str[i]) == 0 && str[i] != ' ' && str[i] != '.') {
            for (int j = i; j < (int) strlen(str); j++) {
                str[j] = str[j + 1];
            }
        }
    }
    return str;
}

// Maakt het geheugen vrij van de database.
//
// In: 1 char ** "DATABASE".
//
// Uit: GEEN UITVOER.
void cleanup_db(char **DATABASE) {
    for (int i = 0; i < DB_SIZE; i++) {
        free(DATABASE[i]);
    }
    free(DATABASE);
}

// Detecteert welk commando de gebruiker in heeft gevoerd en roept
// de juiste functies aan.
//
// In:
// - char * "command". (Dit is het commando dat de gebruiker opgeeft).
// - char * "s1". (Dit is argument 1 die de gebruiker meegeeft.)
// - char * "s2". (Dit is argument 2 die de gebruiker meegeeft.)
//
// Side effects: Roept verschillende functies aan.
//
// Uit:
// - int 0 wanneer alles juist is verlopen.
// - int 1 wanneer er iets fout is gegaan.
int cmd(char *command, char *s1, char *s2) {
    // Als er geen command wordt meegegeven, geven we een foutmelding.
    if (command == NULL) {
        printf("please enter a valid comand!\n");
        return 1;

    } else if (strcmp(command, "help") == 0) {
        help();

    } else if (strcmp(command, "list") == 0) {
        list();

    } else if (strcmp(command, "compare") == 0) {
        // Als 1 van beide argumenten ontbreekt, geven we een foutmelding.
        if (s1 == NULL || s2 == NULL) {
            printf("Please enter a valid comand\n");
            return 1;
        }
        compare(s1, s2);

    } else if (strcmp(command, "size") == 0) {
        printf("%i items in database\n", DB_SIZE);

    } else if (strcmp(command, "read") == 0) {
        readfile(s1);

    } else if (strcmp(command, "add") == 0) {
        // Als er een tweede argument opgegeven wordt, geven we een foutmelding.
        if (s1 == NULL || s2 != NULL) {
            printf("Please enter a valid comand\n");
            return 1;
        }
        add(s1);

    } else if (strcmp(command, "remove") == 0) {
        // Als er geen argument opgegeven wordt, geven we een foutmelding.
        if (s1 == NULL) {
            printf("Please enter a valid comand\n");
            return 1;
        }
        delete(s1);

    } else if (strcmp(command, "retrieve") == 0) {
        retrieve(s1);

    } else if (strcmp(command, "quit") == 0) {
        cleanup_db(DATABASE);
        exit(0);

    } else {
        // Als er geen commando opgegeven wordt, geven we een foutmelding.
        printf("Please enter a valid comand\n");

    }
    return 0;
}

// Mainfunctie
//
// Geheugen wordt gealloceerd voor de database.
// Daarna gaan we een loop in die oneindig
// op nieuwe commandos wacht tot de gebruiker
// het programma stopt.
int main(void) {
    printf("Welcome to DNA Matcher v0.2\n");
    // Allocatie van geheugen.
    DATABASE = malloc(MAX_DB_SIZE * sizeof(char *));

    // Deze loop blijft uitvoeren totdat het programma sluit.
    while (1) {
        printf("console> ");
        char input[MAX_INPUT_SIZE];

        // Als fgets faalt om een input te krijgen, geven we een foutmelding.
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            printf("Error: failed to get input\n");
        } else {
            // De input wordt gestript, en we gebruiken
            // strtok om de input op te delen in commando en argumenten.
            strip(input);
            char *command = strtok(input, " \n");
            char *s1 = strtok(NULL, " \n");
            char *s2 = strtok(NULL, "\n");
            // Aanroep van de juiste functie.
            cmd(command, s1, s2);
        }
    }
}
