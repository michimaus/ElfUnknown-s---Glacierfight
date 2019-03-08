// Copyright Duican Mihnea - Ionut CA314 2019
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxc 20
#define elm1 "MOVE"
#define elm2 "SNOWSTORM"
#define elm3 "PRINT_SCOREBOARD"
#define elm4 "MELTDOWN"
#define def_mask 255
#define in_file "snowfight.in"
#define out_file "snowfight.out"

// Declaration of the struct-data regarding the elves
typedef struct elf_stat {
    char *nm;
    int pos_x, pos_y, hp, hand, eng, elim;
} elf_stat;

// Declaration of the struct-data regarding the map
typedef struct on_ice {
    int hight, hand, elf_here;
} on_ice;

// Function that swaps two integers
void swap_2(int *x, int *y) {
    *x = *x ^ *y;
    *y = *x ^ *y;
    *x = *x ^ *y;
}

// Function that returns the absolute value of an integer
int positive(int x) {
    if ( x < 0 ) {
        return x * (-1);
    }
    return x;
}

// Calculating either a given position is part of the glaciar
int in_range(int x_pos, int y_pos, int x_cent, int y_cent, int R) {
    int distance = (x_pos - x_cent) * (x_pos - x_cent) +
        (y_pos - y_cent) * (y_pos - y_cent);
        // printf("%d ---->>>\n", distance);
    if ( distance > R * R ) {
        return 0;
    }
    return 1;
}

// Snowballs hiting the elves in a given circular area
void snow_storm(int *remain, int storm, elf_stat **v_elf, on_ice ***map,
    int P, FILE *fout) {
    int x, y, R_s, dmg, i, mask = def_mask;
    // Extrating the data from the input number
    // Using & - operation with an 8-bit mask filled with 1
    // And shifting the bits of the input number by 8 positions to the right
    x = storm & mask;
    storm = (storm >> 8);
    y = storm & mask;
    storm = (storm >> 8);
    R_s = storm & mask;
    storm = (storm >> 8);
    dmg = storm & mask;

    for ( i = 0 ; i < P ; ++i ) {
        if ( (*v_elf)[i].hp <= 0 ) {
            continue;
        }
        // Checking if any elf that is not wet is caught in the snowstorm
        if ( in_range((*v_elf)[i].pos_x, (*v_elf)[i].pos_y, x, y, R_s) ) {
            (*v_elf)[i].hp -= dmg;
            // If an elf gets wet it will be sent back home
            if ( (*v_elf)[i].hp <= 0 ) {
                // Clearing the elf's data out of the map
                (*map)[ (*v_elf)[i].pos_x ][ (*v_elf)[i].pos_y ].elf_here = -1;
                fprintf(fout, "%s was hit by snowstorm.\n",
                    (*v_elf)[i].nm);
                --(*remain);
            }
        }
    }
}

// Function that establishes the number of hits tanken by one elf in order
// To lose the fight
int make_hits(int hp, int dmg) {
    if ( dmg == 0 )
        return 2e9;
    if ( hp % dmg ) {
        return hp / dmg + 1;
    }
    return hp / dmg;
}

