#include <stdio.h>      
#include <stdlib.h>     // Voor malloc, free, rand, srand
#include <time.h>       // Voor tijdsfunctie srand(time(NULL))
#include <string.h>     
#include <stdbool.h>    
//extra
#include <stdint.h>     



typedef struct {
    int id;              // Kamer id waar de speler zich bevindt
    int health;          // Gezondheid van de speler
    int damage;          // Schade die de speler doet
} Player;

typedef struct {
    int id;                  // Kamer id
    int visited;             
    int Monster;     // een monster is in de kamer (0 of 1)
    int Item;        // een item is in de kamer (0 of 1)
    int Treasure;    // een schat is in de kamer (0 of 1)
} Room;

typedef struct {
    int monsterhealth;  
    int monsterdamage;  
} Monster;

typedef struct {
    int healthBoost;   
    int damageBoost;   
} Item;
