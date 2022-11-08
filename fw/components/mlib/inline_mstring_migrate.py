#!/usr/bin/env python3

input_header_path = "m-string.h.old"
output_header_path = "m-string.h"
output_source_path = "m-string.c"

def migrate():
    EOL = "\n"

    input_header = open(input_header_path, "r")
    output_header = open(output_header_path, "w")
    output_source = open(output_source_path, "w")

    # write header include to output source
    output_source.write('#include "m-string.h"' + EOL)
    output_source.write(EOL)

    # undefine some macros, which has same name as functions
    undef_list = [
        "m_string_init_set",
        "m_string_set",
        "m_string_cat",
        "m_string_cmp",
        "m_string_equal_p",
        "m_string_strcoll",
        "m_string_search",
    ]

    for undef in undef_list:
        output_source.write("#undef " + undef + EOL)
    output_source.write(EOL)

    while True:
        line = input_header.readline()
        if not line:
            break

        # skip "#if M_USE_FAST_STRING_CONV == 0" block
        if line.startswith("#if M_USE_FAST_STRING_CONV == 0"):
            output_header.write(line)
            while not line.startswith("#endif"):
                line = input_header.readline()
                output_header.write(line)
            continue

        # if current line starts with "static"
        if line.startswith("static"):
            fn_header = line
            line = input_header.readline()

            # and next line starts with "m_string_" or "m_str1ng_"
            if line.startswith("m_string_") or line.startswith("m_str1ng_"):
                # read function header
                fn_header += line
                while fn_header.count('(') != fn_header.count(')'):
                    line = input_header.readline()
                    fn_header += line

                # read function body
                fn_body = ""
                while not line.startswith("}"):
                    line = input_header.readline()
                    fn_body += line
                    
                # remove "static inline" from function header
                fn_header = fn_header.replace("static inline", "")
                fn_header = fn_header.replace("static", "")
                fn_header = fn_header.replace("\r", "")
                fn_header = fn_header.replace("\n", " ")
                fn_header = fn_header.strip()
                
                # write function body to output source
                output_source.write(fn_header)
                output_source.write(fn_body + EOL)

                # write function definition with C++ wrapper to output header
                output_header.write('#ifdef __cplusplus'+EOL+'extern "C" {'+EOL+'#endif'+EOL)
                output_header.write(fn_header + ";\r\n")
                output_header.write('#ifdef __cplusplus'+EOL+'}'+EOL+'#endif'+EOL)
            else:
                output_header.write(fn_header)
                output_header.write(line)

        else:
            output_header.write(line)


if __name__ == "__main__":
    migrate()