#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <math.h>
#include <time.h> // Added for time functions

#define WIDTH 60
#define HEIGHT 16
#define SAVE_FILE "savegame.txt" // Save file for game state

// Maze characters
#define WALL '#'
#define PLAYER 'P'
#define GOAL 'G'
#define EMPTY ' '
#define TRAP '*' // Define trap character as '*'

// Player position
int playerX = 1;
int playerY = 1;

// Current level
int currentLevel = 1; // Start at level 1

// Maze layouts for levels
char maze1[HEIGHT][WIDTH] = {
    "############################################################",
    "#P     #           #              #                        #",
    "# ###  # ######### ### ####### ########### ##### ####### ###",
    "#   #  #      #         #                     #            #",
    "### ####### # ########### ######### ##### ####### ##########",
    "#         # #         #        *    #     #                #",
    "# ####### # # ####### ########## ######## #########   ######",
    "#       #   #   #         #                              # #",
    "##### ####### ### ####### ########## ### ######### ####### #",
    "#              #     *            #                   *    #",
    "# ####### ######## ######### ######### ####### ####### #####",
    "#       #       *        #      *                          #",
    "# ########### ########## ####### #############*######## ####",
    "#       #       #                               #         G#",
    "############################################################",
};


char maze2[HEIGHT][WIDTH] = {
    "############################################################",
    "#P    #                            #     #                 #",
    "# ### # ### ###### ##### # # ### ### ##### #  ########  *###",
    "#   # #   #      #       # # #             #      # #      #",
    "### # # # ###### # ##### # # #### ##### # ######  # # ## # #",
    "#   #   #        #           #  #     # #      #    #  #   #",
    "### ########### ####### #### ## # ### # #####  ### ##  # # #",
    "#         #         #              *  #     #      #   # # #",
    "# ######  # #######*####*######### # ##### #########*#     #",
    "#      #                                    #              #",
    "# ### ## ######## #######*## ########### # ####  ####*## # #",
    "#   #         *     #           #    *                   # #",
    "# #*####### ######### ###### ##### #######*#*#### #### #####",
    "# #     *      *   *                           *           #",
    "### ##### # ##   #   #######*################     ####### G#",
    "############################################################"
};


char maze3[HEIGHT][WIDTH] = {
    "############################################################",
    "#P          #                   #                          #",
    "# ##### ### # ####### ####### ### ##### ##### ##########*###",
    "#     #   #         #         #     #         #            #",
    "##### ### ##### ### ##### ####### ##### ##### ####### ######",
    "#     #         #                      *       #      #    #",
    "# ######## ########### ####### ############### # #### # ####",
    "#               #       #                      #    #      #",
    "# ####### # ########### ####### ######## # ###### #### #####",
    "#       #     *          #               #        *        #",
    "######### ####### ###### ####################### ###### ####",
    "#             *        #           #      *         #      #",
    "# ###### ######## ######## #### ## ############ ######## ###",
    "#          *          #    *    *               *          G",
    "# ##### ############# ####      ############### ####### ####",
    "############################################################"
};


// Active maze pointer
char (*currentMaze)[WIDTH];

// Set the current maze layout
void loadMaze(int level) {
    switch (level) {
        case 1:
            currentMaze = maze1;
            break;
        case 2:
            currentMaze = maze2;
            break;
        case 3:
            currentMaze = maze3;
            break;
        default:
            printf("You've completed all levels! Congratulations!\n");
            Sleep(2000);
            currentLevel = 1; // Reset to level 1
            loadMaze(currentLevel);
    }
}

void gotoxy(int x, int y)  /// for box animation
{
    static HANDLE h = NULL;
    if(!h)
        h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD c = { x, y };
    SetConsoleCursorPosition(h,c);
}


// Portable clear screen function
void clearScreen() {
    system("cls");
}

// Function to print the maze and remaining time
void printMaze(int remainingTime, time_t startTime) {
    clearScreen();
    int elapsedTime = time(NULL) - startTime;  // Calculate elapsed time (how much time has passed since the game started)

    printf("\033[33m=== Level %d ===\033[0m\n// Remaining Time: \033[31m%d\033[0m seconds //\n", currentLevel, remainingTime);
    printf("Press '\033[31mQ\033[0m' to save game and exit.\n");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (currentMaze[i][j] == WALL) {
                printf("\033[31m█\033[0m"); // Red for walls
            } else if (currentMaze[i][j] == PLAYER) {
                printf("\033[32m%c\033[0m", PLAYER); // Green for player
            } else if (currentMaze[i][j] == GOAL) {
                printf("\033[33m%c\033[0m", GOAL); // Yellow for goal
            } else if (currentMaze[i][j] == TRAP) {
                // Make traps invisible if less than 10 seconds have passed
                if (elapsedTime < 10) {
                    printf("%c", EMPTY); // Print empty space
                } else {
                    printf("\033[35m%c\033[0m", TRAP); // Magenta for trap
                }
            } else {
                printf("%c", currentMaze[i][j]); // Empty space
            }
        }
        printf("\n");
    }
}

