#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/memutils.h"
#include "postgres_fe.h"
#include "lib/stringinfo.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(rtn);
PG_FUNCTION_INFO_V1(ntr);

Datum rtn(PG_FUNCTION_ARGS);
Datum ntr(PG_FUNCTION_ARGS);

Datum
rtn(PG_FUNCTION_ARGS)
{
    text *roman_text = PG_GETARG_TEXT_P(0);
    char *roman_str = VARDATA(roman_text);
    int len = VARSIZE(roman_text) - VARHDRSZ;
    int result = 0;

    int values[256];
    memset(values, 0, sizeof(values));

    values['I'] = 1;
    values['V'] = 5;
    values['X'] = 10;
    values['L'] = 50;
    values['C'] = 100;
    values['D'] = 500;
    values['M'] = 1000;

    for (int i = 0; i < len; i++) {
        if (i + 1 < len && values[(unsigned char)roman_str[i]] < values[(unsigned char)roman_str[i + 1]]) {
            result -= values[(unsigned char)roman_str[i]];
        } else {
            result += values[(unsigned char)roman_str[i]];
        }
    }

    PG_RETURN_INT32(result);
}

Datum
ntr(PG_FUNCTION_ARGS)
{
    int number = PG_GETARG_INT32(0);

    // Arrays of Roman numerals and their values
    const char *roman_numerals[] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};
    int values[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
    
    // StringInfoData is a structure used for building strings in PostgreSQL
    StringInfoData buf;
    initStringInfo(&buf);

    if (number <= 0 || number >= 4000) { // Limits for numbers
        PG_RETURN_NULL();
    }

    for (int i = 0; i < 13; i++) {
        while (number >= values[i]) {
            appendStringInfoString(&buf, roman_numerals[i]);
            number -= values[i];
        }
    }
    PG_RETURN_TEXT_P(cstring_to_text(buf.data));
}
