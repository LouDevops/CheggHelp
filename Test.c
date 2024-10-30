#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

//Structure for dictionary linked list.
typedef struct wordListNode {
    char characters[30];
    struct wordListNode* nextNode; 
}wordListNode;

//Second structure for linked last regarding words that can be made up of letters in the master word.
typedef struct gameListNode {
    char character [30];
    bool wordFound;
    struct gameListNode* followingNode;
}gameListNode;

//Global root variables for each linked lists.
struct wordListNode* root = NULL;
struct gameListNode* gameRoot = NULL;

int initialization();
struct wordListNode* getRandomWord(int wordCount);
void findWords(char* masterWord);
void getLetterDistribution(const char* str, int* letterDistribution);
bool compareCounts(int *choices, int *candidate);
void displayWorld(wordListNode* masterWord);
void acceptInput();
bool isDone();
void gameLoop(wordListNode* masterWord);
void teardown();
void cleanupGameListNodes();
void cleanupWordListNodes();


//Function that initializes the linked list with the dictionary words.
int initialization(){
    srand(time(NULL));
    FILE* fp;
    int wordCount = 0;
    char line[100];

    //Opens the file in read mode and checks if there if an error opening the file.
    fp = fopen("2of12.txt", "r");
    if (fp == NULL) {
        printf("File has failed to open, check directory or name.");
        return 1;
    }

    //Reads the file line by line, stores into line array, and strips newline characters and carriage returns.
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0;

        for (int i = 0; line[i] != 0; i++) { 
            line[i] = toupper(line[i]);
        }

        //Creates a new node for the linked list.
        struct wordListNode *newNode = NULL;
        newNode = (struct wordListNode *)malloc(sizeof(struct wordListNode));
        
        //Copies the string from the file into the new node.
        strcpy(newNode->characters, line);

        //Adds the new node to the linked list.
        newNode -> nextNode = root;
        root = newNode;       
        wordCount++;
    }

    fclose(fp);
    return wordCount;
}

//Function that selects a random word from the dictionary linked list and returns a word >6 characters.
struct wordListNode* getRandomWord(int wordCount) {

    //Picks a random number between 0 and the word count.
    int randomNumber = rand() % wordCount;

    struct wordListNode* currentNode = root;
    
    //Iterates through the linked list till nth entry.
    for (int i = 0; i < randomNumber; i++) {
        currentNode = currentNode->nextNode;
    }

    //Searches for a word that is longer than 6 characters.
    while (currentNode != NULL) {
        if (strlen(currentNode->characters) > 6) {
        return currentNode; 
        }
        currentNode = currentNode->nextNode;
    }

    //Handles the case where you reach the end of the list without finding a word.
    return getRandomWord(wordCount);
}

//Function that finds words that can be made from the letters of the master word.
void findWords (char* masterWord) {
    struct wordListNode* currentNode = root;

    //Creates an array that holds the distribution of letters in the master word and calls helper function to calculate the number of occurences of each letter.
    int masterLetterDistribution[26] = {0};
    getLetterDistribution(masterWord, masterLetterDistribution);

    //Iterates through the linked list of dictionary words. Also, creates an array for dictionary words to hold the letters and calls helper to caluclate occurences.
    while (currentNode != NULL) {
    int candidateDistribution[26] = {0};
    getLetterDistribution(currentNode->characters, candidateDistribution);

     //Compares the counts of the master word and the candidate word to tests if the letters from the master word can be used to make the candidate word.
     if (compareCounts(masterLetterDistribution, candidateDistribution)) {
        struct gameListNode* newNode = NULL;

             //If compareCounts returns true meaning the master word letters can make up the dictionary word then a new node is created for the linked list.
             newNode = (struct gameListNode *)malloc(sizeof(struct gameListNode));
            
            //Copies the string from the current node into the new node, then adds it to the linked list gamelistNode.
            strcpy(newNode->character, currentNode->characters);
            newNode->wordFound = false;
            newNode->followingNode = gameRoot;
            gameRoot = newNode;
        }
        currentNode = currentNode->nextNode;
    }
}

