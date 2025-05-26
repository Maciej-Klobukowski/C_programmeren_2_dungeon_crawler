#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>     

// dit is de main player struct basically je character stats
typedef struct {
    int id;              // Welke kamer de speler zich bevindt (current room)
    int health;          
    int damage;          // Hoeveel schade je doet per aanval
    int baseMaxHealth;   // Standaard max HP (altijd 100)
    int bonusMaxHealth;  // Extra HP van items die je oppakt
} Player;

typedef struct {
    int monsterhealth;   // HP van het monster
    int monsterdamage;   // Schade die het monster doet (au!)
} Monster;

// Items die je kan vinden - maken je sterker
typedef struct {
    int healthBoost;     // Hoeveel extra HP dit item geeft
    int damageBoost;     // Hoeveel extra damage dit item geeft
} Item;

// Game room properties - wat er in elke kamer zit
typedef struct {
    int id;              // Kamer nummer
    int visited;         // Ben je hier al geweest? (1=ja, 0=nee)
    int hasMonster;      // Zit er een monster in deze kamer? (1=ja, 0=nee)
    int hasItem;         // Ligt er een item in deze kamer? (1=ja, 0=nee)
    int hasTreasure;     // Is dit de treasure kamer? (1=ja, 0=nee) - JACKPOT!
    Monster monster;     // Het monster dat in deze kamer zit (als hasMonster = 1)
    Item item;           // Het item dat in deze kamer ligt (als hasItem = 1)
} GameRoom;

// Room struct met connecties array (max 4 connecties per kamer)
typedef struct Room {
    int id;                  // speciale kamer ID
    GameRoom game_room;      // Alle game properties van deze kamer
    int connections[4];      // Array met IDs van connected kamers (max 4)
    int num_connections;     // Hoeveel connecties deze kamer heeft (0-4) als het werkte
} Room;

typedef struct AdjListNode {
    int dest;
    struct AdjListNode* next;
} AdjListNode;

typedef struct AdjList {
    AdjListNode* head;
} AdjList;

typedef struct Graph {
    int num_rooms;
    Room* rooms;
    AdjList* adj_lists;
} Graph;

AdjListNode* new_adj_list_node(int dest);
Graph* create_graph(int num_rooms);
bool add_edge(Graph* graph, int src, int dest);
Graph* generate_dungeon(int num_rooms);
void populate_rooms_with_content(Graph* dungeon);
void print_dungeon(Graph* dungeon);
void free_dungeon(Graph* dungeon);
void bgevechtMetMonster(Player *speler, Monster *monster);
void generateBinary(int *randomgetal16, int *bin, char *binary);
Item generateRandomItem();
void applyItem(Player *speler, Item item);
void game_loop(Graph* dungeon, Player* player);
int find_farthest_room(Graph* dungeon);
int count_connections(Graph* graph, int room);
bool are_connected(Graph* graph, int room1, int room2);

// Main functie - hier begint de hele game!
int main() {
    srand(time(NULL));      // Zet random seed zodat het niet steeds hetzelfde is
    int num_rooms = 20;     // Hoeveel kamers de dungeon heeft (kan je aanpassen)
    Graph* dungeon = generate_dungeon(num_rooms);      // Maak de dungeon structuur
    populate_rooms_with_content(dungeon);              // Vul kamers met monsters/items/treasure
    print_dungeon(dungeon);                             // Print de connecties (voor debugging)
    int treasure_room = find_farthest_room(dungeon);   // Zoek de verste kamer voor treasure
    printf("\nTreasure is in room: %d\n", treasure_room);

    // Maak de player character met starting stats
    Player player = {
        .id = 0,               // Start in kamer 0
        .health = 100,         // Start HP
        .damage = 20,          // Start damage
        .baseMaxHealth = 100,  // Max HP zonder items
        .bonusMaxHealth = 0    // Extra HP van items (start = 0)
    };
    game_loop(dungeon, &player);  // Start de game loop!
    free_dungeon(dungeon);
    return 0;
}

// Add bidirectional connection between two rooms (max 4 connections per room)
bool add_edge(Graph* graph, int src, int dest) {
    if (src == dest) return false;                          // Can't connect room to itself
    if (graph->rooms[src].num_connections >= 4) return false;   // Source room full
    if (graph->rooms[dest].num_connections >= 4) return false;  // Destination room full
    
    // Check if rooms are already connected
    for (int i = 0; i < graph->rooms[src].num_connections; i++) {
        if (graph->rooms[src].connections[i] == dest) return false;
    }
    
    // Add bidirectional connection
    graph->rooms[src].connections[graph->rooms[src].num_connections++] = dest;
    graph->rooms[dest].connections[graph->rooms[dest].num_connections++] = src;
    return true;
}

