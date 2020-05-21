#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Calculataes the Levenshtein distance between two strings
 * @param string1
 * @param string2
 * source: https://en.wikipedia.org/wiki/Levenshtein_distance
 */
int levenshteinDistance(char* string1, char* string2)
{
    int size1 = (strlen(string1) + 1);
    int size2 = (strlen(string2) + 1);
    
    if (size1 == 0)
    {
        return size2;
    }
    else if (size2 == 0)
    {
        return size1;
    }
    
    int dist[size1][size2];
    
    // Initialize first row to string 1 and first col to string 2.
    // Initialize all other squares to 0.
    for (int i = 0; i < size2; i++)
    {
        for (int j = 0; j < size1; j++)
        {
            if (i == 0)
            {
                dist[j][i] = j;
            }
            else if (j == 0)
            {
                dist[j][i] = i;
            }
            else
            {
                dist[j][i] = 0;
            }
        }
    }
    
    // Calculate lev distance between chars.
    for (int i = 1; i < size2; i++)
    {
        for (int j = 1; j < size1; j++)
        {
            int cost = 0;
            // Set cost equal to 1 if the chars are not equal.
            if (string1[j-1] != string2[i-1])
            {
                cost++;
            }
            int min = 0;
            int case1 = dist[j-1][i] + 1;
            int case2 = dist[j][i-1] + 1;
            int case3 = dist[j-1][i-1] + cost;
            if (case1 < case2 && case1 < case3)
            {
                min = case1;
            }
            else if (case2 < case3)
            {
                min = case2;
            }
            else
            {
                min = case3;
            }
            dist[j][i] = min;
        }
    }
    // Bottom right array square contains lev distance
    return dist[size1-1][size2-1];
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    char* newWord;
    // Load dictionary.txt to hashtable.
    do
    {
        int i = -1;
        newWord = nextWord(file);
        if (newWord != NULL)
        {
            hashMapPut(map, newWord, i);
            free(newWord);
        }
    } while (newWord != NULL);

}

/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        // Check if all word chars are alphabetical.
        // Convert uppdercase letters to lowercase.
        int i = 0;
        int nonAlphaFlag = 0;
        while (inputBuffer[i] != '\0' && !nonAlphaFlag)
        {
            if (!isalpha(inputBuffer[i]))
            {
                printf("Error: Enter an alphabetic word.\n");
                nonAlphaFlag++;
            }
            else
            {
                inputBuffer[i] = tolower(inputBuffer[i]);
            }
            i++;
        }
        
        // Check if word is in dictionary.
        if (hashMapContainsKey(map, inputBuffer))
        {
            printf("The inputted word %s is spelled correctly.\n", inputBuffer);
        }
        // If not traverse hash table and calculate Levenshein distances.
        else
        {
            // Create table of char* to hold words and table of ints to hold corresponding lev distances.
            char *close[5];
            int levs[5] = {-1, -1, -1, -1, -1};
            for (int index = 0; index < hashMapCapacity(map); index++)
            {
                HashLink *current = map->table[index];
                while (current != NULL)
                {
                    // get Lev Dist and set value equal to it.
                    int val = levenshteinDistance(inputBuffer, current->key);
                    current->value = val;
                    
                    int minVal = 0, maxVal = 0;
                    int minLoc = -1, maxLoc = -1;
                    // Find min and max values in the levs table.
                    for (int i = 0; i < 5; i++)
                    {
                        if (levs[i] < minVal)
                        {
                            minVal = levs[i];
                            minLoc = i;
                        }
                        else if (levs[i] > maxVal)
                        {
                            maxVal = levs[i];
                            maxLoc = i;
                        }
                    }
                    // If min value is less that zero, set it equal to val
                    if (minVal < 0)
                    {
                        levs[minLoc] = val;
                        close[minLoc] = current->key;
                    }
                    // Otherwise check if it's smaller than the largest value in levs
                    else if (val < maxVal)
                    {
                        levs[maxLoc] = val;
                        close[maxLoc] = current->key;
                    }
                    current = current->next;
                }
            }
            printf("The inputted word %s is spelled incorrectly.\n", inputBuffer);
            printf("Did you mean %s, %s, %s, %s, or %s?\n", close[0], close[1], close[2], close[3], close[4]);
        }
        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
    }
    hashMapDelete(map);
    return 0;
}
