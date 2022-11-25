# download latest amiibo data and merge to amiibo_data.csv


from urllib.request import urlopen
import json
import os
import csv


	# uint32_t head;
	# uint32_t tail;
	# const char* amiibo_series;
	# const char* game_series;
	# const char* character;
	# const char* name;
	# const char* notes;

class Amiibo:
    def __init__(self):
        self.head = None
        self.tail = None
        self.name = None
        self.character = None
        self.game_series = None
        self.amiibo_series = None
        self.notes = None

def get_prorject_directory():
    return os.path.abspath(os.path.dirname(__file__)+"/../")


def fetch_amiibo_from_api():
    conn = urlopen("https://www.amiiboapi.com/api/amiibo/")
    body = json.loads(conn.read())
    amiibos = list()
    for ami in body["amiibo"]: 
        amiibo = Amiibo()
        amiibo.head = ami["head"]
        amiibo.tail = ami["tail"]
        amiibo.amiibo_series = ami["amiiboSeries"]
        amiibo.game_series = ami["gameSeries"]
        amiibo.character = ami["character"]
        amiibo.name = ami["name"]
        amiibos.append(amiibo)
    return amiibos


def read_amiibo_from_csv():
    csv_file = get_prorject_directory() + "/data/amiibo_data.csv"
    if not os.path.exists(csv_file):
        return list()
    amiibos = list()
    with open(csv_file, "r", encoding="utf8") as f:
        for r in csv.reader(f):
            amiibo = Amiibo()
            amiibo.head = r[0]
            amiibo.tail = r[1]
            amiibo.amiibo_series = r[2]
            amiibo.game_series = r[3]
            amiibo.character = r[4]
            amiibo.name = r[5]
            amiibo.notes = r[6]
            amiibos.append(amiibo)
    
    return amiibos


def write_amiibo_to_csv(amiibos):
    csv_file = get_prorject_directory() + "/data/amiibo_data.csv"
    with open(csv_file, "w", encoding="utf8", newline="") as f:
        w = csv.writer(f)
        for amiibo in amiibos:
            r = list()
            r.append(amiibo.head)
            r.append(amiibo.tail)
            r.append(amiibo.amiibo_series)
            r.append(amiibo.game_series)
            r.append(amiibo.character)
            r.append(amiibo.name)
            r.append(amiibo.notes)
            w.writerow(r)


def merge_amiibo(amiibos_csv, amiibos_api):
    amiibos_merged = dict()
    for amiibo in amiibos_csv:
        amiibos_merged[amiibo.head +"," + amiibo.tail] = amiibo

    for amiibo in amiibos_api:
        key = amiibo.head +"," + amiibo.tail
        if amiibos_merged.get(key) == None:
            amiibos_merged[key] = amiibo
            print("Found new amiibo: [%s] %s/%s " % (key, amiibo.amiibo_series, amiibo.name))
    amiibos = list()
    for k in amiibos_merged:
        amiibos.append(amiibos_merged[k])
    return amiibos

def gen_amiibo_data_c_file(amiibos):
    c_file = get_prorject_directory() + "/src/amiibo_data.c"
    with open(c_file, "w+", newline="\n", encoding="utf8") as f:
        f.write('#include "amiibo_data.h"\n')
        f.write('const amiibo_data_t amiibo_data[] = {\n')
        for amiibo in amiibos:
            f.write('{0x%s, 0x%s, "%s", "%s", "%s", "%s", "%s"}, \n' % (amiibo.head, amiibo.tail,
             amiibo.amiibo_series, amiibo.game_series, amiibo.character, amiibo.name, 
             amiibo.notes))
        f.write("{0, 0, 0, 0, 0, 0}\n")
        f.write("};\n")



amiibos_api = fetch_amiibo_from_api()
amiibos_csv = read_amiibo_from_csv()
amiibos_merged = merge_amiibo(amiibos_csv, amiibos_api)
write_amiibo_to_csv(amiibos_merged)
print("Found %d amiibo records." % len(amiibos_merged))
gen_amiibo_data_c_file(amiibos_merged)
