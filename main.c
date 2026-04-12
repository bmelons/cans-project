#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FOOD_NAME_LENGTH 64
#define LOADING_BUFFER_LENGTH 128
#define COLUMN_DIVIDER '\t'
#define LINE_DIVIDER '\n'
#define TITLE_DIVIDER ">-----"

// cries myself to sleep because i chose a OOP project in C of all things - Apr 11, 2025 9:41 PM

// @ TAYLER, see if your code editor has a TODO thing that searches for all instances of TODO in a codebase and puts it in a list, so useful epic

// TODO: move all funcs into prototypes for codebase sake

enum UserStates
{
    MAIN_MENU,
    UPDATE_MENU,
    ADD_MENU,
    REMOVE_MENU,
    LIST_MENU,
    LOG_MENU,
    EXIT_STATE

};

enum Operations
{
    UPDATED_FOOD,
    ADDED_FOOD,
    REMOVED_FOOD,

};

typedef struct Food
{
    char name[MAX_FOOD_NAME_LENGTH];
    int count;
    struct Food *next;
} Food;

typedef struct HistoryItem
{
    char note[32];
    struct HistoryItem *next;

} HistoryItem;

// GLOBALS
enum UserStates g_state = MAIN_MENU;
Food *g_foodHead;
Food *g_foodTail;
HistoryItem *g_logHead;
HistoryItem *g_logTail;

// PROTOTYPES
// Food List Management
Food *InitFood(char name[], int count);
void AppendFood(Food *food);
void IterateFoods(void (*callback)(Food *));
int TotalStock();
void PrintFood(Food *food);
// History Management
HistoryItem *InitHistoryItem(char note[]);
void AppendHistoryItem(HistoryItem *item);
// Data Management
void data_RewriteLogs();
void data_LoadLogs();
void data_SerializeFood();
void data_DeserializeFood();
// Program States and Utility
void MainTree();
void AddFoodMenu();
void RemoveFoodMenu();
void UpdateFoodMenu();
void MenuTitle(char *header, char *subheader);
void LineBreakNTimes(int n);
int Exit();

int main(void)
{
    printf("Welcome to C.A.N.S.\n");

    // AppendFood( InitFood("hello POOP",1) );
    // AppendFood( InitFood("penis sauce",1));

    // On load setup
    data_DeserializeFood();

    IterateFoods(PrintFood);
    // MenuTitle("Menu", "View important or urgent info and choose operations");
    while (1)
    {
        // break;
        switch (g_state)
        {
        default:
        case MAIN_MENU:
            MainTree();
        }
        LineBreakNTimes(1);
    }

    return Exit();
}

