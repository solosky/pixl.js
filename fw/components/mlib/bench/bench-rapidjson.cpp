#define NDEBUG
#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/encodedstream.h"
#include "rapidjson/error/en.h"

#include <stdio.h>
#include <string>
#include <vector>

#include "common.h"

using namespace std;

/********************************************************************************************/

vector<string> vstring;
vector<string> vstring2;

static void
bench_vector_string_init(size_t n)
{
    for(size_t i = 0; i < n; i++)
    {
        char buff[100];
        snprintf(buff, sizeof(buff), "%zu", i);
        std::string s(buff);
        vstring.push_back(s);
    }
}

static void
bench_vector_string_init_big(size_t n)
{
    for(size_t i = 0; i < n; i++)
    {
        char buff[100];
        snprintf(buff, sizeof(buff), "%zu%zu%zu%zu%zu%zu", i, i, i, i, i, i);
        std::string s(buff);
        vstring.push_back(s);
    }
}

static void
bench_vector_string_json_run(size_t n)
{
    if (vstring.size() != n) {
        printf("Size are different!\n");
        abort();
    }

    FILE *f = fopen("tmp-serial.json", "wt");
    if (!f) abort();
    {
        // Prepare writer and output stream.
        char writeBuffer[65536];
        typedef rapidjson::AutoUTFOutputStream<unsigned, rapidjson::FileWriteStream> OutputStream; 
        rapidjson::FileWriteStream os(f, writeBuffer, sizeof(writeBuffer));
        OutputStream eos(os, rapidjson::kUTF8, false); 
        rapidjson::PrettyWriter<OutputStream, rapidjson::UTF8<>, rapidjson::AutoUTF<unsigned> > writer(eos);

        writer.StartArray();
        for(size_t i = 0; i < n; i++){
            writer.String(vstring[i]);
        }
        writer.EndArray();
    }
    fclose(f);
    
    f = fopen("tmp-serial.json", "rt");
    if (!f) abort();
    {
        char readBuffer[65536];
        rapidjson::FileReadStream is(f, readBuffer, sizeof(readBuffer));
        rapidjson::Document d;
        d.ParseStream(is);
        for(size_t i = 0; i < d.Size(); i++) {
            rapidjson::Value& s = d[i];
            vstring2.push_back(s.GetString());
        }
    }
    fclose(f);

    if (vstring2.size() != n) {
        printf("Size are different!\n");
        abort();
    }
}

static void
bench_vector_string_clear(void)
{
    if (vstring != vstring2) {
        printf("Array are different!\n");
        abort();
    }
}

/********************************************************************************************/

vector<unsigned long> vulong;
vector<unsigned long> vulong2;

static void
bench_vector_ulong_init(size_t n)
{
    for(size_t i = 0; i < n; i++)
    {
      vulong.push_back(i*i);
    }
}

static void
bench_vector_ulong_json_run(size_t n)
{
    if (vulong.size() != n) {
        printf("Size are different!\n");
        abort();
    }

    FILE *f = fopen("tmp-serial.json", "wt");
    if (!f) abort();
    {
        // Prepare writer and output stream.
        char writeBuffer[65536];
        typedef rapidjson::AutoUTFOutputStream<unsigned, rapidjson::FileWriteStream> OutputStream; 
        rapidjson::FileWriteStream os(f, writeBuffer, sizeof(writeBuffer));
        OutputStream eos(os, rapidjson::kUTF8, false); 
        rapidjson::PrettyWriter<OutputStream, rapidjson::UTF8<>, rapidjson::AutoUTF<unsigned> > writer(eos);

        writer.StartArray();
        for(size_t i = 0; i < n; i++){
            writer.Int64(vulong[i]);
        }
        writer.EndArray();
    }
    fclose(f);
    
    f = fopen("tmp-serial.json", "rt");
    if (!f) abort();
    {
        char readBuffer[65536];
        rapidjson::FileReadStream is(f, readBuffer, sizeof(readBuffer));
        rapidjson::Document d;
        d.ParseStream(is);
        for(size_t i = 0; i < d.Size(); i++) {
            rapidjson::Value& s = d[i];
            vulong2.push_back(s.GetInt64());
        }
    }
    fclose(f);

    if (vulong2.size() != n) {
        printf("Size are different!\n");
        abort();
    }
}

static void
bench_vector_ulong_clear(void)
{
    if (vulong != vulong2) {
        printf("Array are different!\n");
        abort();
    }
}

/********************************************************************************************/

const config_func_t table[] = {
  {110,    "serial-json STR", 10000000, bench_vector_string_init, bench_vector_string_json_run, bench_vector_string_clear},
  {111,    "serial-json STR.big", 10000000, bench_vector_string_init_big, bench_vector_string_json_run, bench_vector_string_clear},
  {112,    "serial-json INT", 10000000, bench_vector_ulong_init, bench_vector_ulong_json_run, bench_vector_ulong_clear}
};

int main(int argc, const char *argv[])
{
  test("RAPIDJSON", numberof(table), table, argc, argv);
  exit(0);
}
