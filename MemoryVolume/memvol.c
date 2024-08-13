#include "postgres.h"
#include "fmgr.h"
#include "executor/spi.h"
#include "utils/builtins.h"
#include "utils/numeric.h"

PG_MODULE_MAGIC;

// Функция для создания таблицы и вставки значений
PG_FUNCTION_INFO_V1(init_vlm_ref);

Datum
init_vlm_ref(PG_FUNCTION_ARGS)
{
    int ret;
    const char *create_table_query = "CREATE TABLE IF NOT EXISTS vlm_ref (name text, k numeric);";
    const char *insert_data_query = 
        "INSERT INTO vlm_ref VALUES "
        "('Бит',1), ('Байт',8), ('Килобайт', 8192),"
        "('Мегабайт',8388608), ('Гигабайт',8589934592),"
        "('Терабайт',8.796093e12), ('Петабайт',9.0071993e15);";

    SPI_connect();

    // Создание таблицы
    ret = SPI_exec(create_table_query, 0);
    if (ret < 0)
        elog(ERROR, "Failed to create vlm_ref table");

    // Вставка данных
    ret = SPI_exec(insert_data_query, 0);
    if (ret < 0)
        elog(ERROR, "Failed to insert data into vlm_ref");

    SPI_finish();

    PG_RETURN_VOID();
}

// Функция конвертации значений
PG_FUNCTION_INFO_V1(vlm2vlm);

Datum
vlm2vlm(PG_FUNCTION_ARGS)
{
    // Получаем аргументы функции
    Numeric value = PG_GETARG_NUMERIC(0);
    text *name_from = PG_GETARG_TEXT_PP(1);
    text *name_to = PG_GETARG_TEXT_PP(2);

    // Перевод в строки
    char *name_from_str = text_to_cstring(name_from);
    char *name_to_str = text_to_cstring(name_to);

    Numeric result;
    bool isnull;
    Datum from_k, to_k;

    SPI_connect();

    // Получаем коэффициент для name_from
    char query_from[256];
    snprintf(query_from, sizeof(query_from), 
             "SELECT k FROM vlm_ref WHERE name = '%s'", name_from_str);
    SPI_execute(query_from, true, 1);   // Запрос
    if (SPI_processed > 0)
    {   // Берем значения из полученного кортежа
        from_k = SPI_getbinval(SPI_tuptable->vals[0], SPI_tuptable->tupdesc, 1, &isnull);
    }
    else
    {
        SPI_finish();
        ereport(ERROR, (errmsg("Unit not found: %s", name_from_str)));
    }

    // Получаем коэффициент для name_to
    char query_to[256];
    snprintf(query_to, sizeof(query_to), 
             "SELECT k FROM vlm_ref WHERE name = '%s'", name_to_str);
    SPI_execute(query_to, true, 1);     // Запрос
    if (SPI_processed > 0)
    {   // Берем значения из полученного кортежа
        to_k = SPI_getbinval(SPI_tuptable->vals[0], SPI_tuptable->tupdesc, 1, &isnull);
    }
    else
    {
        SPI_finish();
        ereport(ERROR, (errmsg("Unit not found: %s", name_to_str)));
    }

    // Конвертация значения
    result = DatumGetNumeric(
        DirectFunctionCall2(numeric_div,
                            DirectFunctionCall2(numeric_mul, NumericGetDatum(value), from_k),
                            to_k)
    );

    SPI_finish();

    PG_RETURN_NUMERIC(result);
}
