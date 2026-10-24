/* Smart Lost and Found Matching System - DSA Mini Project
 * Author: Kanav Mundra
 * Data structures: hash table (chaining), queue, linked list, stack
 * Compile: gcc -std=c11 -Wall -Wextra -pedantic lost_found.c -o lost_found
 * Run: ./lost_found
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define TABLE_SIZE 11
#define LEN 50
#define THRESHOLD 65

typedef struct Item {
    int id;
    char name[LEN], category[LEN], color[LEN], location[LEN];
    struct Item *next;
} Item;

typedef struct Match {
    int lostId, foundId, score;
    struct Match *next;
} Match;

static Item *foundTable[TABLE_SIZE];
static Item *lostFront = NULL, *lostRear = NULL;
static Match *historyTop = NULL;
static int nextId = 1;

static void lower(char *s) {
    for (; *s; ++s) *s = (char)tolower((unsigned char)*s);
}

static int readLine(const char *prompt, char *buf, size_t size) {
    printf("%s", prompt);
    if (!fgets(buf, (int)size, stdin)) return 0;
    size_t len = strlen(buf);
    if (len && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
    }
    return 1;
}

static int readInt(const char *prompt, int *value) {
    char line[64], *end;
    if (!readLine(prompt, line, sizeof line)) return 0;
    long n = strtol(line, &end, 10);
    while (isspace((unsigned char)*end)) ++end;
    if (end == line || *end != '\0' || n < INT_MIN || n > INT_MAX) {
        puts("Please enter a valid number.");
        return -1;
    }
    *value = (int)n;
    return 1;
}

static int hash(const char *key) {
    unsigned long h = 0;
    while (*key) h = (h * 31U + (unsigned char)*key++) % TABLE_SIZE;
    return (int)h;
}

static Item *createItem(void) {
    Item *it = calloc(1, sizeof *it);
    if (!it) {
        fputs("Error: unable to allocate memory.\n", stderr);
        return NULL;
    }
    it->id = nextId++;
    if (!readLine("Item name: ", it->name, sizeof it->name) ||
        !readLine("Category (phone/wallet/bag/keys/...): ", it->category, sizeof it->category) ||
        !readLine("Color: ", it->color, sizeof it->color) ||
        !readLine("Location: ", it->location, sizeof it->location)) {
        free(it);
        return NULL;
    }
    lower(it->name);
    lower(it->category);
    lower(it->color);
    lower(it->location);
    return it;
}

static void insertFound(Item *it) {
    int h = hash(it->category);
    it->next = foundTable[h];
    foundTable[h] = it;
}

static void enqueueLost(Item *it) {
    it->next = NULL;
    if (!lostRear) lostFront = lostRear = it;
    else {
        lostRear->next = it;
        lostRear = it;
    }
}

static void pushMatch(int l, int f, int s) {
    Match *m = malloc(sizeof *m);
    if (!m) {
        fputs("Warning: match history could not be saved (out of memory).\n", stderr);
        return;
    }
    m->lostId = l;
    m->foundId = f;
    m->score = s;
    m->next = historyTop;
    historyTop = m;
}

static void printItem(const Item *p) {
    printf("  #%d  %s | %s | %s | %s\n", p->id, p->name, p->category, p->color, p->location);
}

static void display(void) {
    puts("\n--- Pending LOST reports (queue) ---");
    for (Item *p = lostFront; p; p = p->next) printItem(p);
    puts("--- Unclaimed FOUND items (hash table) ---");
    for (int i = 0; i < TABLE_SIZE; ++i)
        for (Item *p = foundTable[i]; p; p = p->next) printItem(p);
}

static void showHistory(void) {
    puts("\n--- Match history (latest first) ---");
    if (!historyTop) puts("  No matches yet.");
    for (Match *m = historyTop; m; m = m->next)
        printf("  Lost #%d <-> Found #%d (score %d%%)\n", m->lostId, m->foundId, m->score);
}

static int score(const Item *a, const Item *b) {
    if (strcmp(a->category, b->category) != 0) return 0;
    int s = 40;
    if (strcmp(a->color, b->color) == 0) s += 25;
    if (strcmp(a->location, b->location) == 0) s += 25;
    if (strstr(a->name, b->name) || strstr(b->name, a->name)) s += 10;
    return s;
}

static Item *bestFound(const Item *lost, int *best) {
    Item *res = NULL;
    *best = 0;
    for (Item *p = foundTable[hash(lost->category)]; p; p = p->next) {
        int s = score(lost, p);
        if (s > *best) { *best = s; res = p; }
    }
    return res;
}

static Item *bestLost(const Item *found, int *best) {
    Item *res = NULL;
    *best = 0;
    for (Item *p = lostFront; p; p = p->next) {
        int s = score(found, p);
        if (s > *best) { *best = s; res = p; }
    }
    return res;
}

static void removeFound(Item *target) {
    Item **pp = &foundTable[hash(target->category)];
    while (*pp && *pp != target) pp = &(*pp)->next;
    if (*pp) *pp = target->next;
}

static void removeLost(Item *target) {
    Item *prev = NULL, *p = lostFront;
    while (p && p != target) { prev = p; p = p->next; }
    if (!p) return;
    if (prev) prev->next = p->next;
    else lostFront = p->next;
    if (p == lostRear) lostRear = prev;
}

static void reportLost(void) {
    Item *lost = createItem();
    if (!lost) return;
    int s;
    Item *found = bestFound(lost, &s);
    if (found && s >= THRESHOLD) {
        removeFound(found);
        pushMatch(lost->id, found->id, s);
        printf("\nMATCH! Lost #%d <-> Found #%d (score %d%%)\nCollect from: %s\n",
               lost->id, found->id, s, found->location);
        free(found);
        free(lost);
    } else {
        enqueueLost(lost);
        printf("\nNo match yet. Lost report #%d queued.\n", lost->id);
    }
}

static void reportFound(void) {
    Item *found = createItem();
    if (!found) return;
    int s;
    Item *lost = bestLost(found, &s);
    if (lost && s >= THRESHOLD) {
        removeLost(lost);
        pushMatch(lost->id, found->id, s);
        printf("\nMATCH! Lost #%d <-> Found #%d (score %d%%)\nOwner reported location: %s\n",
               lost->id, found->id, s, lost->location);
        free(lost);
        free(found);
    } else {
        insertFound(found);
        printf("\nNo match yet. Found item #%d stored.\n", found->id);
    }
}

static void searchCategory(void) {
    char category[LEN];
    int any = 0;
    if (!readLine("Category to search: ", category, sizeof category)) return;
    lower(category);
    for (Item *p = foundTable[hash(category)]; p; p = p->next)
        if (strcmp(p->category, category) == 0) { printItem(p); any = 1; }
    if (!any) puts("  No found items in this category.");
}

static void cleanup(void) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        Item *p = foundTable[i];
        while (p) { Item *next = p->next; free(p); p = next; }
        foundTable[i] = NULL;
    }
    Item *p = lostFront;
    while (p) { Item *next = p->next; free(p); p = next; }
    lostFront = lostRear = NULL;
    while (historyTop) {
        Match *next = historyTop->next;
        free(historyTop);
        historyTop = next;
    }
}

int main(void) {
    puts("Smart Lost and Found Matching System");
    for (;;) {
        puts("\n1. Report LOST item\n2. Report FOUND item\n3. Search found items by category"
             "\n4. Show pending lists\n5. Match history\n0. Exit");
        int ch, status = readInt("Choice: ", &ch);
        if (status == 0) break;
        if (status < 0) continue;
        switch (ch) {
            case 1: reportLost(); break;
            case 2: reportFound(); break;
            case 3: searchCategory(); break;
            case 4: display(); break;
            case 5: showHistory(); break;
            case 0: puts("Goodbye!"); cleanup(); return 0;
            default: puts("Invalid choice.");
        }
    }
    cleanup();
    return 0;
}
