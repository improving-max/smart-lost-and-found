# Smart Lost and Found Management System

A menu-driven C application that records lost and found items and suggests matches using a simple weighted similarity score. This Data Structures and Algorithms (DSA) mini project demonstrates a hash table, queue, linked lists, and stack.

**Author:** Kanav Mundra

## Features

- Report a lost item and compare it with currently stored found items.
- Report a found item and compare it with pending lost reports.
- Search unclaimed found items by category.
- Display pending lost reports and unclaimed found items.
- View match history, with the newest match first.
- Case-insensitive text handling for item descriptions.

## Data structures

| Structure | Purpose |
| --- | --- |
| Hash table with separate chaining | Stores unclaimed found items by category |
| Queue-style linked list | Tracks pending lost reports |
| Linked lists | Links items and match records |
| Stack | Keeps match history in latest-first order |

## Matching score

Items must have the same category. The score is calculated as follows:

| Criterion | Points |
| --- | ---: |
| Same category | 40 |
| Same color | 25 |
| Same location | 25 |
| One item name contains the other | 10 |

A match is accepted when the best score is at least **65%**. This is an educational heuristic, not proof of ownership. Confirm an item's ownership through the relevant lost-and-found staff before returning it.

## Requirements

- A C compiler such as GCC or Clang
- A terminal or command prompt
- `make` is optional

## Build and run

### Linux / macOS

```sh
gcc -std=c11 -Wall -Wextra -pedantic lost_found.c -o lost_found
./lost_found
```

Or, if Make is installed:

```sh
make
./lost_found
```

### Windows (MinGW-w64)

```sh
gcc -std=c11 -Wall -Wextra -pedantic lost_found.c -o lost_found.exe
lost_found.exe
```

## How to use

Run the program and select a menu option:

1. Report LOST item.
2. Report FOUND item.
3. Search found items by category.
4. Show pending lists.
5. Display match history.
0. Exit.

Enter an item name, category, color, and location when prompted. Text fields are normalized to lowercase to make matching case-insensitive.

## Limitations

- Data is stored in memory only and is lost when the program exits.
- Matching uses exact category, color, and location comparisons, plus substring matching for names.
- The program is intended for learning and demonstration; it does not verify ownership or provide a persistent database.

## License

This project is distributed under the MIT License. See [LICENSE](LICENSE).