// Function to save the game state in text format
void saveGame() {
    FILE *file = fopen(SAVE_FILE, "w"); // Open in text write mode
    if (file == NULL) {
        perror("Error saving game");
        return;
    }
    fprintf(file, "%d %d %d\n", playerX, playerY, currentLevel);
    fclose(file);
    printf("Game saved successfully.\n");
}

// Function to load the game state from a text file
void loadGame() {
    FILE *file = fopen(SAVE_FILE, "r"); // Open in text read mode
    if (file == NULL) {
        printf("No saved game found. Starting a new game...\n");
        startNewGame();
        return;
    }
    if (fscanf(file, "%d %d %d", &playerX, &playerY, &currentLevel) != 3) {
        printf("Error reading save file. Starting a new game...\n");
        fclose(file);
        startNewGame();
        return;
    }
    fclose(file);

    loadMaze(currentLevel);

    // Remove any existing 'P' from the maze
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (currentMaze[i][j] == PLAYER) {
                currentMaze[i][j] = EMPTY;
            }
        }
    }

    // Place player in the loaded position
    currentMaze[playerY][playerX] = PLAYER;
    printf("Game loaded successfully.\n");
}


void  big_box_animation()
{
    system("chcp 65001 > nul"); // Set terminal to UTF-8 encoding
    int time = 1;

    for(int i = 71; i>=1; i--)
    {
        Sleep(time);
        gotoxy(i,1);
        printf("\033[32m¤\033[0m");
        fflush(stdout);
    }

    for(int i = 1; i<=30; i++)
    {
        Sleep(time);
        gotoxy(1,i);
        printf("\033[32m¤\033[0m");
        fflush(stdout);
    }

    for(int i = 1; i<=71; i++)
    {
        Sleep(time);
        gotoxy(i,30);
        printf("\033[32m¤\033[0m");
        fflush(stdout);
    }

    for(int i = 30; i>=1; i--)
    {
        Sleep(time);
        gotoxy(71,i);
        printf("\033[32m¤\033[0m");
        fflush(stdout);
    }
}


/// animation for box;
void  box_animation()
{
    system("chcp 65001 > nul"); // Set terminal to UTF-8 encoding
    int time = 5;

    for(int i = 50; i>=1; i--)
    {
        Sleep(time);
        gotoxy(i,1);
        printf("\033[32m¤\033[0m");
        fflush(stdout);
    }

    for(int i = 1; i<=25; i++)
    {
        Sleep(time);
        gotoxy(1,i);
        printf("\033[32m¤\033[0m");
        fflush(stdout);
    }

    for(int i = 1; i<=50; i++)
    {
        Sleep(time);
        gotoxy(i,25);
        printf("\033[32m¤\033[0m");
        fflush(stdout);
    }

    for(int i = 25; i>=1; i--)
    {
        Sleep(time);
        gotoxy(50,i);
        printf("\033[32m¤\033[0m");
        fflush(stdout);
    }
}

// Function to display the menu
void displayMenu() {
    clearScreen();
    box_animation();
    printf("\n\n\n\n\n");
    printf(" \033[32m¤\033[0m         \033[45m === MAZE GAME === \033[0m\n\n");
    printf(" \033[32m¤\033[0m         \033[31mTHE ONLY WAY TO WIN IS TO PLAY\n\n\033[0m");
    printf(" \033[32m¤\033[0m         \033[33m1) Start new game.\n\n\033[0m");
    printf(" \033[32m¤\033[0m         \033[33m2) Resume game.\n\n\033[0m");
    printf(" \033[32m¤\033[0m         \033[33m3) Instructions.\n\n\033[0m");
    printf(" \033[32m¤\033[0m         \033[33m4) About developers.\n\n\033[0m");
    printf(" \033[32m¤\033[0m         \033[33m5) Quit.\n\n\033[0m");
    printf(" \033[32m¤\033[0m         \033[36mChoose an option: \033[0m");
}

// Function to start a new game
void startNewGame() {
    playerX = 1;
    playerY = 1;
    currentLevel = 1; // Reset to level 1
    loadMaze(currentLevel); // Load the first maze
}

