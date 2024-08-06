/* rtn--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION rtn" to load this file. \quit

-- Function for Roman to Arabic
CREATE FUNCTION rtn(text) RETURNS integer
AS 'MODULE_PATHNAME', 'rtn'
LANGUAGE C STRICT;

-- Function for Arabic to Roman
CREATE FUNCTION ntr(integer) RETURNS text
AS 'MODULE_PATHNAME', 'ntr'
LANGUAGE C IMMUTABLE;