// Food Linkedlist Operations
Food *InitFood(char name[], int count)
{
    Food *food = malloc(sizeof(Food));
    strcpy(food->name, name);
    food->count = count;
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

int TotalStock()
{
    // i really like the legend of zelda majora's mask - apr 12 3:30pm
    Food *ptr = g_foodHead;
    int total = 0;
    while (ptr != NULL)
    {
        total += ptr->count;
        ptr = ptr->next;
    };
    return total;
}

void PrintFood(Food *food)
{
    printf("%s [%d]\n", food->name, food->count);
}
// History linkedlist operations

HistoryItem *InitHistoryItem(char note[])
{
    HistoryItem *newOperation = malloc(sizeof(HistoryItem));
    strcpy(newOperation->note, note);
    newOperation->next = NULL;
    return newOperation;
    // enum Operations action;
    // char note[32];
    // struct HistoryItem *next;
}

void AppendHistoryItem(HistoryItem *item)
{
    if (g_logHead == NULL)
    {
        g_logHead = item;
        g_logTail = item;
    }
    else
    {
        g_logTail->next = item;
        g_logTail = item;
    }
    return;
}

// external data handling
// marked with data_ to signify the importance
// running some of these when not needed may be really bad

void data_RewriteLogs()
// rewrite and not write because it rewrites everything every time its called
// no need for the serialization algorithm because its just linebreak seperated data
{
    FILE *logfile;

    logfile = fopen("./CANS.log", "w");
    if (logfile == NULL)
    {
        printf("Failed to open ./CANS.log\n");
        return;
    };

    HistoryItem *ptr = g_logHead;
    while (ptr != NULL)
    {
        fprintf(logfile, "%s\n", ptr->note);
        ptr = ptr->next;
    };
    fclose(logfile);
}

void data_LoadLogs()
{
    FILE *logfile;

    logfile = fopen("./CANS.log", "r");
    if (logfile == NULL)
    {
        printf("No existing log file, writing log...\n");
        return;
    };

    char buffer[LOADING_BUFFER_LENGTH];
    while (fgets(buffer, sizeof(buffer), logfile))
    {
        AppendHistoryItem(InitHistoryItem(buffer));
    }
    fclose(logfile);
}

void data_SerializeFood()
{
    FILE *datafile;
    // char *datastring = malloc(2048*sizeof(char));

    datafile = fopen("./.cansdata", "r");
    if (datafile == NULL)
    {
        printf("No existing data, writing data... ./.cansdata\n");
    }
    fopen("./.cansdata", "w");

    Food *ptr = g_foodHead;
    while (ptr != NULL)
    {
        fprintf(datafile, "%s%c%d%c", ptr->name, COLUMN_DIVIDER, ptr->count, LINE_DIVIDER);
        ptr = ptr->next;
    };
    fclose(datafile);
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
    char buffer[LOADING_BUFFER_LENGTH] = "";
    Food *food = InitFood("", 0);
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
            food = InitFood("", 0);
            strTarget = food->name;
            i = 0;
            continue;
        }
        *(strTarget + i++) = c;
    }
    fclose(datafile);
}

int Exit()
{
    // TODO: free all nodes/foods
    return 0;
}

void LineBreakNTimes(int n)
{
    for (; n > 0; n--)
    {
        printf("\n");
    }
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
    printf("\n%s\n", TITLE_DIVIDER);
}

void AddFoodMenu()
{
    MenuTitle("Add Food", "Add a new type of food to the database");
    // TODO: fgets the name and count of the food and use appendfood to add it to the database;
}
void RemoveFoodMenu()
{
    // TODO: remove node Y by freeing it after assigning its parent node's next value to node Y's next value
}
void UpdateFoodMenu()
{
    // TODO: fgets the name and subcommand
        // add to
        // set
        // subtract from
    // then fgets & sscanf the amount
}

void MainTree()
{
    MenuTitle("Menu", "View important or urgent info and choose operations");
    // TODO: Display 3 most recent operations
    // TODO: Display 0-3 highest and lowest stocked items
    // TODO: maybe? make above two configurable
    int command;
    char cmdBuffer[100];

    printf("Please enter a command\n");
    printf("[1] to UPDATE quantities of the stored foods\n");
    printf("[2] to ADD a food to the database\n");
    printf("[3] to REMOVE a food from the database\n");
    printf("[4] to LIST out the current database\n");
    printf("[5] for session logs.\n");
    printf("[6] to exit the program\n");
    printf("Press a listed number followed by [Enter] to choose an option.\n");
    // getchar();
    fgets(cmdBuffer, sizeof(cmdBuffer), stdin);
    int scanStatus = sscanf(cmdBuffer, "%d", &command);

    printf("%d bobo\n", command); // TODO: remove debug line
    if (scanStatus != 1)
    {
        printf("Invalid command.");
        return;
    }
    switch (command)
    {
    case 1:
        LineBreakNTimes(2);
        g_state = UPDATE_MENU;
        break;
    case 2:
        LineBreakNTimes(2);
        g_state = ADD_MENU;
    case 3:
        LineBreakNTimes(2);
        g_state = REMOVE_MENU;
    case 4:
        // whoopee, short enough to handle in the maintree
        MenuTitle("Current Database", "Each food and how much is in stock");
        IterateFoods(PrintFood);
    case 5:
        LineBreakNTimes(2);
    default:
        break;
    }
}
