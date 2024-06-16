#ifndef LABIRINT_H
#define LABIRINT_H

typedef struct {
    int x, y;
} Agent;

typedef struct {
    int x, y, step;
} Collision;

typedef struct {
    char size_x;
    char size_y;
    char **map;
    Agent **agent_moves;    
    Agent *agents;
    char number_of_agents;
    Agent **porhibited_positions;
} Labirint;

typedef struct Node {
    int x, y;
    struct Node* parent;
} Node;

void create_labirint_from_file(Labirint *labirint, const char *filename);
void free_labirint(Labirint *labirint);
char get_number_of_agents(Labirint *labirint);
void labirint_visualisation(Labirint *labirint, char mode);
void find_paths(Labirint *labirint);
void sinc_moves(Labirint *labirint);
void print_moves(Labirint *labirint);
void visualisate_moves(Labirint *labirint);
Labirint* copy_labirint(Labirint *labirint, int agent_index, Agent collision);
void print_porhibited_map(Labirint *labirint);
int play(Labirint *labirint);

#endif // LABIRINT_H