// Main game loop
void gameLoop() {
    char input;
    time_t startTime, currentTime;
    int remainingTime;

    while (1) {
        startTime = time(NULL);  // Record the start time of the level

        // Run the level and count time for 60 seconds
        while (1) {
            currentTime = time(NULL);  // Get the current time
            remainingTime = 60 - difftime(currentTime, startTime);  /// 60 seconds timer

            // Check if the time is up
            if (remainingTime <= 0) {
                printf("Time's up! You failed to complete the level.\n");
                Sleep(2000);
                return;  // Return to the main menu
            }

            // Update the maze with remaining time
            currentMaze[playerY][playerX] = PLAYER;  // Update player position
            printMaze(remainingTime, startTime);  // Pass remaining time and start time to print function

            input = _getch();
            if (input == 'w' && currentMaze[playerY - 1][playerX] != WALL) {
                currentMaze[playerY][playerX] = EMPTY;
                playerY--;
            } else if (input == 's' && currentMaze[playerY + 1][playerX] != WALL) {
                currentMaze[playerY][playerX] = EMPTY;
                playerY++;
            } else if (input == 'a' && currentMaze[playerY][playerX - 1] != WALL) {
                currentMaze[playerY][playerX] = EMPTY;
                playerX--;
            } else if (input == 'd' && currentMaze[playerY][playerX + 1] != WALL) {
                currentMaze[playerY][playerX] = EMPTY;
                playerX++;
            } else if (input == 'q') {
                saveGame();
                printf("Thank you for playing. Exiting...\n");
                Sleep(2000);
                exit(0);
                //return;
            }

            // Check for goal
            if (currentMaze[playerY][playerX] == GOAL) {
                printf("Congratulations! You've reached the goal!\n");
                Sleep(2000);
                currentLevel++;
                playerX = 1;
                playerY = 1;
                loadMaze(currentLevel);
                break;  // Exit level and reset the timer for the new level
            }

            // Check for trap
            if (currentMaze[playerY][playerX] == TRAP) {
                printf("You hit a trap! Game over!\n");
                Sleep(2000);
                return;  // Return to the main menu
            }
        }
    }
}

int main() {
    system("chcp 65001 > nul"); // Set terminal to UTF-8 encoding
    int choice;
    while (1) {
        displayMenu();
        scanf("%d", &choice);
        if (choice == 1) {
            startNewGame();
            gameLoop();
        } else if (choice == 2) {
            loadGame();
            gameLoop();
        } else if (choice == 3) {
            clearScreen();
            big_box_animation();
            printf("\n\n\n");
            printf(" \033[32m¤\033[0m  === Instructions ===\n\n");
            printf(" \033[32m¤\033[0m  1. Objective:\n");
            printf(" \033[32m¤\033[0m    - Navigate the maze to reach the goal ('\033[33mG\033[0m') with in \033[31m60\033[0m second.\n");
            printf(" \033[32m¤\033[0m     - Avoid traps ('\033[35m*\033[0m') that will show up in \033[31m10\033[0m second from start.\n\n");
            printf(" \033[32m¤\033[0m  2. Controls:\n");
            printf(" \033[32m¤\033[0m     - '\033[33mW\033[0m': Move Up\n");
            printf(" \033[32m¤\033[0m     - '\033[33mS\033[0m': Move Down\n");
            printf(" \033[32m¤\033[0m     - '\033[33mA\033[0m': Move Left\n");
            printf(" \033[32m¤\033[0m     - '\033[33mD\033[0m': Move Right\n\n");
            printf(" \033[32m¤\033[0m  3. Obstacles:\n");
            printf(" \033[32m¤\033[0m     - Walls ('\033[31m█\033[0m'): You cannot pass through them.\n\n");
            printf(" \033[32m¤\033[0m  4. Winning:\n");
            printf(" \033[32m¤\033[0m     - Reach the goal ('\033[33mG\033[0m') to advance to the next level.\n\n");
            printf(" \033[32m¤\033[0m  Press any key to return to the menu...");
            _getch();
        } else if (choice == 4) {
            clearScreen();
            box_animation();
            printf("\n\n\n");
            printf(" \033[32m¤\033[0m     === About Developers ===\n\n");
            printf(" \033[32m¤\033[0m     This game is developed by,\n\n");
            printf(" \033[32m¤\033[0m     \033[33m1. Thasinur Mohian\033[0m\n\n");
            printf(" \033[32m¤\033[0m     \033[33m2. Md. Samidul Islam\033[0m\n\n");
            printf(" \033[32m¤\033[0m     \033[33m3. Minhaz Uddin\033[0m\n\n");
            printf(" \033[32m¤\033[0m     Thank you for playing!\n\n");
            printf(" \033[32m¤\033[0m     Press any key to return to the menu...");
            _getch();
        } else if (choice == 5) {
            printf("\n\n \033[32m¤\033[0m       Thank you for playing! Goodbye!\n\n\n\n");
            break;
        } else {
            printf("Invalid choice. Try again.\n");
            Sleep(2000);
        }
    }
    return 0;
}

