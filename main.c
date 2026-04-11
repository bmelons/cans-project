#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FOOD_NAME_LENGTH 64
#define DESERIALIZATION_BUFFER_LENGTH 1024
#define COLUMN_DIVIDER '\t'
#define LINE_DIVIDER '\n'

// TODO: move all funcs into prototypes!

enum UserStates {
    Main,
    Add,
    History
};

typedef struct Food
{
    char name[MAX_FOOD_NAME_LENGTH];
    int count;
    struct Food *next;
} Food;

#pragma region 
#pragma endregion
// GLOBALS
Food *g_head;
Food *g_tail;

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
    if (g_head == NULL)
    {
        g_head = food;
        g_tail = food;
    }
    else
    {
        g_tail->next = food;
        g_tail = food;
    }
    return;
}

void IterateFoods(void (*callback)(Food *))
{ // mostly a recipe for later use, but feel free to use with PrintFood
    // under the assumption that following the trail of nexts from the head will always reach the tail
    Food *ptr = g_head;
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
        printf("Failed to open data file ./.cansdata");
    }

    Food *ptr = g_head;
    while (ptr != NULL)
    {
        fprintf(datafile, "%s%c%d\n", ptr->name, COLUMN_DIVIDER, ptr->count);
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
        printf("Failed to open ./.cansdata");
        return;
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
            food->count = strtod(buffer,NULL);
            strcpy(buffer,"");
            AppendFood(food);
            food = InitEmptyFood();
            strTarget = food->name;
            i = 0;
            continue;
        }
        *(strTarget+i++) = c;
        // printf("%c", cur);
    }
}

int Exit()
{
    // TODO: free all nodes/foods
    return 0;
}



int main(void)
{
    printf("Welcome to C.A.N.S.\n");
    enum UserStates state = Main; // 
    // AppendFood( InitFood("hello POOP",1) );
    // AppendFood( InitFood("penis sauce",1));



    // On load setup
    data_DeserializeFood();

    IterateFoods(PrintFood);

    while (1) {
        break;
        switch (state)
        {
        case default:
        case Main:
            printf("Main View ---\n");
            // TODO: Display 3 most recent changes 
            // TODO: Display 3 lowest food supplies
            break;
        }
        
        
        

    }

    return Exit();
}