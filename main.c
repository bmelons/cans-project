#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_FOOD_NAME_LENGTH 64
#define LOADING_BUFFER_LENGTH 128
#define COLUMN_DIVIDER '\t'
#define LINE_DIVIDER '\n'
#define TITLE_DIVIDER "------"

// @ TAYLER, see if your code editor has a TODO thing that searches for all instances of TODO in a codebase and puts it in a list, so useful epic

//TODO: one of us, probably me because youre busy, goes and standardizes the return codes, some functions return 0 to indicate success and some return 1 for the same reason
//      the c standard is that 0 == success so lets do that

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

typedef struct Food
{
    char name[MAX_FOOD_NAME_LENGTH];
    int count;
    struct Food *next;
} Food;

typedef struct HistoryItem
{
    char note[128];
    struct HistoryItem *next;

} HistoryItem;

// GLOBALS
enum UserStates g_usrState = MAIN_MENU;
Food *g_foodHead;
Food *g_foodTail;
HistoryItem *g_logHead;
HistoryItem *g_logTail;

// PROTOTYPES
// Food List Management
Food *InitFood(char name[], int count);
Food *QueryFood(char query[]);
void AppendFood(Food *food);
int RemoveFood (Food *food);
void IterateFoods(void (*callback)(Food *));
int TotalStock();
void PrintFood(Food *food);
// History Management
HistoryItem *InitHistoryItem(char note[]);
void AppendHistoryItem(HistoryItem *item);
void DisplayHistory();
void DisplayNRecentChanges(int n);
// Data Management
void data_RewriteLogs();
void data_LoadLogs();
void data_SerializeFood();
void data_DeserializeFood();
// Program States and Utility
int MainTree();
void AddFoodMenu();
void RemoveFoodMenu();
void UpdateFoodMenu();
void MenuTitle(char *header, char *subheader);
void LineBreakNTimes(int n);
void TrimNewline(char *str);
void Pause();
int Confirm(char msg[]);
void ClearBuffer();
void LowerString(char *str);
int Exit();

