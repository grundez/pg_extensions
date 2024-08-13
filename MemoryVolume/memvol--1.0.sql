\echo Use "CREATE EXTENSION memvol" to load this file. \quit

COMMENT ON EXTENSION memvol IS 'Единицы измерения объема памяти. memvol--1.0.sql';

CREATE FUNCTION init_vlm_ref() 
RETURNS void AS 'MODULE_PATHNAME', 'init_vlm_ref' 
LANGUAGE C;
SELECT init_vlm_ref();

CREATE FUNCTION vlm2vlm(numeric, text, text) 
RETURNS numeric AS 'MODULE_PATHNAME', 'vlm2vlm' 
LANGUAGE C;

-- Необходимо для использования функции любым пользователем
GRANT SELECT ON vlm_ref TO public;