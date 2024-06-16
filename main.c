#include "labirint.h"
#include "utils.h"
#include "queue.h"

int main() {
    Labirint labirint;
    create_labirint_from_file(&labirint, "labirint.txt");

    play(&labirint);

    free_labirint(&labirint);
    return 0;
}
