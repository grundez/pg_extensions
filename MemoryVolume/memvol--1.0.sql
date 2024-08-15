\echo Use "CREATE EXTENSION memvol" to load this file. \quit

COMMENT ON EXTENSION memvol IS 'Единицы измерения объема памяти. memvol--1.0.sql';

-- Функция инициализации и заполнения таблицы значений
CREATE FUNCTION init_vlm_ref() 
RETURNS void AS 'MODULE_PATHNAME', 'init_vlm_ref' 
LANGUAGE C;
SELECT init_vlm_ref();

-- Функция перевода величин
CREATE FUNCTION vlm2vlm(numeric, text, text) 
RETURNS numeric AS 'MODULE_PATHNAME', 'vlm2vlm' 
LANGUAGE C;

-- Функция для отображения содержимого таблицы vlm_ref
CREATE FUNCTION display_vlm_ref()
    RETURNS SETOF vlm_ref
    AS 'MODULE_PATHNAME', 'display_vlm_ref'
    LANGUAGE C STRICT;

-- Необходимо для использования функции любым пользователем
GRANT SELECT ON vlm_ref TO public;