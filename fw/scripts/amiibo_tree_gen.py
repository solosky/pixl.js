import os 
import struct
import csv

root = "../data/Amiibo Bins"

class Amiibo:
    def __init__(self):
        self.id = None
        self.name_en = None
        self.name_cn = None

class Game:
    def __init__(self):
        self.id = None
        self.parent_id = None
        self.name_en = None
        self.name_cn = None
        self.order = None

class Link:
    def __init__(self):
        self.game_id = None
        self.amiibo_id = None
        self.note_en = None
        self.note_cn = None
        self.name_it = None

def get_prorject_directory():
    return os.path.abspath(os.path.dirname(__file__)+"/../")


def read_amiibo_from_csv():
    csv_file = get_prorject_directory() + "/data/amiidb_amiibo.csv"
    if not os.path.exists(csv_file):
        return list()
    amiibos = list()
    with open(csv_file, "r", encoding="utf8") as f:
        for r in csv.reader(f):
            amiibo = Amiibo()
            amiibo.id = r[0]
            amiibo.name_en = r[1]
            amiibo.name_cn = r[2]
            amiibos.append(amiibo)
    
    return amiibos

def write_amiibo_to_csv(amiibos):
    csv_file = get_prorject_directory() + "/data/amiidb_amiibo.csv"
    with open(csv_file, "w", encoding="utf8", newline="") as f:
        w = csv.writer(f)
        for amiibo in amiibos:
            r = list()
            r.append(amiibo.id)
            r.append(amiibo.name_en)
            r.append(amiibo.name_cn)
            w.writerow(r)

def read_games_from_csv():
    csv_file = get_prorject_directory() + "/data/amiidb_game.csv"
    if not os.path.exists(csv_file):
        return list()
    games = list()
    with open(csv_file, "r", encoding="utf8") as f:
        for r in csv.reader(f):
            game = Game()
            game.id = r[0]
            game.parent_id = r[1]
            game.name_en = r[2]
            game.name_cn = r[3]
            game.order = r[4]
            games.append(game)
    return games

def read_link_from_csv():
    csv_file = get_prorject_directory() + "/data/amiidb_link.csv"
    if not os.path.exists(csv_file):
        return list()
    links = list()
    with open(csv_file, "r", encoding="utf8") as f:
        for r in csv.reader(f):
            link = Link()
            link.game_id = r[0]
            link.amiibo_id = r[1]
            link.note_en = r[2]
            link.note_cn = r[3]
            link.note_it = r[4]
            links.append(link)
    return links


def write_link_to_csv(links):
    csv_file = get_prorject_directory() + "/data/amiidb_link.csv"
    with open(csv_file, "w", encoding="utf8", newline="") as f:
        w = csv.writer(f)
        for link in links:
            r = list()
            r.append(link.game_id)
            r.append(link.amiibo_id)
            r.append(link.note_en)
            r.append(link.note_cn)
            r.append(link.note_it)
            w.writerow(r)



amiibo_list = []
games = []
links = []

def update_amiibo_name(amiibo_id, amiibo_name):
    for amiibo in amiibo_list:
        if amiibo.id == amiibo_id:
            amiibo.name_en = amiibo_name
            amiibo.name_cn = amiibo_name
            break


def read_amiibo_id(filename):
    file = open(filename, "rb")
    content = file.read()
    file.close()
    if(len(content) == 540):
        head = struct.unpack(">I", bytes(content[84:88]))[0]
        tail = struct.unpack(">I", bytes(content[88:92]))[0]
        return ("%08x%08x" % ( head, tail))
    else:
        return ""
    
def add_ac_link(amiibo_id, game_name):
    for game in games:
        if game.name_en == game_name:
            link = Link()
            link.game_id = game.id
            link.amiibo_id = amiibo_id
            link.note_cn = ""
            link.note_en = ""
            link.note_it = ""
            links.append(link)



def traverse_dir(path):
    for dir, dirs, files in os.walk(path):
        for file in files:
            short_dir = dir[len(root) + 1:]
            amiibo_name = file[0:-4]
            amiibo_id = read_amiibo_id(dir + "/" + file)
            if short_dir.find("Animal Crossing Amiibo") >= 0:
                update_amiibo_name(amiibo_id, amiibo_name)
                dir_name = os.path.basename(short_dir)
                add_ac_link(amiibo_id, dir_name)
                print(dir_name, ",",amiibo_name,",", amiibo_id)


def remove_ac_link():
    new_links = []
    global links
    for link in links:
        if link.game_id != "11":
            new_links.append(link)
    links = new_links


def link_sort_key(link):
    for amiibo in amiibo_list:
        if link.amiibo_id == amiibo.id:
            return ("%02d:%s")% (int(link.game_id) , amiibo.name_en)
    return ("%02d:%s")% (link.game_id , "")

def sort_link():
    links.sort(key=lambda x: link_sort_key(x) )


amiibo_list = read_amiibo_from_csv()
games = read_games_from_csv()
links = read_link_from_csv()

remove_ac_link()

traverse_dir("../data/Amiibo Bins")

sort_link()

write_amiibo_to_csv(amiibo_list)
write_link_to_csv(links)