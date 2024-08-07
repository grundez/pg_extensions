#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/memutils.h"
#include "postgres_fe.h"
#include "lib/stringinfo.h"
#include "utils/guc.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(rtn);
PG_FUNCTION_INFO_V1(ntr);

Datum rtn(PG_FUNCTION_ARGS);
Datum ntr(PG_FUNCTION_ARGS);

static int min_value;
static int max_value;

void _PG_init(void);

void
_PG_init(void)
{
    DefineCustomIntVariable(
        "rtn.min_value",
        "Minimum value for conversion.",
        "Sets the minimum value for Roman numeral conversion.",
        &min_value,
        1,
        1,
        10000,
        PGC_USERSET,
        0,
        NULL,
        NULL,
        NULL
    );

    DefineCustomIntVariable(
        "rtn.max_value",
        "Maximum value for conversion.",
        "Sets the maximum value for Roman numeral conversion.",
        &max_value,
        3999,
        1,
        10000,
        PGC_USERSET,
        0,
        NULL,
        NULL,
        NULL
    );
}

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

    if (result < min_value || result > max_value) {
        ereport(ERROR,
                (errmsg("Input value %d is out of the allowed range [%d, %d]",
                        result, min_value, max_value)));
    }

    PG_RETURN_INT32(result);
}

Datum
ntr(PG_FUNCTION_ARGS)
{
    int number = PG_GETARG_INT32(0);

    if (number < min_value || number > max_value) {
        ereport(ERROR,
                (errmsg("Input value %d is out of the allowed range [%d, %d]",
                        number, min_value, max_value)));
    }

    // Объявление всех переменных в начале функции
    const char *roman_numerals[] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};
    int values[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
    StringInfoData buf;

    initStringInfo(&buf);

    for (int i = 0; i < 13; i++) {
        while (number >= values[i]) {
            appendStringInfoString(&buf, roman_numerals[i]);
            number -= values[i];
        }
    }
    PG_RETURN_TEXT_P(cstring_to_text(buf.data));
}