int compare(const void *a, const void *b) {
    return (*(char *)a - *(char *)b);
}
void displayWorld(wordListNode* masterWord) {
    char sortedLetterWords[30];
    strcpy(sortedLetterWords, masterWord->characters);
    qsort(sortedLetterWords, strlen(sortedLetterWords), sizeof(char), compare);

    // Print the sorted letters
    for (int i = 0; i < strlen(sortedLetterWords); i++) {
        printf("%c ", sortedLetterWords[i]);
    }
    printf("\n---------------\n");

    // Print found words or underscores
    gameListNode* temp = gameRoot;
    while (temp != NULL) {
        if (temp->wordFound) {
            printf("FOUND: %s\n", temp->character);
        } else {
            for (int i = 0; i < strlen(temp->character); i++) {
                printf("_ ");
            }
            printf("\n");
        }
        temp = temp->followingNode;
    }
    printf("---------------\n");
}


void acceptInput() {
    char input [40];
    printf("Enter a guess: ");
    // Accepts the guess from the user, and stores it into the char array. 
    if (fgets(input, sizeof(input), stdin) != NULL) { 
        input[strcspn(input, "\r\n")] = 0; // Strips off carriage returns and line feeds.
        //Capitalizes the entire string of the input
        for (int i = 0; input[i] != 0; i++) { 
            input[i] = toupper(input[i]);
        }
        printf("Your Guess:  %s\n", input);
    }
    gameListNode* temp = gameRoot;
    while(temp != NULL){
        if (strcmp(temp->character, input) == 0){
            temp->wordFound = true;
        }
        temp = temp->followingNode;
    }
}
bool isDone() {
    gameListNode* temp = gameRoot;
    while (temp != NULL) {
        if (!temp->wordFound) {
            return false;
        }
        temp = temp->followingNode;
    }
    return true;
}


// Counts the number of occurences of each letter in the incoming word in the user input string and stores into array.
void getLetterDistribution(const char* str, int* letterDistribution) {
    for ( int i = 0; str[i] != 0; i++) {
//Converts each letter/character into an index that corresponds to place in the alphabet. So if str[i] is Z, Z-A = 25
        letterDistribution [str[i] - 'A'] ++; 
    }
}

// Compares two arrays that represent the choices, and one that matches the candidate word. If the letters in candidate exceed the count in choices returns false. Otherwise, true.
bool compareCounts(int *choices, int *candidate) {
    for (int i = 0; i < 26; i++) {
        if (candidate[i] > choices[i]) {
            return false;
        
        }
    }
    return true;
}

// Loops over displayWorld and acceptInput function, when isDone returns true, loop stops.
void gameLoop(wordListNode* masterWord) {
    do {
        displayWorld(masterWord); // Pass the correct type
        acceptInput();
    } while (!isDone());
}


void teardown() {
    cleanupGameListNodes();
    cleanupWordListNodes();
    printf("All Done.\n");
}

void cleanupGameListNodes() {
     gameListNode* temp = gameRoot;
    while (temp != NULL) {
        gameListNode* next = temp->followingNode;
        free(temp);
        temp = next;
    }
    gameRoot = NULL;
}
void cleanupWordListNodes() {
     wordListNode* temp = root;
    while (temp != NULL) {
        wordListNode* next = temp->nextNode;
        free(temp);
        temp = next;
    }
    root = NULL;
}



int main(int argc, char **argv) {
    int wordCount = initialization();

    
    struct wordListNode* masterWordNode = getRandomWord(wordCount);
    
    if (masterWordNode == NULL) {
        printf("No suitable master word found.\n");
        return 1; // Exit if no master word is found
    }

    findWords(masterWordNode->characters);

    gameLoop(masterWordNode);
    teardown(); 
    return 0;
}