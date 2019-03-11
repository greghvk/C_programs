// Implements a spell-checker

#include <ctype.h>
#include <stdio.h>

#include "dictionary.h"



// Default dictionary
#define DICTIONARY "dictionaries/large"

int main(int argc, char *argv[])
{
    // Check for correct number of args
    if (argc != 2 && argc != 3)
    {
        printf("Usage: speller [dictionary] text\n");
        return 1;
    }

    // Load dictionary
    bool loaded = load(dictionary);

    // Exit if dictionary not loaded
    if (!loaded)
    {
        printf("Could not load %s.\n", dictionary);
        return 1;
    }

    // Try to open text
    char *text = (argc == 3) ? argv[2] : argv[1];
    FILE *file = fopen(text, "r");
    if (file == NULL)
    {
        printf("Could not open %s.\n", text);
        unload();
        return 1;
    }

    // Prepare to report misspellings
    printf("\nMISSPELLED WORDS\n\n");

    // Prepare to spell-check
    int index = 0, misspellings = 0, words = 0;
    char word[LENGTH + 1];

    // Spell-check each word in text
    for (int c = fgetc(file); c != EOF; c = fgetc(file))
    {
        // Allow only alphabetical characters and apostrophes
        if (isalpha(c) || (c == '\'' && index > 0))
        {
            // Append character to word
            word[index] = c;
            index++;

            // Ignore alphabetical strings too long to be words
            if (index > LENGTH)
            {
                // Consume remainder of alphabetical string
                while ((c = fgetc(file)) != EOF && isalpha(c));

                // Prepare for new word
                index = 0;
            }
        }

        // Ignore words with numbers
        else if (isdigit(c))
        {
            // Consume remainder of alphanumeric string
            while ((c = fgetc(file)) != EOF && isalnum(c));

            // Prepare for new word
            index = 0;
        }

        // We must have found a whole word
        else if (index > 0)
        {
            // Terminate current word
            word[index] = '\0';

            // Update counter
            words++;

            // Check word's spelling
            bool misspelled = !check(word);

            // Print word if misspelled
            if (misspelled)
            {
                printf("%s\n", word);
                misspellings++;
            }

            // Prepare for next word
            index = 0;
        }
    }

    // Check whether there was an error
    if (ferror(file))
    {
        fclose(file);
        printf("Error reading %s.\n", text);
        unload();
        return 1;
    }

    // Close text
    fclose(file);

    // Determine dictionary's size
    unsigned int n = size();

    // Unload dictionary
    bool unloaded = unload();
    
    // Abort if dictionary not unloaded
    if (!unloaded)
    {
        printf("Could not unload %s.\n", dictionary);
        return 1;
    }

    // Calculate time to unload dictionary
    time_unload = calculate(&before, &after);

    // Results
    printf("\nWORDS MISSPELLED:     %d\n", misspellings);
    printf("WORDS IN DICTIONARY:  %d\n", n);
    printf("WORDS IN TEXT:        %d\n", words);
 
    // Success
    return 0;
}