// Generate connected dungeon with rooms having max 4 connections each
Graph* generate_dungeon(int num_rooms) {
    if (num_rooms <= 0) return NULL;

    Graph* dungeon = create_graph(num_rooms);

    // Connect each room to at least one previous room to ensure connectivity
    for (int i = 1; i < num_rooms; i++) {
        int attempts = 0;
        bool connected = false;

        while (!connected && attempts < 100) {
            int connect_to = rand() % i;               // Connect to any previous room
            connected = add_edge(dungeon, i, connect_to);
            attempts++;
        }
    }

    for (int i = 0; i < num_rooms; i++) {
        int extra_connections = 1 + rand() % 2;

        for (int j = 0; j < extra_connections; j++) {
            if (count_connections(dungeon, i) >= 4) break;

            int attempts = 0;
            bool success = false;

            while (attempts < 100) {
                int possible_connection = rand() % num_rooms;
                if (possible_connection == i) {
                    attempts++;
                    continue;
                }

                success = add_edge(dungeon, i, possible_connection);
                if (success) break;
                attempts++;
            }
        }
    }

    return dungeon;
}

void populate_rooms_with_content(Graph* dungeon) {
    int treasure_room = find_farthest_room(dungeon);

    for (int i = 0; i < dungeon->num_rooms; i++) {
        if (i != 0 && rand() % 100 < 40) {
            dungeon->rooms[i].game_room.hasMonster = 1;
            if (rand() % 2) {
                dungeon->rooms[i].game_room.monster.monsterhealth = 400;
                dungeon->rooms[i].game_room.monster.monsterdamage = 228;
            } else {
                dungeon->rooms[i].game_room.monster.monsterhealth = 70;
                dungeon->rooms[i].game_room.monster.monsterdamage = 15;
            }
        }

        if (rand() % 100 < 30) {
            dungeon->rooms[i].game_room.hasItem = 1;
            dungeon->rooms[i].game_room.item = generateRandomItem();
        }

        if (i == treasure_room) {
            dungeon->rooms[i].game_room.hasTreasure = 1;
        }
    }
}

// Combat systeem met binaire patronen - zo cool!
void bgevechtMetMonster(Player *speler, Monster *monster) {
    int randomgetal16, bin;
    char binary[9];  // String voor binair patroon

    // Gevecht gaat door tot iemand dood is 
    while (speler->health > 0 && monster->monsterhealth > 0) {
        generateBinary(&randomgetal16, &bin, binary);   // Maak random binair patroon

        printf("\nNieuwe ronde! Binair patroon: %s\n", binary);

        // Ga door elk bit in het binaire patroon
        for (int i = 0; binary[i] != '\0'; i++) {
            if (speler->health <= 0 || monster->monsterhealth <= 0) break;

            if (binary[i] == '1') {
                // Bij '1' valt de speler aan let's go!
                printf("Speler valt aan voor %d schade!\n", speler->damage);
                monster->monsterhealth -= speler->damage;
                printf("Monster HP: %d\n", monster->monsterhealth);
            } else {
                // Bij '0' valt het monster aan
                printf("Monster valt aan voor %d schade!\n", monster->monsterdamage);
                speler->health -= monster->monsterdamage;
                printf("Speler HP: %d\n", speler->health);
            }
        }
    }

    if (speler->health > 0) {
        speler->health += 10;
        int totalMaxHP = speler->baseMaxHealth + speler->bonusMaxHealth;

        if (speler->health > totalMaxHP) {
            speler->health = totalMaxHP;
        }

        printf("You got lucky! HP hersteld naar %d/%d\n",
               speler->health, totalMaxHP);
    }
}

// Maak een nieuwe node voor de linked list (oud systeem, werkt nog stee
AdjListNode* new_adj_list_node(int dest) {
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    newNode->dest = dest;        // Welke kamer dit naar wijst
    newNode->next = NULL;        // Nog geen volgende node
    return newNode;
}

// Maak de hele dungeon structuur - basically de foundation van alles
Graph* create_graph(int num_rooms) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));  // Allocate memory voor de graph
    graph->num_rooms = num_rooms;                   // Hoeveel kamers we hebben

    // Maak alle kamers aan en zet alles op 0/leeg
    graph->rooms = (Room*)malloc(num_rooms * sizeof(Room));
    for (int i = 0; i < num_rooms; i++) {
        graph->rooms[i].id = i;                     // Kamer nummer
        graph->rooms[i].game_room.id = i;           // Game room ook het nummer geven
        graph->rooms[i].game_room.visited = 0;      // Nog niet bezocht
        graph->rooms[i].game_room.hasMonster = 0;   // Geen monster (nog)
        graph->rooms[i].game_room.hasItem = 0;      // Geen item (nog)
        graph->rooms[i].game_room.hasTreasure = 0;  // Geen treasure (nog)
        graph->rooms[i].num_connections = 0;        // Nog geen connecties
    }

    // Oude linked list systeem (werkt nog steeds maar we gebruiken arrays nu)
    graph->adj_lists = (AdjList*)malloc(num_rooms * sizeof(AdjList));
    for (int i = 0; i < num_rooms; i++) {
        graph->adj_lists[i].head = NULL;            // Lege linked lists
    }

    return graph;  // Return de hele dungeon!
}

