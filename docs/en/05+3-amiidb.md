#amiidb

## Home Page

* Browse…
* search…
* my collection…
* My data…
* set up…
* [quit]

## Browse game list

* The Legend of Zelda
* Animal Crossing: Friends
* Monster Hunter: Rise
* …
* [return]


## Amiibo List

* *Link
* Method
* Zelda
* Cannon
* [Previous page]
* [Next page]
* [return]

```
Remark:
If there is a large amount of Animal Crossing data, it will be displayed in separate pages.
(*) marked as favorites
How to check if it has been used on the same day?
```

## List menu

* collect…
* Save as…
* search…
* [return]


## Amiibo details
* `<01 ab:bc:cd:de:ef:f0:01>`
* Link
* Baoge climbing set

## Amiibo details menu

* Randomly generated
* Auto Random [On]
* Collect…
* Cancel favorites…
* Create new amiibo…
* Save as…
* [Back to list]
* [Return to menu]
* [Exit application]


## search

* [______________]
* Link/Legend of Zelda
* Link/riot
* [Return to menu]


## my collection
* Favorites 1
* Favorites 2
* [Return to menu]


## My favorites menu

* Create new favorites…
* Delete favorites…
* Clear favorites…
* [Return]

## Favorites list page

```
Same as amiibo list page
Click amiibo to jump to amiibo details page
```

## My data

* 00-link
* 01-8bitlink
* …
* 20-Empty data
* [Return to menu]

```
Note: 20 slots, save as you like, the number of slots can be modified in the settings
```

## set up

* Amiibo keys [Loaded]
* Data slot [20]
* [Return to menu]

## Database data format

db_header
----------------------------------
magic: 4 bytes fixed bytes: AMDB
version: 2 bytes
db_amiibo_offset: 4 bytes
db_game_offset: 4 bytes
db_link_offset: 4 bytes
string_pool_offset: 4 bytes
data_pool_offset: 4 bytes

db_amiibo
----------------------------------
record_header + record_data

record_header(4 bytes):
* version: 1 bytes
* flags: 1 bytes
* record_entry_size: 2 bytes
* record_cnt: 2 bytes

amiibo_record(14 bytes, fixed length):
* id: 2 bytes
* head: 4 bytes
* tail: 4 bytes
* name_en: 4 bytes (offset in string pool)
* name_cn: 4 bytes (offset in string pool)
--------------------------

db_game
--------------------------
record_header + record_data

record_header(4 bytes):
* version: 1 bytes
* flags: 1 bytes
* record_entry_size: 2 bytes
* record_cnt: 2 bytes

record_data(N * game_record):

game_record(18 bytes):
* id: 2 bytes
* parent_id: 2 bytes
* name_en: 4 bytes (offset in string pool)
* name_cn: 4 bytes (offset in string pool)
* link_cnt: 2 bytes
* link_data: 4 bytes begin index of link_record
--------------------------


db_link
--------------------------
record_header + record_data

record_header(4 bytes):
* version: 1 bytes
* flags: 1 bytes
* record_entry_size: 2 bytes
* record_cnt: 2 bytes

record_data(N * link_record):

link_record(16 bytes):
* game_id: 2 bytes
* amiibo_id: 2 bytes
* usage_en: 4 bytes (offset in string pool)
* usage_cn: 4 bytes (offset in string pool)
----------------------------

string_pool
----------------------------
record_header + record_data

record_header(4 bytes):
* version: 1 bytes
* flags: 1 bytes
* record_entry_size: 2 bytes: 0xFFFF NOT DEFINED
* record_cnt: 2 bytes

record_data(N * link_record):

pool_record(X bytes):
* length: 2 bytes
* data: X bytes + with trail \0 bytes
----------------------------

flags:
0000 0001: record db
0000 0010: string pool
0000 0100: data pool