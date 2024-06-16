#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "functions.h"

void set_colour(char color) {
    switch (color) {
        case 1: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY); break;
        case 2: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;
        case 3: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY); break;
        case 4: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break;
        case 5: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE); break;
        case 6: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY); break;
        case 7: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED); break;
        case 8: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); break;
        case 9: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY); break;
        default: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); break;
    }
}

void reset_colour() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void disable_prints() {
    freopen("NUL", "w", stdout);
}

void enable_prints() {
    freopen("CON", "w", stdout);
}

Queue* create_queue(int capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->nodes = (Node**)malloc(sizeof(Node*) * capacity);
    queue->front = 0;
    queue->rear = 0;
    queue->capacity = capacity;
    return queue;
}

void enqueue(Queue* queue, Node* node) {
    if (queue->rear == queue->capacity) return;
    queue->nodes[queue->rear++] = node;
}

Node* dequeue(Queue* queue) {
    if (queue->front == queue->rear) return NULL;
    return queue->nodes[queue->front++];
}

int is_queue_empty(Queue* queue) {
    return queue->front == queue->rear;
}

void free_queue(Queue* queue) {
    free(queue->nodes);
    free(queue);
}

char get_number_of_agents(Labirint *labirint) {
    return labirint->number_of_agents;
}

void labirint_visualisation(Labirint *labirint, char mode) {
    if (mode == HASHS) {
        printf("#"); for (int i = 0; i < labirint->size_x; i++) {
            printf("#");
        } printf("#\n");
        for (int i = 0; i < labirint->size_y; i++) {
            printf("#");
            for (int j = 0; j < labirint->size_x; j++) {
                char agent = 0;
                for (int k = 0; k < get_number_of_agents(labirint); k++) {
                    if (labirint->agents[k].x == j && labirint->agents[k].y == i) {
                        set_colour(k+1);
                        printf("%d", k+1);
                        reset_colour();
                        agent = 1;
                        break;
                    }
                }
                if (!agent) {
                    if (labirint->map[i][j] == WALL) {
                        printf("%c", labirint->map[i][j]);
                    } else if (labirint->map[i][j] < 0 && labirint->map[i][j] >= -10) {
                        set_colour(labirint->map[i][j] * -1);
                        printf("%c", labirint->map[i][j] * -1 + 'A' - 1);
                        reset_colour();
                    } else {
                        printf("%c", labirint->map[i][j]);
                    }
                }
            }
            printf("#\n");
        }
        printf("#"); for (int i = 0; i < labirint->size_x; i++) {
            printf("#");
        } printf("#\n");
    } else if (mode == SQ) {}
}

void create_labirint_from_file(Labirint *labirint, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
        return;
    }
    
    fscanf(file, "%hhd %hhd", &labirint->size_x, &labirint->size_y);

    labirint->map = (char **)malloc(labirint->size_y * sizeof(char *));
    for (int i = 0; i < labirint->size_y; i++) {
        labirint->map[i] = (char *)malloc(labirint->size_x * sizeof(char));
    }
    for (int i = 0; i < labirint->size_y; i++) {
        fscanf(file, "%s", labirint->map[i]);
    }
    labirint->number_of_agents = 0;
    for (int i = 0; i < labirint->size_y; i++) {
        for (int j = 0; j < labirint->size_x; j++) {
            if (labirint->map[i][j] > '0' && labirint->map[i][j] <= '9') {
                labirint->number_of_agents++;
            }
        }
    }

    labirint->agents = (Agent *)malloc(labirint->number_of_agents * sizeof(Agent));

    for (int i = 0; i < labirint->size_y; i++) {
        for (int j = 0; j < labirint->size_x; j++) {
            if (labirint->map[i][j] > '0' && labirint->map[i][j] <= '9') {
                labirint->agents[labirint->map[i][j] - '0' - 1].x = j;
                labirint->agents[labirint->map[i][j] - '0' - 1].y = i;
    
                labirint->map[i][j] = SPACE;

            } else if (labirint->map[i][j] >= 'A' && labirint->map[i][j] <= 'L') {
                labirint->map[i][j] = -(labirint->map[i][j] - 'A' + 1);
            } 
        }
    }

    labirint->porhibited_positions = (Agent **)malloc(labirint->number_of_agents * sizeof(Agent *));
    for (int i = 0; i < labirint->number_of_agents; i++) {
        labirint->porhibited_positions[i] = (Agent *)malloc(sizeof(Agent));
        labirint->porhibited_positions[i][0].x = -1;
        labirint->porhibited_positions[i][0].y = -1;
    }

    fclose(file);
}