// Tel hoeveel connecties een kamer heeft (oud systeem)
int count_connections(Graph* graph, int room) {
    int count = 0;
    AdjListNode* node = graph->adj_lists[room].head;  // Start bij eerste node
    while (node) {
        count++;              // Tel deze connectie
        node = node->next;    // Ga naar volgende
    }
    return count;             // Return totaal aantal
}

// Check of twee kamers met elkaar verbonden zijn (oud systeem)
bool are_connected(Graph* graph, int room1, int room2) {
    AdjListNode* node = graph->adj_lists[room1].head;  // Start bij room1's connecties
    while (node) {
        if (node->dest == room2) return true;   // Gevonden! Ze zijn connected
        node = node->next;                      // Probeer volgende
    }
    return false;  // Nope, niet connected
}

// Print alle connecties van elke kamer (handig voor debugging)
void print_dungeon(Graph* dungeon) {
    for (int i = 0; i < dungeon->num_rooms; i++) {
        printf("Room %d connects to: ", i);    // Print kamer nummer
        // Print alle connecties van deze kamer
        for (int j = 0; j < dungeon->rooms[i].num_connections; j++) {
            printf("%d ", dungeon->rooms[i].connections[j]);
        }
        printf("(total: %d)\n", dungeon->rooms[i].num_connections);  // En het totaal
    }
}

// Memory cleanup - belangrijk anders krijg je memory leaks!
void free_dungeon(Graph* dungeon) {
    if (!dungeon) return;  // Als er niks is, doe niks

    // Free alle linked list nodes (oud systeem)
    for (int i = 0; i < dungeon->num_rooms; i++) {
        AdjListNode* node = dungeon->adj_lists[i].head;
        while (node) {
            AdjListNode* temp = node;  // Bewaar huidige node
            node = node->next;         // Ga naar volgende
            free(temp);                // Free de oude node
        }
    }

    // Free alle allocated memory
    free(dungeon->adj_lists);  // Free linked lists array
    free(dungeon->rooms);      // Free rooms array
    free(dungeon);             // Free de hele graph
}

