// Implements a dictionary's functionality

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define DICTIONARY "dictionaries/large"

unsigned int trie_size = 0;

#include "dictionary.h"

// Represents number of children for each node in a trie
#define N 27

// Represents a node in a trie
typedef struct node
{
    bool is_word;
    struct node *children[N];
}
node;

// Represents a trie
node *root;

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // Initialize trie
    root = malloc(sizeof(node));
    if (root == NULL)
    {
        return false;
    }
    root->is_word = false;
    for (int i = 0; i < N; i++)
    {
        root->children[i] = NULL;
    }

    // Open dictionary
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        //unload();TODO
        return false;
    }

    // Buffer for a word
    char word[LENGTH + 1];

    // Insert words into trie
    while (fscanf(file, "%s", word) != EOF)
    {
        node *current_node = root;

        //Read every character of the word
        for (int letter = 0; word[letter] != '\0'; letter++)
        {
            int index = getindex(word[letter]);
            if(index>=0)
            {
                //If we haven't seen this part of any word yet, prepare new node
                if(current_node->children[index]==NULL)
                {
                    current_node->children[index] = malloc(sizeof(node));
                    current_node = current_node->children[index];
                    for (int i = 0; i < N; i++)
                    {
                        current_node->children[i] = NULL;
                    }
                }else
                {
                    current_node = current_node->children[index];
                }
            }
        }

        //The whole word has been loaded into tire - mark as word
        current_node->is_word = true;
        trie_size++;

    }

    // Close dictionary
    fclose(file);

    // Indicate success
    return true;
}

unsigned int size(void)
{
    return trie_size;
}

int getindex(const char checked_char){
    if(checked_char == '\'')
    {
        return 26;
    }else if(isalpha(checked_char))
    {
        return tolower(checked_char) - 'a';
    }else
    {
        return -1;
    }
}



bool check(const char *word)
{
    node *current_node = root;
    for(int i = 0; word[i] != '\0'; i++)
    {
        int index = getindex(word[i]);

        //check if character is valid, if not return false
        if(index>=0)
        {
            //If there is no pointer from this trie index, there is no such word. Else go to next node
            if(current_node->children[index] == NULL)
            {
                return false;
            }
            else
            {
                current_node = current_node->children[index];
            }
        }
        else
        {
            return false;
        }
    }

    //If we are here, it means we have read all the letters. We can now check if it's a word
    if(current_node->is_word == true)
    {
        return true;
    }else
    {
        return false;
    }
}

int main()
{
    bool result = load(DICTIONARY);
  //  printf("%i", result);
//    printf("%i", trie_size);
    printf("%i\n", trie_size);
    printf("%i/n", check("afslk"));
    return 0;
}