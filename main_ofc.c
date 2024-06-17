#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void set_colour(char color) {
    switch (color) {
        case 1: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY); break; // red
        case 2: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY); break; // green
        case 3: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY); break; // blue
        case 4: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY); break; // yellow
        case 5: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE); break; // purple
        case 6: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY); break; // pink
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

typedef enum {
    WALL = '#',
    SPACE = '.',
} Cell;

typedef enum {
    HASHS = 0,
    SQ = 1,
} Modes;

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
    Collision **porhibited_positions;
} Labirint;

typedef struct Node {
    int x, y;
    int step_on;
    struct Node* parent;
} Node;

typedef struct Queue {
    Node** nodes;
    int front;
    int rear;
    int capacity;
} Queue;

Queue* create_queue(int capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->nodes = (Node**)malloc(sizeof(Node*) * capacity);
    queue->front = 0;
    queue->rear = 0;
    queue->capacity = capacity;
    return queue;
}

void increase_capacity(Queue* queue) {
    queue->nodes = (Node**)realloc(queue->nodes, sizeof(Node*) * queue->capacity * 2);
    queue->capacity *= 2;
}

void enqueue(Queue* queue, Node* node) {
    if (queue->rear == queue->capacity){
        increase_capacity(queue);
    } //return;
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

    labirint->porhibited_positions = (Collision **)malloc(labirint->number_of_agents * sizeof(Collision *));
    for (int i = 0; i < labirint->number_of_agents; i++) {
        labirint->porhibited_positions[i] = (Collision *)malloc(sizeof(Collision));
        labirint->porhibited_positions[i][0].x = -1;
        labirint->porhibited_positions[i][0].y = -1;
    }

    fclose(file);
}

