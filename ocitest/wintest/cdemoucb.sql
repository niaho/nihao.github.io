rem
rem $Header: rdbms/demo/cdemoucb.sql /st_rdbms_11.2.0/1 2011/03/21 11:43:53 rajsahoo Exp $
rem
rem cdemoucb.sql
rem
rem Copyright (c) 1998, 2011, Oracle and/or its affiliates. 
rem All rights reserved. 
rem
rem    NAME
rem      cdemoucb.sql - <one-line expansion of the name>
rem
rem    DESCRIPTION
rem      <short description of component this file declares/defines>
rem
rem    NOTES
rem      <other useful comments, qualifications, etc.>
rem
rem    MODIFIED   (MM/DD/YY)
rem    rajsahoo    03/14/11 - XbranchMerge rajsahoo_bug-10005724 from main
rem    azhao       04/10/07 - case sensitive password
rem    hdnguyen    11/14/00 - fixed connect internal
rem    mjaeger     07/14/99 - bug 808870: OCCS: convert tabs, no long lines
rem    bgoyal      10/16/98 - Sql for cdemoucb.c
rem    bgoyal      10/16/98 - Created
rem
connect sys/change_on_install as sysdba;
drop user cdemoucb cascade;
grant connect, resource to cdemoucb identified by CDEMOUCB;