int main(void)
{
    printf("Welcome to C.A.N.S.\n");
    int active = 1;

    // On load setup
    data_DeserializeFood();
    data_LoadLogs();

    while (active)
    {

        // break;
        switch (g_usrState)
        {
        default:
        case MAIN_MENU:
            if (!MainTree())
            {
                active = 0;
            }
            break;
        case ADD_MENU:
            AddFoodMenu();
            break;
        case REMOVE_MENU:
            RemoveFoodMenu();
            break;
        }

        LineBreakNTimes(1);
        // ClearBuffer();
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

Food *QueryFood(char query[])
{   
    LowerString(query);
    Food *ptr = g_foodHead;

    while (ptr != NULL)
    {
        char s[LOADING_BUFFER_LENGTH];
        strcpy(s,ptr->name);
        LowerString(s);
        if (strstr(s,query) != NULL)
        {
            return ptr;
        }
    }

    return NULL;
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

int RemoveFood(Food *food) {
    Food *ptr = g_foodHead;
    
    if (g_foodHead == food) {
        g_foodHead = food->next;
        free(food);
        return 0;
    }

    while (ptr != NULL) { // loop through list
        if (ptr->next == food) {
            ptr->next = food->next;
            free(food);
            return 0;
        }
        ptr=ptr->next;
    }
    return 1;
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

void DisplayHistory()
{
    HistoryItem *ptr = g_logHead;
    int id = 1;
    while (ptr != NULL)
    {
        printf("%d: %s", id++, ptr->note);
        ptr = ptr->next;
    };
}

int CalculateHistoryLength()
{
    int counter = 0;
    HistoryItem *ptr = g_logHead;
    while (ptr != NULL)
    {
        ptr = ptr->next;
        counter++;
    }
    return counter;
}

void DisplayNRecentChanges(int n)
{

    int counter = 0;
    int length = CalculateHistoryLength();
    HistoryItem *ptr = g_logHead;
    while (counter < length - n && ptr->next != NULL)
    {
        ptr = ptr->next;
        counter++;
    }
    if (counter <= 0)
    {
        printf("No recent changes\n");
        LineBreakNTimes(1);
        return;
    }
    printf("Recent Changes (old->new)\n");
    while (ptr != NULL)
    {
        printf("- [%3d] \"%s\"\n", counter, ptr->note);
        ptr = ptr->next;
        counter++;
    }
    LineBreakNTimes(1);
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
        HistoryItem *new = InitHistoryItem(buffer);
        AppendHistoryItem(new);
        TrimNewline(new->note);
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
    fclose(datafile);
    datafile = fopen("./.cansdata", "w");

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
        fclose(fopen("./.cansdata", "w"));

        return;
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
            *(strTarget + i) = '\0';
            strTarget = buffer;
            i = 0;
            continue;
        }
        if (c == LINE_DIVIDER)
        {
            food->count = strtol(buffer, NULL, 10);
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

void TrimNewline(char *str)
{
    str[strcspn(str, "\n")] = '\0';
}

void Pause()
{
    printf("Press enter to continue...\n");
    getchar();
}

int Confirm(char msg[])
{
    char input[LOADING_BUFFER_LENGTH];
    char command;

    while (1)
    {
        printf("%s (y/n): ", msg);

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            return 0; // Handle EOF/Error
        }

        if (sscanf(input, " %c", &command) == 1)
        {
            command = tolower(command);

            if (command == 'y')
            {
                return 1;
            }
            else if (command == 'n')
            {
                return 0;
            }
        }

        // 3. If we get here, the input was invalid
        printf("Invalid input! Please enter 'y' or 'n'.\n");
    }
}

void ClearBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void LowerString(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        str[i] = tolower(str[i]);
        i++;
    }
    return;
}

int Exit()
{
    data_SerializeFood();
    data_RewriteLogs();
    // TODO: free all nodes/foods
    Food *ptr = g_foodHead;
    Food *prev = g_foodHead;
    while (ptr != NULL)
    {
        prev = ptr;
        ptr = ptr->next;
        free(prev);
    }
    

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
    printf("░░░░░░░░░░░░░░░░░\n");
    printf("[CANS] ");
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
    char countBuffer[LOADING_BUFFER_LENGTH];
    Food *newFood = InitFood("", 0);
    printf("Please enter the name of the new food: ");
    fgets(newFood->name, sizeof(newFood->name), stdin);
    printf("Please enter an initial amount of the good: ");
    fgets(countBuffer, sizeof(countBuffer), stdin);
    sscanf(countBuffer, "%d", &(newFood->count));
    TrimNewline(newFood->name);

    int addConfirmation = Confirm("Please confirm the database addition");
    if (addConfirmation)
    {
        AppendFood(newFood);
        HistoryItem *logitem = InitHistoryItem("Added item.");
        AppendHistoryItem(logitem);
        printf("Added %s [x%d] to the database\n", newFood->name, newFood->count);
    }
    else
    {
        free(newFood);
        printf("Cancelled operation");
    }

    int repeatConfirmation = Confirm("Would you like to continue adding to the database?");
    if (repeatConfirmation == 0)
    {
        g_usrState = MAIN_MENU;
    }
    // user state is already in add mode, dont need to change it if the user wants to continue

    return;
}


void RemoveFoodMenu()
{
    char buffer[LOADING_BUFFER_LENGTH];
    printf("Input the name of the food you wish to remove (fuzzy-searched)\n");
    fgets(buffer,sizeof(buffer),stdin);
    TrimNewline(buffer);
    Food *food = QueryFood(buffer);
    if (food == NULL) {
        printf("Could not find the food.");
    }
    else {
        RemoveFood(food);
        printf("Removed food successfully");
    }

    if (Confirm("Do you want to continue removing foods?")) {
        return;
    }
    g_usrState = MAIN_MENU;
}
void UpdateFoodMenu()
{
    // TODO: fgets the name and subcommand
    // add to
    // set
    // subtract from
    // then fgets & sscanf the amount
}

int MainTree()
{
    MenuTitle("Menu", "View important or urgent info and choose operations");
    DisplayNRecentChanges(3);
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

    if (scanStatus != 1)
    {
        printf("Invalid command.");
        return 1;
    }
    switch (command)
    {
    case 1:
        LineBreakNTimes(2);
        g_usrState = UPDATE_MENU;
        break;
    case 2:
        LineBreakNTimes(2);
        g_usrState = ADD_MENU;
        break;
    case 3:
        LineBreakNTimes(2);
        g_usrState = REMOVE_MENU;
        break;
    case 4:
        // whoopee, short enough to handle in the maintree
        MenuTitle("Current Database", "Each food and how much is in stock");
        if (g_foodHead == NULL)
        {
            printf("No foods are in the database.\n");
        }
        IterateFoods(PrintFood);
        Pause();
        break;
    case 5:
        MenuTitle("History", "Log of operations");
        if (g_logHead == NULL)
        {
            printf("No operations have been performed yet.\n");
        }
        DisplayHistory();
        Pause();
        LineBreakNTimes(1);
        break;
    case 6:
        return 0;
    default:
        break;
    }
    return 1;
}