void free_labirint(Labirint *labirint) {
    for (int i = 0; i < labirint->size_y; i++) {
        free(labirint->map[i]);
    }
    // da osvobodq porhibbited moves i oste nesto ako ostanalo
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

char is_valid_move(Labirint *labirint, char agent_index, int x, int y, int **visited, int step_rn) {
    if (x < 0 || x >= labirint->size_x || y < 0 || y >= labirint->size_y) {
        return 0;
    }
    if (labirint->map[y][x] == WALL || visited[y][x] == 1) {
        return 0;
    }
    for(int i=0;labirint->porhibited_positions[agent_index][i].x!=-1 ; i++){
        if(labirint->porhibited_positions[agent_index][i].x == x && labirint->porhibited_positions[agent_index][i].y == y){
            if(labirint->porhibited_positions[agent_index][i].step>step_rn){
                //return 0;
            }else{
                return 0;
            }
        }
    }
    return 1;
}

int** create_visited_map(int size_x, int size_y) {
    int** visited = (int**)malloc(size_y * sizeof(int*));
    for (int i = 0; i < size_y; i++) {
        visited[i] = (int*)malloc(size_x * sizeof(int));
        memset(visited[i], 0, size_x * sizeof(int));
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
    int steps = 0;
    Node* path = current;
    while (path->parent) {
        steps++;
        path = path->parent;
    }

    labirint->agent_moves[agent_index] = (Agent*)malloc((steps + 1) * sizeof(Agent));

    int index = steps - 1;
    while (current->parent) {
        labirint->agent_moves[agent_index][index--] = (Agent){current->x, current->y};
        current = current->parent;
    }

    labirint->agent_moves[agent_index][steps] = (Agent){-1, -1};
}

void find_paths(Labirint *labirint){
    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    for (char agent_index = 0; agent_index < labirint->number_of_agents; agent_index++) {

        Agent start = labirint->agents[agent_index];
        Agent goal = {0, 0};
        for (int i = 0; i < labirint->size_y; i++) {
            for (int j = 0; j < labirint->size_x; j++) {
                if (labirint->map[i][j] == -(agent_index + 1)) {
                    goal.x = j;
                    goal.y = i;
                    break;
                }
            }
        }

        Queue* queue = create_queue(1000);
        int** visited = create_visited_map(labirint->size_x, labirint->size_y);

        Node* start_node = (Node*)malloc(sizeof(Node));
        start_node->x = start.x;
        start_node->y = start.y;
        start_node->parent = NULL;


        start_node->step_on = 0;
        enqueue(queue, start_node);


        Node* current = NULL;
        while (!is_queue_empty(queue)) {

            current = dequeue(queue);
            if (current->x == goal.x && current->y == goal.y) {
                break;
            }

            visited[current->y][current->x] = 1;

            for (int i = 0; i < 4; i++) {
                int new_x = current->x + directions[i][0];
                int new_y = current->y + directions[i][1];

                int step_on = current->step_on + 1;

                if (is_valid_move(labirint, agent_index, new_x, new_y, visited,step_on)) {
                    Node* neighbor = (Node*)malloc(sizeof(Node));
                    neighbor->x = new_x;
                    neighbor->y = new_y;
                    neighbor->parent = current;
                    neighbor->step_on = step_on;

                    enqueue(queue, neighbor);
                }
            }

        }

        if (current->x == goal.x && current->y == goal.y) {
            reconstruct_path(current, labirint, agent_index);
        }

        free_queue(queue);
        free_visited_map(visited, labirint->size_y);
    }

}

int find_path_length(Labirint *labirint, int agent_index) {
    int length = 0;
    while (!(labirint->agent_moves[agent_index][length].x == -1 && labirint->agent_moves[agent_index][length].y == -1)) {
        length++;
    }
    return length;
}

int* find_priority_list(Labirint *labirint){
    int *path_lenghts = (int *)malloc(labirint->number_of_agents * sizeof(int));
    for (int i = 0; i < labirint->number_of_agents; i++) {
        path_lenghts[i] = i;
    }
    for (int i = 0; i < labirint->number_of_agents; i++) {
        for (int j = 0; j < labirint->number_of_agents - i - 1; j++) {
            if (find_path_length(labirint, path_lenghts[j]) < find_path_length(labirint, path_lenghts[j + 1])) {
                int temp = path_lenghts[j];
                path_lenghts[j] = path_lenghts[j + 1];
                path_lenghts[j + 1] = temp;
            }
        }
    }
    printf("Priority list: ");
    for (int i = 0; i < labirint->number_of_agents; i++) {
        printf("%d ", path_lenghts[i] + 1);
    }printf("\n");
    return path_lenghts;
}

void sinc_moves(Labirint *labirint){
    int *priority_list = find_priority_list(labirint);
   
    int slowest_agent = priority_list[0];
    
    for (int i = 0; i < labirint->number_of_agents; i++) {
        if (i != slowest_agent) {
            int moves_to_add = find_path_length(labirint, slowest_agent) - find_path_length(labirint, i);
            printf("Moves to add for agent %d = %d",i, moves_to_add);
            labirint->agent_moves[i] = (Agent *)realloc(labirint->agent_moves[i], (find_path_length(labirint, slowest_agent)+1) * sizeof(Agent)); // nz dali trbqva da ima +1 ili ne
            int path_length = find_path_length(labirint, i);
            for (int j = 0; j < moves_to_add; j++) {
                
                for (int k = path_length + j; k > 0; k--) {
                    labirint->agent_moves[i][k] = labirint->agent_moves[i][k - 1];
                }
                
                for (int k = 0; labirint->porhibited_positions[i][k].x!=-1; k++) {
                    printf("AAAAAAAAAAAAAAAAA");
                    if (labirint->porhibited_positions[i][k].x == labirint->agent_moves[i][j].x && labirint->porhibited_positions[i][k].y == labirint->agent_moves[i][j].y) {
                        labirint->agent_moves[i][j] = labirint->agent_moves[i][j + 1];
                        labirint->agent_moves[i][j + 1] = labirint->agent_moves[i][j + 2];
                    }
                }
            }
            labirint->agent_moves[i][find_path_length(labirint, slowest_agent)] = (Agent){-1, -1};
            printf(";\n");
        }
    }
    free(priority_list);
}

void print_moves(Labirint *labirint){
    for (int i = 0; i < get_number_of_agents(labirint); i++) {
        printf("Agent %d: ", i + 1);
        for (int j = 0; j < find_path_length(labirint, i); j++) {
            if (labirint->agent_moves[i][j].x == 0 && labirint->agent_moves[i][j].y == 0) {
                break;
            }
            printf("(%d, %d) ", labirint->agent_moves[i][j].x, labirint->agent_moves[i][j].y);
        }
        printf("\n");
    }
}

void visualisate_moves(Labirint *labirint){
    int step = 0;
    // system("cls");
    printf("Initial state:\n");
    labirint_visualisation(labirint, HASHS);
    Sleep(3000);
    while (!is_game_over(labirint)) {
        printf("Step: %d\n", step);
        for (int i = 0; i < get_number_of_agents(labirint); i++) {
            if (labirint->agent_moves[i][0].x == -1 && labirint->agent_moves[i][0].y == -1) {
                continue;
            }
            labirint->agents[i].x = labirint->agent_moves[i][step].x;
            labirint->agents[i].y = labirint->agent_moves[i][step].y;
        }
        labirint_visualisation(labirint, HASHS);
        Sleep(1000);
        
        step++;
        
    }
}

Labirint* copy_labirint(Labirint *labirint, int agent_index, Collision collision) {
    Labirint *new_labirint = (Labirint *)malloc(sizeof(Labirint));
    new_labirint->size_x = labirint->size_x;
    new_labirint->size_y = labirint->size_y;

    new_labirint->map = (char **)malloc(new_labirint->size_y * sizeof(char *));
    for (int i = 0; i < new_labirint->size_y; i++) {
        new_labirint->map[i] = (char *)malloc(new_labirint->size_x * sizeof(char));
        memcpy(new_labirint->map[i], labirint->map[i], new_labirint->size_x * sizeof(char));
    }

    new_labirint->number_of_agents = labirint->number_of_agents;

    new_labirint->agents = (Agent *)malloc(new_labirint->number_of_agents * sizeof(Agent));
    memcpy(new_labirint->agents, labirint->agents, new_labirint->number_of_agents * sizeof(Agent));

    new_labirint->porhibited_positions = (Collision **)malloc(new_labirint->number_of_agents * sizeof(Collision *));
    
    for (int i = 0; i < new_labirint->number_of_agents; i++) {
        int num_por_moves = 0;
        while (!(labirint->porhibited_positions[i][num_por_moves].x == -1 && labirint->porhibited_positions[i][num_por_moves].y == -1)) {
            num_por_moves++;
        }

        new_labirint->porhibited_positions[i] = (Collision *)malloc((num_por_moves + 1) * sizeof(Collision));

        memcpy(new_labirint->porhibited_positions[i], labirint->porhibited_positions[i], (num_por_moves + 1) * sizeof(Agent));

        if (i == agent_index) {
            printf("number of por_pos for [%d]= %d\n", agent_index, num_por_moves + 1);
            new_labirint->porhibited_positions[i] = (Collision *)realloc(new_labirint->porhibited_positions[i], (num_por_moves + 2) * sizeof(Collision));
            new_labirint->porhibited_positions[i][num_por_moves] = collision;
            new_labirint->porhibited_positions[i][num_por_moves + 1] = (Collision){-1, -1, -1};
        }
    }

/////////////////////////

    new_labirint->agent_moves = (Agent **)malloc(new_labirint->number_of_agents * sizeof(Agent *));
    for (int i = 0; i < new_labirint->number_of_agents; i++) {
        int num_moves = 0;
        while (!(labirint->agent_moves[i][num_moves].x == -1 && labirint->agent_moves[i][num_moves].y == -1)) {
            num_moves++;
        }

        new_labirint->agent_moves[i] = (Agent *)malloc((num_moves + 1) * sizeof(Agent));

        memcpy(new_labirint->agent_moves[i], labirint->agent_moves[i], (num_moves + 1) * sizeof(Agent));
    }

    return new_labirint;
}

void print_porhibited_map(Labirint *labirint){
    for (int i = 0; i < labirint->size_y; i++) {
        for (int j = 0; j < labirint->size_x; j++) {
            char agent = 0;
            for (int k = 0; k < get_number_of_agents(labirint); k++) {
                for (int l = 0;labirint->porhibited_positions[k][l].x != -1; l++) {
                    if (labirint->porhibited_positions[k][l].x == j && labirint->porhibited_positions[k][l].y == i) {
                        set_colour(k + 1);
                        printf("%d", k + 1);
                        reset_colour();
                        agent = 1;
                    }
                }
            }
            if(!agent){
                if (labirint->map[i][j] == WALL) {
                    printf("%c", labirint->map[i][j]);
                } else if((labirint->map[i][j] < 0 && labirint->map[i][j] >= -10) || labirint->map[i][j] == SPACE){
                    printf(".");
                }
            }
        }
        printf("\n");
    }
}

Labirint end_labirint;

int play(Labirint *labirint){
    printf("Adress of labirint: %p\n", labirint);
    find_paths(labirint);
    sinc_moves(labirint);
    print_moves(labirint);

    print_porhibited_map(labirint);
    
    int moves_count = find_path_length(labirint, 0);
    for (int i = 0; i < moves_count; i++) {
        for (int j = 0; j < get_number_of_agents(labirint); j++) {
            for (int k = 0; k < get_number_of_agents(labirint); k++) {
                if (j != k) {
                    if (labirint->agent_moves[j][i].x == labirint->agent_moves[k][i].x && labirint->agent_moves[j][i].y == labirint->agent_moves[k][i].y) {
                        printf("Collision between agents %d and %d at (%d, %d)\n", j + 1, k + 1, labirint->agent_moves[j][i].x, labirint->agent_moves[j][i].y);
                        char collision_exists_left = 0;
                        char collision_exists_right = 0;
                        for (int l = 0; labirint->porhibited_positions[j][l].x!=-1 ; l++) {
                            if (labirint->porhibited_positions[j][l].x == labirint->agent_moves[j][i].x && labirint->porhibited_positions[j][l].y == labirint->agent_moves[j][i].y) {
                                collision_exists_left = 1;
                                printf("Collision exists in %d\n", j);
                            }
                        }
                        for (int l = 0; labirint->porhibited_positions[k][l].x!=-1 ; l++) {
                            if (labirint->porhibited_positions[k][l].x == labirint->agent_moves[k][i].x && labirint->porhibited_positions[k][l].y == labirint->agent_moves[k][i].y) {
                                collision_exists_right = 1;
                                printf("Collision exists in %d\n", k);
                            }
                        }

                        if(!(collision_exists_left || collision_exists_right)){
                            Labirint *new_labirint_right = copy_labirint(labirint, k, (Collision){labirint->agent_moves[k][i].x, labirint->agent_moves[k][i].y, i});
                            Labirint *new_labirint_left = copy_labirint(labirint, j, (Collision){labirint->agent_moves[j][i].x, labirint->agent_moves[j][i].y, i});
                            if(play(new_labirint_left) < play(new_labirint_right)){
                                labirint = copy_labirint(new_labirint_left, -1, (Collision){0,0,-1});
                                printf("-------------------- Chose left\n");
                            }else{
                                labirint = copy_labirint(new_labirint_right, -1, (Collision){0,0,-1});
                                printf("-------------------- Chose right\n");
                            }
                        }else{
                            return 1000;
                        }

                        
                    }
                    if(i>0 && labirint->agent_moves[j][i].x == labirint->agent_moves[k][i-1].x && labirint->agent_moves[j][i].y == labirint->agent_moves[k][i-1].y && labirint->agent_moves[k][i].x == labirint->agent_moves[j][i-1].x && labirint->agent_moves[k][i].y == labirint->agent_moves[j][i-1].y){
                        printf("Collision between agents %d and %d at (%d, %d)\n", j + 1, k + 1, labirint->agent_moves[j][i].x, labirint->agent_moves[j][i].y);
                        char collision_exists_left = 0;
                        char collision_exists_right = 0;
                        for (int l = 0; labirint->porhibited_positions[j][l].x!=-1 ; l++) {
                            if (labirint->porhibited_positions[j][l].x == labirint->agent_moves[j][i].x && labirint->porhibited_positions[j][l].y == labirint->agent_moves[j][i].y) {
                                collision_exists_left = 1;
                                printf("Collision exists in %d\n", j);
                            }
                        }
                        for (int l = 0; labirint->porhibited_positions[k][l].x!=-1 ; l++) {
                            if (labirint->porhibited_positions[k][l].x == labirint->agent_moves[k][i].x && labirint->porhibited_positions[k][l].y == labirint->agent_moves[k][i].y) {
                                collision_exists_right = 1;
                                printf("Collision exists in %d\n", k);
                            }
                        }

                        if(!(collision_exists_left || collision_exists_right)){
                            Labirint *new_labirint_right = copy_labirint(labirint, k, (Collision){labirint->agent_moves[k][i].x, labirint->agent_moves[k][i].y, i});
                            Labirint *new_labirint_left = copy_labirint(labirint, j, (Collision){labirint->agent_moves[j][i].x, labirint->agent_moves[j][i].y, i});
                            if(play(new_labirint_left) < play(new_labirint_right)){
                                labirint = copy_labirint(new_labirint_left, -1, (Collision){0,0, -1});
                                printf("-------------------- Chose left\n");
                            }else{
                                labirint = copy_labirint(new_labirint_right, -1, (Collision){0,0, -1});
                                printf("-------------------- Chose right\n");
                            }
                        }else{
                            return 1000;
                        }
                    }
                    
                }
            }
        }
    }

    print_moves(labirint);
    printf("Adress of labirint: %p\n", labirint);

    end_labirint = *labirint;

    return moves_count;
}

int main() {
    printf("Hello, World!\n");

    Labirint labirint;
    // create_labirint_from_file(&labirint, "labirints\\porhibited_move_next.txt");
    // create_labirint_from_file(&labirint, "labirints\\maluk_labirint.txt");
    // create_labirint_from_file(&labirint, "labirints\\labirint_uslovie.txt");
    create_labirint_from_file(&labirint, "labirints\\dulug_labirint.txt");
    printf("Labirint created\n");
    
    labirint_visualisation(&labirint, HASHS);

    printf("Number of agents: %d\n", get_number_of_agents(&labirint));

    int game_over = play(&labirint);

    print_moves(&end_labirint);
    visualisate_moves(&end_labirint);
    
    
    
    free_labirint(&labirint);
    printf("Game over in %d moves\n", game_over);
}