// Function that checks the energy and the gloves for the following cell
// And debates the outcome of a possible duel
void next_position(int *remain, elf_stat **elf, on_ice ***map, int next_x,
    int next_y, int idx, FILE *fout) {
    int cost = positive((*map)[ next_x ][ next_y ].hight -
        (*map)[ (*elf)[idx].pos_x ][ (*elf)[idx].pos_y ].hight);
    // Checks if the elf has enaugh energy left in order to access the cell
    if ( (*elf)[idx].eng >= cost ) {
        (*map)[ (*elf)[idx].pos_x ][ (*elf)[idx].pos_y ].elf_here = -1;
        (*elf)[idx].eng -= cost;
        (*elf)[idx].pos_x = next_x;
        (*elf)[idx].pos_y = next_y;
        // Switching with better gloves
        if ( (*map)[ next_x ][ next_y ].hand > (*elf)[idx].hand ) {
            swap_2(&(*map)[ next_x ][ next_y ].hand, &(*elf)[idx].hand);
        }
        // Checks if the following cell is occupied
        if ( (*map)[ next_x ][ next_y ].elf_here != -1 ) {
            int att = (*map)[ next_x ][ next_y ].elf_here;
            int hits1, hits2;
            // Calculating the number of hits for the dueling elves
            hits1 = make_hits((*elf)[att].hp, (*elf)[idx].hand);
            hits2 = make_hits((*elf)[idx].hp, (*elf)[att].hand);
            // Establishing the elf that is going to take the first hit
            if ( (*elf)[idx].eng >= (*elf)[att].eng ) {
                // The winner is the one that will need less hits
                // In order to put down his oponent
                if ( hits1 <= hits2 ) {
                    // Incrementing the winner's value for the oponents
                    // It has defeted
                    ++(*elf)[idx].elim;
                    (*elf)[att].hp = -1;
                    // Recalucating the HP vallue of the winner
                    (*elf)[idx].hp -= ( ( hits1 - 1 ) * (*elf)[att].hand );
                    // If winner is the one who wanted to access the new cell
                    // It will apear to be in that cell
                    (*map)[ next_x ][ next_y ].elf_here = idx;
                    // Adding stamina to the winner
                    (*elf)[idx].eng += (*elf)[att].eng;
                    fprintf(fout, "%s sent %s back home.\n", (*elf)[idx].nm,
                        (*elf)[att].nm);
                    --(*remain);

                } else {
                    ++(*elf)[att].elim;
                    (*elf)[idx].hp = -1;
                    (*elf)[att].hp -= ( hits2 * (*elf)[idx].hand );
                    (*elf)[att].eng += (*elf)[idx].eng;
                    fprintf(fout, "%s sent %s back home.\n", (*elf)[att].nm,
                        (*elf)[idx].nm);
                    --(*remain);
                }

            } else {
                if ( hits2 <= hits1 ) {
                    ++(*elf)[att].elim;
                    (*elf)[idx].hp = -1;
                    (*elf)[att].hp -= ( ( hits2 - 1 ) * (*elf)[idx].hand );
                    (*elf)[att].eng += (*elf)[idx].eng;
                    fprintf(fout, "%s sent %s back home.\n", (*elf)[att].nm,
                        (*elf)[idx].nm);
                    --(*remain);
                } else {
                    ++(*elf)[idx].elim;
                    (*elf)[att].hp = -1;
                    (*elf)[idx].hp -= ( hits1 * (*elf)[att].hand );
                    (*map)[ next_x ][ next_y ].elf_here = idx;
                    (*elf)[idx].eng += (*elf)[att].eng;
                    fprintf(fout, "%s sent %s back home.\n", (*elf)[idx].nm,
                        (*elf)[att].nm);
                    --(*remain);
                }
            }
        } else {
            // The case where the elf gets in the cell without fighting
            // with anybody
            (*map)[ next_x ][ next_y ].elf_here = idx;
        }
    } else {
        return;
    }
}

// Function that will move the elf around the map
void moving_elf(int *remain, int idx, int R, char *move, elf_stat **elf,
    on_ice ***map, FILE *fout) {
    int i = 0, next_x, next_y, fall = 0;
    // Going until the elf gets wet or it finishes its moves
    while ( move[i] != '\0' && (*elf)[idx].hp > 0 ) {
        // Moving up
        if ( move[i] == 'U' ) {
            next_x = (*elf)[idx].pos_x - 1;
            next_y = (*elf)[idx].pos_y;
            // Checking if the elf stays on the map
            if ( next_x < 0 ) {
                fall = 1;
            } else {
                // Calling the function cheking the actions that could accur
                next_position(&(*remain), &(*elf), &(*map), next_x, next_y,
                    idx, fout);
            }
        }
        // Moving left
        if ( move[i] == 'L' ) {
            next_x = (*elf)[idx].pos_x;
            next_y = (*elf)[idx].pos_y - 1;
            if ( next_y < 0 ) {
                fall = 1;
            } else {
                next_position(&(*remain), &(*elf), &(*map), next_x, next_y,
                    idx, fout);
            }
        }
        // Moving right
        if ( move[i] == 'R' ) {
            next_x = (*elf)[idx].pos_x;
            next_y = (*elf)[idx].pos_y + 1;
            if ( next_y > 2 * R ) {
                fall = 1;
            } else {
                next_position(&(*remain), &(*elf), &(*map), next_x, next_y,
                    idx, fout);
            }
        }
        // Moving down
        if ( move[i] == 'D' ) {
            next_x = (*elf)[idx].pos_x + 1;
            next_y = (*elf)[idx].pos_y;
            if ( next_x > 2 * R ) {
                fall = 1;
            } else {
                next_position(&(*remain), &(*elf), &(*map), next_x, next_y,
                    idx, fout);
            }
        }
        // Functon ends when there is one remaining elf
        if ( (*remain) == 1 ) {
            return;
        }
        // Checking if the elf stays in the radius of the glaciar
        if ( fall || in_range((*elf)[idx].pos_x, (*elf)[idx].pos_y,
                R, R, R) == 0 ) {
            (*elf)[idx].hp = -1;
            (*map)[ (*elf)[idx].pos_x ][ (*elf)[idx].pos_y ].elf_here = -1;
            --(*remain);
            fprintf(fout, "%s fell off the glacier.\n", (*elf)[idx].nm);
            return;
        }
        ++i;
    }
}