void free_labirint(Labirint *labirint) {
    for (int i = 0; i < labirint->size_y; i++) {
        free(labirint->map[i]);
    }
    free(labirint->map);
    free(labirint->agents);
}

char is_game_over(Labirint *labirint) {
    char agents_n = get_number_of_agents(labirint);
    char found_places = 0;
    for (int i = 0; i < agents_n; i++) {
        if (labirint->map[labirint->agents[i].y][labirint->agents[i].x] == -(i + 1)) {
            found_places++;
        }
    }
    return agents_n == found_places;
}

char is_valid_move(Labirint *labirint, char agent_index, int x, int y, int **visited) {
    if (x < 0 || x >= labirint->size_x || y < 0 || y >= labirint->size_y) {
        return 0;
    }
    if (labirint->map[y][x] == WALL || visited[y][x] == 1) {
        return 0;
    }
    for(int i=0;labirint->porhibited_positions[agent_index][i].x!=-1 ; i++){
        if(labirint->porhibited_positions[agent_index][i].x == x && labirint->porhibited_positions[agent_index][i].y == y){
            return 0;
        }
    }
    return 1;
}

int** create_visited_map(int size_x, int size_y) {
    int** visited = (int**)malloc(size_y * sizeof(int*));
    for (int i = 0; i < size_y; i++) {
        visited[i] = (int*)malloc(size_x * sizeof(int));
        for (int j = 0; j < size_x; j++) {
            visited[i][j] = 0;
        }
    }
    return visited;
}

void free_visited_map(int** visited, int size_y) {
    for (int i = 0; i < size_y; i++) {
        free(visited[i]);
    }
    free(visited);
}

void reconstruct_path(Node* current, Labirint *labirint, char agent_index) {
    Node* path = current;
    int path_length = 0;
    while (path != NULL) {
        path = path->parent;
        path_length++;
    }
    labirint->agent_moves[agent_index] = (Agent *)malloc(path_length * sizeof(Agent));
    labirint->agent_moves[agent_index][path_length-1].x = current->x;
    labirint->agent_moves[agent_index][path_length-1].y = current->y;
    path = current->parent;
    for (int i = path_length-2; i >= 0; i--) {
        labirint->agent_moves[agent_index][i].x = path->x;
        labirint->agent_moves[agent_index][i].y = path->y;
        path = path->parent;
    }
}

int find_path_length(Labirint *labirint, int agent_index) {
    if(labirint->agent_moves[agent_index] == NULL){
        return 0;
    }
    int length = 0;
    while(labirint->agent_moves[agent_index][length].x != -1) {
        length++;
    }
    return length;
}

