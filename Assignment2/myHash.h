#define HASH_SIZE 100

int hash_size;

struct HASH {
    char topic[20];
    int start_index;
    int end_index;
};

struct HASH hashTable[HASH_SIZE];

void
init_hash() {
    hash_size = 0;
    
    for (int i = 0; i < HASH_SIZE; i++) {
        bzero(hashTable[i].topic, sizeof(hashTable[i].topic));
        hashTable[i].start_index = 0;
        hashTable[i].end_index = 0;
    }
}

void
insert_hash(char *topic, int current_counter) {
    int i = hash_size;
    ++hash_size;

    strcpy(hashTable[i].topic, topic);
    hashTable[i].end_index = current_counter;
    printf("Inserting %d %s\n", i, topic);
}

void
update_insert_hash(char *topic, int insert_counter) {
    int LIMIT = hash_size;

    for (int i = 0; i < LIMIT; i++)
        if (hashTable[i].topic)
            if (strcmp(hashTable[i].topic, topic) == 0) {
                hashTable[i].end_index = insert_counter;
                break;
            }
}

void
update_delete_hash(char *topic, int deleted_counter) {
    int LIMIT = hash_size;

    for (int i = 0; i < LIMIT; i++)
        if (hashTable[i].topic)
            if (strcmp(hashTable[i].topic, topic) == 0) {
                hashTable[i].start_index = deleted_counter;
                break;
            }
}

int
get_counter_hash(char *topic) {
    int LIMIT = hash_size;

    for (int i = 0; i < LIMIT; i++)
        if (hashTable[i].topic)
            if (strcmp(hashTable[i].topic, topic) == 0)
                return hashTable[i].end_index;
    
    return -1;
}

void
print_hash() {
    int LIMIT = hash_size;
    
    for (int i = 0; i < LIMIT; i++)
        printf("%d \t %d \t %s\n", hashTable[i].start_index, hashTable[i].end_index, hashTable[i].topic);
}

int
get_start_counter_hash(char *topic) {
    int LIMIT = hash_size;

    for (int i = 0; i < LIMIT; i++)
        if (hashTable[i].topic)
            if (strcmp(hashTable[i].topic, topic) == 0)
                return hashTable[i].start_index;
    
    return -1;
}