// Dit converteert een random getal naar binair - voor combat systeem
void generateBinary(int *randomgetal16, int *bin, char *binary) {
    *randomgetal16 = rand() % 17;  // Random getal tussen 0 en 16
    int rem = 0, place = 1;
    *bin = 0;                      // Reset binair getal

    // Special case voor 0 - anders crasht het
    if (*randomgetal16 == 0) {
        binary[0] = '0';           // Zet gewoon '0' in de string
        binary[1] = '\0';          // En stop het af
        *bin = 0;
        return;
    }

    // Converteer decimaal naar binair (als int, bijv. 5 wordt 101)
    int temp = *randomgetal16;
    while (temp > 0) {
        rem = temp % 2;
        temp = temp / 2;
        *bin = *bin + (rem * place);
        place = place * 10;
    }

    // Converteer binair int naar een array (zonder leading zeros) wist nie da kan gebeuren
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

Item generateRandomItem() {
    Item item;
    int itemType = rand() % 2;

    if (itemType == 0) {
        item.healthBoost = (rand() % 16) + 5;
        item.damageBoost = 0;
    } else {
        item.healthBoost = 0;
        item.damageBoost = (rand() % 6) + 2;
    }

    return item;
}

void applyItem(Player *speler, Item item) {
    if (item.healthBoost > 0) {
        speler->bonusMaxHealth += item.healthBoost;
        speler->health += item.healthBoost;
        printf("Health boosted by %d! HP is nu %d/%d\n",
               item.healthBoost,
               speler->health,
               speler->baseMaxHealth + speler->bonusMaxHealth);
    } else if (item.damageBoost > 0) {
        speler->damage += item.damageBoost;
        printf("Damage boosted by %d! New damage: %d\n", item.damageBoost, speler->damage);
    }
}

// Zoek de verste kamer van start (voor treasure placement) - BFS algoritme!
int find_farthest_room(Graph* dungeon) {
    // Array om afstanden bij te houden (-1 = niet bezocht)
    int* distances = (int*)malloc(dungeon->num_rooms * sizeof(int));
    for (int i = 0; i < dungeon->num_rooms; i++) distances[i] = -1;  // Alles op -1

    // Queue voor BFS (breadth-first search) - basic CS algoritme
    int queue[dungeon->num_rooms];
    int front = 0, rear = 0;

    queue[rear++] = 0;    // Start bij kamer 0
    distances[0] = 0;     // Afstand van start naar zichzelf = 0

    // BFS loop - bezoek alle kamers en tel afstanden
    while (front < rear) {
        int current = queue[front++];  // Pak volgende kamer uit queue

        // Kijk naar alle connecties van huidige kamer (NIEUW systeem - connections array!)
        for (int i = 0; i < dungeon->rooms[current].num_connections; i++) {
            int connected_room = dungeon->rooms[current].connections[i];
            if (distances[connected_room] == -1) {  // Als nog niet bezocht
                distances[connected_room] = distances[current] + 1;  // Afstand = huidige + 1
                queue[rear++] = connected_room;     // Voeg toe aan queue
            }
        }
    }

    // Zoek de kamer met de grootste afstand
    int max_distance = 0, farthest_room = 0;
    for (int i = 0; i < dungeon->num_rooms; i++) {
        if (distances[i] > max_distance) {
            max_distance = distances[i];   // Nieuwe record
            farthest_room = i;             // Dit is de verste kamer
        }
    }

    free(distances);        // Clean up memory
    return farthest_room;   // Return de verste kamer voor treasure!
}

// De main game loop hier gebeurt alle actie
void game_loop(Graph* dungeon, Player* player) {
    int current_room_id = player->id;  // Start kamer
    bool game_running = true;          // Game aan het runnen?
    bool has_won = false;              // Heeft speler gewonnen?

    // Main game loop - blijft runnen tot je dood bent of wint
    while (game_running) {
        Room* current_room = &dungeon->rooms[current_room_id];  // Huidige kamer
        current_room->game_room.visited = 1;                    // Mark als bezocht
        printf("\nYou are in room %d\n", current_room_id);     // Laat zien waar je bent
        printf("Connections to rooms: ");

        // Print alle connecties 
        AdjListNode* node = dungeon->adj_lists[current_room_id].head;
        while (node) {
            printf("%d ", node->dest);  // Print connected kamer nummer
            node = node->next;
        }
        printf("\n");

        // Check voor monsters tijd om te vechten!
        if (current_room->game_room.hasMonster && current_room->game_room.monster.monsterhealth > 0) {
            printf("There's a monster here!\n");   
            bgevechtMetMonster(player, &current_room->game_room.monster);  // FIGHT!
            if (player->health <= 0) {
                printf("Game over! You died.\n");   // RIP speler
                game_running = false;
                break;
            }
            current_room->game_room.hasMonster = 0;  // Monster dood, remove het
        }

        // Check voor items - free loot!
        if (current_room->game_room.hasItem) {
            printf("You found an item!\n");         
            applyItem(player, current_room->game_room.item);  // Apply het item
            current_room->game_room.hasItem = 0;     // Item opgepakt, remove het
        }

        // Check voor treasure - JACKPOT!
        if (current_room->game_room.hasTreasure) {
            printf("\n*** CONGRATULATIONS! You found the treasure and won the game! ***\n");
            has_won = true;         
            game_running = false;    
            break;
        }

        printf("\nPlayer status: HP %d/%d, Damage %d\n",
               player->health,
               player->baseMaxHealth + player->bonusMaxHealth,
               player->damage);

        if (game_running) {
            printf("\nEnter room number to move to, or -1 to quit: ");
            int choice;
            if (scanf("%d", &choice) != 1) {
                while (getchar() != '\n');
                continue;
            }

            if (choice == -1) {
                game_running = false;
                printf("Thanks for playing!\n");
            } else {
                bool valid_move = false;
                node = dungeon->adj_lists[current_room_id].head;
                while (node) {
                    if (node->dest == choice) {
                        valid_move = true;
                        break;
                    }
                    node = node->next;
                }

                if (valid_move) {
                    current_room_id = choice;
                    player->id = current_room_id;
                } else {
                    printf("Invalid move! No connection to room %d\n", choice);
                }
            }
        }
    }

    if (has_won) {
        printf("\n========================================\n");
        printf("= YOU ARE VICTORIOUS!                 =\n");
        printf("= You conquered the dungeon and found =\n");
        printf("= the legendary treasure!            =\n");
        printf("========================================\n");
        printf("Final stats: HP %d/%d, Damage %d\n",
               player->health,
               player->baseMaxHealth + player->bonusMaxHealth,
               player->damage);
    }
}
