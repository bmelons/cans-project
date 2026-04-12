#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FOOD_NAME_LENGTH 64
#define DESERIALIZATION_BUFFER_LENGTH 1024
#define COLUMN_DIVIDER '\t'
#define LINE_DIVIDER '\n'

// cries myself to sleep because i chose a OOP project in C of all things - Apr 11, 2025 9:41 PM

// @ TAYLER, see if your code editor has a TODO thing that searches for all instances of TODO in a codebase and puts it in a list, so useful epic

// TODO: move all funcs into prototypes for codebase sake

enum UserStates
{
    MAIN_MENU,
    DB_ADD_MENU,
    QUANTITY_ADD_MENU,
    REMOVE_MENU,
    LOG_MENU,
    EXPORT // export to a slightly prettier version of the .cansdata format

};

enum Operations
{
    ADD_FOOD,
    REMOVE_FOOD,
    UPDATE_FOOD

};

typedef struct Food
{
    char name[MAX_FOOD_NAME_LENGTH];
    int count;
    struct Food *next;
} Food;

typedef struct HistoryItem
{
    enum Operations action;
    char note[32];

} HistoryItem;

#pragma region
#pragma endregion
// GLOBALS

Food *g_foodHead;
Food *g_foodTail;

Food *InitFood(char name[], int count)
{
    Food *food = malloc(sizeof(Food));
    strcpy(food->name, name);
    food->count = count;
    food->next = NULL;
    return food;
}

Food *InitEmptyFood()
{
    Food *food = malloc(sizeof(Food));
    strcpy(food->name, "");
    food->count = 0;
    food->next = NULL;
    return food;
}

void AppendFood(Food *food)
{
    if (g_foodHead == NULL)
    {
        g_foodHead = food;
        g_foodTail = food;
    }
    else
    {
        g_foodTail->next = food;
        g_foodTail = food;
    }
    return;
}

void IterateFoods(void (*callback)(Food *))
{ // mostly a recipe for later use, but feel free to use with PrintFood
    // under the assumption that following the trail of nexts from the head will always reach the tail
    Food *ptr = g_foodHead;
    while (ptr != NULL)
    {
        callback(ptr);
        ptr = ptr->next;
    };
}

void PrintFood(Food *food)
{
    printf("%s [%d]\n", food->name, food->count);
}

void data_SerializeFood()
{
    FILE *datafile;
    // char *datastring = malloc(2048*sizeof(char));

    datafile = fopen("./.cansdata", "w");
    if (datafile == NULL)
    {
        printf("No existing data, writing data... ./.cansdata\n");
        fopen("./.cansdata", "w");
    }

    Food *ptr = g_foodHead;
    while (ptr != NULL)
    {
        fprintf(datafile, "%s%c%d%c", ptr->name, COLUMN_DIVIDER, ptr->count, LINE_DIVIDER);
        ptr = ptr->next;
    };
}

void data_DeserializeFood()
{
    // go through each letter of the file's text
    // write each letter to strTarget
    // if the letter is the COLUMN_DIVIDER, switch strTarget to buffer
    // if the letter is the LINE_DIVIDER, do the following
    /*
        - convert buffer to a number
        - assign that number it to the current food's count
        - append the food
        - clear the buffer
        -
    */
    FILE *datafile;

    datafile = fopen(".cansdata", "r");
    if (datafile == NULL)
    {
        printf("No existing data, writing data... ./.cansdata\n");
        fopen("./.cansdata", "w");
        return data_DeserializeFood();
    }

    int c;
    char buffer[DESERIALIZATION_BUFFER_LENGTH] = "";
    Food *food = InitEmptyFood();
    int i = 0;
    char *strTarget = food->name;

    while ((c = fgetc(datafile)) != EOF)
    {
        if (c == COLUMN_DIVIDER)
        {
            strTarget = buffer;
            i = 0;
            continue;
        }
        if (c == LINE_DIVIDER)
        {
            food->count = strtod(buffer, NULL);
            strcpy(buffer, "");
            AppendFood(food);
            food = InitEmptyFood();
            strTarget = food->name;
            i = 0;
            continue;
        }
        *(strTarget + i++) = c;
        // printf("%c", cur);
    }
}

int Exit()
{
    // TODO: free all nodes/foods
    return 0;
}

void MenuTitle(char *header, char *subheader)
{
    // TODO: figure out if its possible to filter the end of strings without null terminators because the program will bork itself
    printf("\n[CANS] ");
    while (*header != '\0')
    {
        printf("%c", *(header++));
    }
    printf("\n");
    while (*subheader != '\0')
    {
        printf("%c", *(subheader++));
    }
    printf("\n\n");
}

void MainTree()
{
    MenuTitle("Menu", "View important or urgent info and choose operations");
    // TODO: Display 3 most recent operations
    // TODO: Display 0-3 highest and lowest stocked items
    // TODO: maybe? make above two configurable

    int command = NULL;
    while (1)
    {
        printf("Please enter a command\n");
        printf("[1] to UPDATE quantities of the stored foods");
        printf("[2] to ADD a food to the database\n");
        printf("[3] to REMOVE a food from the database\n");
        printf("[4] for session logs.");
        int wasSuccessful = scanf(" %d", &command);
        // Sanity checks/input sanitization
        if (!wasSuccessful )
        {
            printf("Invalid command");
            continue;
        }
        
    }
}

int main(void)
{
    printf("Welcome to C.A.N.S.\n");
    enum UserStates state = MAIN_MENU; //
    // AppendFood( InitFood("hello POOP",1) );
    // AppendFood( InitFood("penis sauce",1));

    // On load setup
    data_DeserializeFood();

    IterateFoods(PrintFood);
    MenuTitle("Menu", "View important or urgent info and choose operations");
    while (1)
    {
        break;
        switch (state)
        {
        default:
        case MAIN_MENU:
            MainTree();
            break;
        }
    }

    return Exit();
}