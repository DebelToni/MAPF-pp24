#ifndef UTILS_H
#define UTILS_H

#include <windows.h>

void set_colour(char color);
void reset_colour();
void disable_prints();
void enable_prints();
char is_game_over(Labirint *labirint);
char is_valid_move(Labirint *labirint, char agent_index, int x, int y, int **visited);
int** create_visited_map(int size_x, int size_y);
void free_visited_map(int** visited, int size_y);
void reconstruct_path(Node* current, Labirint *labirint, char agent_index);
int find_path_length(Labirint *labirint, int agent_index);
int* find_priority_list(Labirint *labirint);

#endif // UTILS_H