// Function that checks and initializes the data regarding an elf
void elf_on_ice(int *remain, int R, int idx, elf_stat *elf, on_ice ***map,
    FILE *fout) {
    (*elf).elim = 0;
    // Cheking if the elf is to be found in radius of the glaciar
    if ( in_range((*elf).pos_x, (*elf).pos_y, R, R, R) ) {
        // Setting the position of one elf on the map
        (*map)[ (*elf).pos_x ][ (*elf).pos_y ].elf_here = idx;
        // Getting the gloves
        (*elf).hand = (*map)[ (*elf).pos_x ][ (*elf).pos_y ].hand;
        (*map)[ (*elf).pos_x ][ (*elf).pos_y ].hand = 0;
    } else {
        (*elf).hp = -1;
        --(*remain);
        fprintf(fout, "%s has missed the glacier.\n", (*elf).nm);
    }
}

// Function for realocating the map and for reducing the radius of the glaciar
void melting(int *remain, elf_stat **elf, on_ice ***map, int eng_add,
        int *R, int P, FILE *fout) {
    int i, j;
    // Checking if there are any elves on the edge
    for ( i = 0 ; i < P ; ++i ) {
        if ( (*elf)[i].hp <= 0 ) {
            continue;
        }
        if ( in_range((*elf)[i].pos_x, (*elf)[i].pos_y,
            *R, *R, *R - 1) == 0 ) {
            // Elves getting wet outside of the new radius
            (*elf)[ i ].hp = -1;
            (*map)[ (*elf)[i].pos_x ][ (*elf)[i].pos_y ].elf_here = -1;
            fprintf(fout, "%s got wet because of global warming.\n",
                (*elf)[i].nm);
            --(*remain);
        } else {
            // Updating the position and adding stamina to the other elves
            (*elf)[i].eng += eng_add;
            --(*elf)[i].pos_x;
            --(*elf)[i].pos_y;
        }
    }
    // Updating the data of the map and realocating its size
    for ( i = 0 ; i < (*R) * 2 - 1 ; ++i ) {
        for ( j = 0 ; j < (*R) * 2 - 1 ; ++j ) {
            (*map)[i][j] = (*map)[i + 1][j + 1];
        }
        (*map)[i] = realloc((*map)[i],
            ( 2 * (*R - 1) + 1 ) * sizeof(on_ice));
    }
    for ( i = 2 * (*R) - 1 ; i <= 2 * (*R) ; ++i ) {
        free((*map)[i]);
    }
    --(*R);
}

// Function for establishing the sorting conditions
void sorting_cond(int i, elf_stat **v_elf, int **sorted, int *ok) {
    // By the number of the elves one has tanken out
    if ( (*v_elf)[(*sorted)[i]].elim == (*v_elf)[(*sorted)[i + 1]].elim ) {
        // By the name - lexicographical order
        if ( strcmp((*v_elf)[ (*sorted)[i] ].nm,
            (*v_elf)[ (*sorted)[i + 1] ].nm ) > 0) {
            swap_2(&(*sorted)[i], &(*sorted)[i + 1]);
            *ok = 1;
        }
    } else {
        if ( (*v_elf)[ (*sorted)[i] ].elim <
            (*v_elf)[ (*sorted)[i + 1] ].elim ) {
            swap_2(&(*sorted)[i], &(*sorted)[i + 1]);
            *ok = 1;
        }
    }
}

// Function for making the corect order for the scoreboard
void elf_ranking(elf_stat *v_elf, int P, FILE *fout) {
    int i, *sorted, ok, j, dry_no = 0, wet_no = P - 1;
    // Allocating memory for the auxiliary sorting array
    sorted = malloc(P * sizeof(int));
    // Having the elves sorted by their status of beeing wet or dry
    for ( i = 0 ; i < P ; ++i ) {
        if ( v_elf[i].hp > 0 ) {
            sorted[dry_no] = i;
            ++dry_no;
        } else {
            sorted[wet_no] = i;
            --wet_no;
        }
    }
    ok = 1;
    j = 0;
    // Using buble sort
    // Sorting those elves who are dry
    while ( ok ) {
        ok = 0;
        for ( i =  0 ; i < wet_no - j ; ++i ) {
            sorting_cond(i, &v_elf, &sorted, &ok);
        }
        ++j;
    }
    ok = 1;
    j = 0;
    // Sorting those elves who are wet
    while ( ok ) {
        ok = 0;
        for ( i = dry_no ; i < P - 1 - j ; ++i ) {
            sorting_cond(i, &v_elf, &sorted, &ok);
        }
        ++j;
    }
    // Printing the elves
    fprintf(fout, "SCOREBOARD:\n");
    for ( i = 0 ; i < P ; ++i ) {
        fprintf(fout, "%s\t", v_elf[ sorted[i] ].nm);
        if ( v_elf[sorted[i]].hp > 0 ) {
            fprintf(fout, "DRY\t");
        } else {
            fprintf(fout, "WET\t");
        }
        fprintf(fout, "%d\n", v_elf[ sorted[i] ].elim);
    }
    // Freeing the memory used fot the sorting array
    free(sorted);
}