void find_paths(Labirint *labirint) {
    for (int agent_index = 0; agent_index < labirint->number_of_agents; agent_index++) {
        int** visited = create_visited_map(labirint->size_x, labirint->size_y);
        Queue* queue = create_queue(labirint->size_x * labirint->size_y);
        Node* start = (Node*)malloc(sizeof(Node));
        start->x = labirint->agents[agent_index].x;
        start->y = labirint->agents[agent_index].y;
        start->parent = NULL;
        enqueue(queue, start);
        Node* goal = NULL;
        while (!is_queue_empty(queue)) {
            Node* current = dequeue(queue);
            if (labirint->map[current->y][current->x] == -(agent_index + 1)) {
                goal = current;
                break;
            }
            int directions[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
            for (int i = 0; i < 4; i++) {
                int new_x = current->x + directions[i][0];
                int new_y = current->y + directions[i][1];
                if (is_valid_move(labirint, agent_index, new_x, new_y, visited)) {
                    Node* neighbor = (Node*)malloc(sizeof(Node));
                    neighbor->x = new_x;
                    neighbor->y = new_y;
                    neighbor->parent = current;
                    enqueue(queue, neighbor);
                    visited[new_y][new_x] = 1;
                }
            }
        }
        if (goal != NULL) {
            reconstruct_path(goal, labirint, agent_index);
        }
        free_queue(queue);
        free_visited_map(visited, labirint->size_y);
    }
}

void sinc_moves(Labirint *labirint) {
    int max_path_length = 0;
    for (int i = 0; i < labirint->number_of_agents; i++) {
        int path_length = find_path_length(labirint, i);
        if (path_length > max_path_length) {
            max_path_length = path_length;
        }
    }
    for (int i = 0; i < labirint->number_of_agents; i++) {
        int path_length = find_path_length(labirint, i);
        if (path_length < max_path_length) {
            labirint->agent_moves[i] = (Agent *)realloc(labirint->agent_moves[i], max_path_length * sizeof(Agent));
            for (int j = path_length; j < max_path_length; j++) {
                labirint->agent_moves[i][j].x = -1;
                labirint->agent_moves[i][j].y = -1;
            }
        }
    }
}

void print_moves(Labirint *labirint) {
    for (int step = 0; step < find_path_length(labirint, 0); step++) {
        for (int agent_index = 0; agent_index < labirint->number_of_agents; agent_index++) {
            if (labirint->agent_moves[agent_index][step].x != -1) {
                printf("Agent %d: (%d, %d)\n", agent_index + 1, labirint->agent_moves[agent_index][step].x, labirint->agent_moves[agent_index][step].y);
            }
        }
    }
}

void visualisate_moves(Labirint *labirint) {
    int step = 0;
    while (!is_game_over(labirint)) {
        for (int agent_index = 0; agent_index < labirint->number_of_agents; agent_index++) {
            if (labirint->agent_moves[agent_index][step].x != -1) {
                labirint->agents[agent_index].x = labirint->agent_moves[agent_index][step].x;
                labirint->agents[agent_index].y = labirint->agent_moves[agent_index][step].y;
            }
        }
        labirint_visualisation(labirint, HASHS);
        step++;
        Sleep(1000);
    }
}

Labirint* copy_labirint(Labirint *labirint, int agent_index, Agent collision) {
    Labirint* new_labirint = (Labirint*)malloc(sizeof(Labirint));
    new_labirint->size_x = labirint->size_x;
    new_labirint->size_y = labirint->size_y;
    new_labirint->map = (char**)malloc(new_labirint->size_y * sizeof(char*));
    for (int i = 0; i < new_labirint->size_y; i++) {
        new_labirint->map[i] = (char*)malloc(new_labirint->size_x * sizeof(char));
        for (int j = 0; j < new_labirint->size_x; j++) {
            new_labirint->map[i][j] = labirint->map[i][j];
        }
    }
    new_labirint->number_of_agents = labirint->number_of_agents;
    new_labirint->agents = (Agent*)malloc(new_labirint->number_of_agents * sizeof(Agent));
    for (int i = 0; i < new_labirint->number_of_agents; i++) {
        new_labirint->agents[i].x = labirint->agents[i].x;
        new_labirint->agents[i].y = labirint->agents[i].y;
    }
    new_labirint->porhibited_positions = (Agent**)malloc(new_labirint->number_of_agents * sizeof(Agent*));
    for (int i = 0; i < new_labirint->number_of_agents; i++) {
        new_labirint->porhibited_positions[i] = (Agent*)malloc(sizeof(Agent));
        for (int j = 0; labirint->porhibited_positions[i][j].x != -1; j++) {
            new_labirint->porhibited_positions[i][j].x = labirint->porhibited_positions[i][j].x;
            new_labirint->porhibited_positions[i][j].y = labirint->porhibited_positions[i][j].y;
        }
    }
    new_labirint->porhibited_positions[agent_index][0].x = collision.x;
    new_labirint->porhibited_positions[agent_index][0].y = collision.y;
    return new_labirint;
}

void print_porhibited_map(Labirint *labirint) {
    for (int i = 0; i < labirint->number_of_agents; i++) {
        printf("Agent %d prohibited positions:\n", i + 1);
        for (int j = 0; labirint->porhibited_positions[i][j].x != -1; j++) {
            printf("(%d, %d)\n", labirint->porhibited_positions[i][j].x, labirint->porhibited_positions[i][j].y);
        }
    }
}

int* find_priority_list(Labirint *labirint) {
    int* priority_list = (int*)malloc(labirint->number_of_agents * sizeof(int));
    int* path_lengths = (int*)malloc(labirint->number_of_agents * sizeof(int));
    for (int i = 0; i < labirint->number_of_agents; i++) {
        path_lengths[i] = find_path_length(labir
path_lengths[i] = find_path_length(labirint, i);
    }

    for (int i = 0; i < labirint->number_of_agents; i++) {
        int min_index = -1;
        for (int j = 0; j < labirint->number_of_agents; j++) {
            if (priority_list[j] == 0 && (min_index == -1 || path_lengths[j] < path_lengths[min_index])) {
                min_index = j;
            }
        }
        priority_list[i] = min_index + 1;
        priority_list[min_index] = 1;
    }
    return priority_list;
}

void simulate(Labirint *labirint) {
    int* priority_list = find_priority_list(labirint);
    int step = 0;
    while (!is_game_over(labirint)) {
        for (int i = 0; i < labirint->number_of_agents; i++) {
            int agent_index = priority_list[i] - 1;
            if (labirint->agent_moves[agent_index][step].x != -1) {
                int next_x = labirint->agent_moves[agent_index][step].x;
                int next_y = labirint->agent_moves[agent_index][step].y;

                int collision = 0;
                for (int j = 0; j < labirint->number_of_agents; j++) {
                    if (j != agent_index && labirint->agents[j].x == next_x && labirint->agents[j].y == next_y) {
                        collision = 1;
                        break;
                    }
                }

                if (!collision) {
                    labirint->agents[agent_index].x = next_x;
                    labirint->agents[agent_index].y = next_y;
                } else {
                    Labirint* temp_labirint = copy_labirint(labirint, agent_index, (Agent){next_x, next_y});
                    find_paths(temp_labirint);
                    sinc_moves(temp_labirint);

                    int new_path_length = find_path_length(temp_labirint, agent_index);
                    labirint->agent_moves[agent_index] = (Agent*)realloc(labirint->agent_moves[agent_index], new_path_length * sizeof(Agent));
                    for (int k = 0; k < new_path_length; k++) {
                        labirint->agent_moves[agent_index][k].x = temp_labirint->agent_moves[agent_index][k].x;
                        labirint->agent_moves[agent_index][k].y = temp_labirint->agent_moves[agent_index][k].y;
                    }

                    free_labirint(temp_labirint);
                }
            }
        }
        labirint_visualisation(labirint, HASHS);
        step++;
        Sleep(1000);
    }
    free(priority_list);
}

void find_solution(char *filename) {
    Labirint labirint;
    create_labirint_from_file(&labirint, filename);
    find_paths(&labirint);
    sinc_moves(&labirint);
    simulate(&labirint);
    free_labirint(&labirint);
}
