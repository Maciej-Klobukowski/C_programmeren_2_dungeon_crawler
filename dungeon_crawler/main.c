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


void bgevechtMetMonster(Player *speler, int monsterType);
void generateBinary(int *randomgetal16, int *bin, char *binary);

int main() {

    srand(time(NULL));  // random getall thing
    Player speler = { .id = 1, .health = 100, .damage = 20 };// info voor de speler hp damage en id voor opslag
    int monsterType = (rand() % 2) + 1;// 1 of 2 nee kaas
    bgevechtMetMonster(&speler, monsterType);// fight fight fight

    return 0;

}



void generateBinary(int *randomgetal16, int *bin, char *binary) {
    *randomgetal16 = rand() % 17;  // 0 t/m 16
    int rem = 0, place = 1;
    *bin = 0;

    // Special case voor 0
    if (*randomgetal16 == 0) {
        binary[0] = '0';
        binary[1] = '\0';
        *bin = 0;
        return;
    }

    // Converteer decimaal naar binair (als int, bijv. 5 -> 101)
    int temp = *randomgetal16;
    while (temp > 0) {
        rem = temp % 2;
        temp = temp / 2;
        *bin = *bin + (rem * place);
        place = place * 10;
    }

    // Converteer binair int naar een array (zonder leading zeros)
    temp = *bin;
    int length = 0;
    
    // Bepaal eerst de lengte
    int temp2 = temp;
    while (temp2 > 0) {
        temp2 /= 10;
        length++;
    }
    
    // Vul de array van achter naar voren
    for (int i = length - 1; i >= 0; i--) {
        binary[i] = (temp % 10) + '0';
        temp /= 10;
    }
    binary[length] = '\0';  // Null-terminator
}

void bgevechtMetMonster(Player *speler, int monsterType) {
    Monster monster;
    int randomgetal16, bin;
    char binary[6]; // 8 bits + null terminator

    // Seed de random number generator
    srand(time(0));

    if (monsterType == 1) {
        monster.monsterhealth = 40;
        monster.monsterdamage = 8;
        printf("Je vecht tegen Monster kaaas.\n");
    } else if (monsterType == 2) {
        monster.monsterhealth = 70;
        monster.monsterdamage = 15;
        printf("Je vecht tegen Monster vis.\n");
    } else {
        printf("Onbekend\n");
        return;
    }

    while (speler->health > 0 && monster.monsterhealth > 0) {
        // Genereer een nieuw binair patroon voor elke ronde
        generateBinary(&randomgetal16, &bin, binary);
        
        printf("\nNieuwe ronde! Binair patroon: %s\n", binary);
        
        // Voer aanvallen uit volgens het binaire patroon
        for (int i = 0; binary[i] != '\0'; i++) {
            if (speler->health <= 0 || monster.monsterhealth <= 0) break;
            
            if (binary[i] == '1') {
                // Speler valt aan
                printf("Speler valt aan voor %d schade!\n", speler->damage);
                monster.monsterhealth -= speler->damage;
                printf("Monster HP: %d\n", monster.monsterhealth);
            } else {
                // Monster valt aan
                printf("Monster valt aan voor %d schade!\n", monster.monsterdamage);
                speler->health -= monster.monsterdamage;
                printf("Speler HP: %d\n", speler->health);
            }
        }
    }

    // Resultaat
    if (speler->health <= 0) {
        printf("\nDe speler is verslagen...\n");
    } else {
        printf("\nHet monster is verslagen!\n");
        speler->health += 10;
        printf("Speler geneest 10 HP. Nieuwe HP: %d\n", speler->health);
    }
}