// Function for geting the data for the map and for the elves
void read_map(int *remain, int *R, int *P, elf_stat **v_elf, on_ice ***map,
    FILE *fin, FILE *fout) {
    int j, i;
    fscanf(fin, "%d%d", R, P);
    *remain = *P;
    // Allocating memory for the data regarding the map and the elves
    *v_elf = malloc(*P * sizeof(elf_stat));
    *map = (on_ice**)malloc((*R * 2 + 1) * sizeof(on_ice*));
    // Reading the map
    for ( i = 0 ; i <= 2 * (*R) ; ++i ) {
        (*map)[i] = (on_ice*)malloc((2 * (*R) + 1) * sizeof(on_ice));
        for ( j = 0 ; j <= 2 * (*R) ; ++j ) {
            (*map)[i][j].elf_here = -1;
            fscanf(fin, "%d%d", &(*map)[i][j].hight,
                        &(*map)[i][j].hand);
        }
    }
    // Reading the elves
    for ( i = 0 ; i < *P ; ++i ) {
        (*v_elf)[i].nm = malloc(maxc * sizeof(char));
        fscanf(fin, "%s", (*v_elf)[i].nm);
        (*v_elf)[i].nm = realloc((*v_elf)[i].nm,
            (strlen((*v_elf)[i].nm) + 1) * sizeof(char));
        fscanf(fin, "%d%d%d%d", &(*v_elf)[i].pos_x, &(*v_elf)[i].pos_y,
            &(*v_elf)[i].hp, &(*v_elf)[i].eng);
        // Calling for the function that checks and initializes
        // The data regarding an elf
        elf_on_ice(&(*remain), *R, i, &(*v_elf)[i], &(*map), fout);
    }
}

int main() {
    // Initiating the main variables
    int R, P, i, idx, remain = 0;
    char comand[20], move[20];
    elf_stat *v_elf;
    on_ice **map;
    // Openeing the files for the input and output
    FILE *fin = fopen(in_file, "r");
    FILE *fout = fopen(out_file, "w");
    // Calling for the function that reads all the data
    read_map(&remain, &R, &P, &v_elf, &map, fin, fout);
    // Reading the comands untill there will be only one elf left
    // Or until the end of the file
    while ( remain > 1 ) {
        fscanf(fin, "%s", comand);
        if ( strcmp(comand, elm1) == 0 ) {
            fscanf(fin, "%d%s", &idx, move);
            moving_elf(&remain, idx, R, move, &v_elf, &map, fout);
        }
        if ( strcmp(comand, elm2) == 0 ) {
            fscanf(fin, "%d", &idx);
            snow_storm(&remain, idx, &v_elf, &map, P, fout);
        }
        if ( strcmp(comand, elm4) == 0 ) {
            fscanf(fin, "%d", &idx);
            melting(&remain, &v_elf, &map, idx, &R, P, fout);
        }
        if ( strcmp(comand, elm3) == 0 ) {
            elf_ranking(v_elf, P, fout);
        }
        // Condition to reach the end of the file
        if ( comand[0] == '\0' ) {
            remain = 0;
        }
        comand[0] = '\0';
    }
    // Finding the winner elf
    if ( remain == 1 ) {
        for ( i = 0 ; i < P ; ++i ) {
            if ( v_elf[i].hp > 0 ) {
                fprintf(fout, "%s has won.\n", v_elf[i].nm);
                i = P;
            }
        }
    }
    // Freeing all memory
    for ( i = 0 ; i <= 2 * R  ; ++i ) {
        free(map[i]);
    }
    for ( i = 0 ; i < P ; ++i ) {
        free(v_elf[i].nm);
    }
    free(map);
    free(v_elf);
    fclose(fout);
    fclose(fin);
    return 0;
}
