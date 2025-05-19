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


void gevechtMetMonster(Player *speler, int monsterType);

int main() {

    srand(time(NULL));  // random getall thing
    Player speler = { .id = 1, .health = 100, .damage = 20 };// info voor de speler hp damage en id voor opslag
    int monsterType = (rand() % 2) + 1;// 1 of 2 nee kaas
    gevechtMetMonster(&speler, monsterType);// fight fight fight

    return 0;

}

void gevechtMetMonster(Player *speler, int monsterType) {
    Monster monster;

    if (monsterType == 1) {
        monster.monsterhealth = 40;
        monster.monsterdamage = 15;
        printf("Je vecht tegen Hemlmonth the vampire .\n");
    } else if (monsterType == 2) {
        monster.monsterhealth = 70;
        monster.monsterdamage = 10;
        printf("Je vecht tegen izara wind geest.\n");
    } else {
        printf("Onbeked\n");// beter gebeurt dit nie
        return;
    }

    // Start gevecht
    while (speler->health > 0 && monster.monsterhealth > 0) {
        printf("Speler valt monster aan voor %d schade!\n", speler->damage);
        monster.monsterhealth -= speler->damage;

        if (monster.monsterhealth <= 0) {
            printf("de monster is dood gegaan\n");
            break;
        }

        printf("Monster valt aan %d schade!\n", monster.monsterdamage);
        speler->health -= monster.monsterdamage;

        if (speler->health <= 0) {
            printf("YOU DIED!(skill issue)\n");
            return;
        }
        printf("Speler HP: %d | Monster HP: %d\n", speler->health, monster.monsterhealth);
    }   //hp van de speler
    speler->health += 10;// heals anders gaat speler de snel dood
    if (speler->health > 100) speler->health = 100;
    printf("Speler healed 10 HP. Nieuwe HP: %d\n", speler->health);
}
