#include "odbc.h"

/*
@type    : ODBC 1.0 API
@purpose : returns the list of table, catalog, or schema names, and
table types, stored in a specific data source. The driver
returns the information as a result set
*/

static char *last_qry_str=NULL;

SQLRETURN SQL_API SQLTables(SQLHSTMT hstmt,
							SQLCHAR FAR *sztab_qualifier,
							SQLSMALLINT cbtab_qualifier,
							SQLCHAR FAR *sztab_owner,SQLSMALLINT cbtab_owner,
							SQLCHAR FAR *sztab_name,SQLSMALLINT cbtab_name,
							SQLCHAR FAR *sztab_type,SQLSMALLINT cbtab_type)
{
	char Qualifier_buff[NAME_LEN + 1],
		Owner_buff[NAME_LEN + 1],
		Name_buff[NAME_LEN + 1],
		Type_buff[32],
		*tab_qualifier,
		*tab_owner,
		*tab_name,
		*tab_type,
		*db_name;
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;
	bool  all_dbs = true;
  	char  sql_str[1024];
	char  where_str[128];
	bool  b_use_where = false;

	DBUG_ENTER("SQLTables");
	SQLFreeStmt(hstmt,MYSQL_RESET);
	tab_qualifier=fix_str((char FAR *) Qualifier_buff,(char*)sztab_qualifier,
			 cbtab_qualifier);
	tab_owner=	fix_str((char FAR*) Owner_buff, (char*)sztab_owner,cbtab_owner);
	tab_name =	fix_str((char FAR*) Name_buff, (char*)sztab_name,cbtab_name);
	tab_type =  fix_str((char FAR *)Type_buff, (char*)sztab_type,cbtab_type);
	db_name  =  stmt->p_svctx->p_server->db_name;
	/*全部表及视*/
	if (!tab_qualifier[0] || (!strcmp(tab_qualifier,"%")) || 
		!(_casecmp(tab_qualifier,"SQL_ALL_CATALOGS",16)) || 
		!(_casecmp(tab_qualifier,db_name,min(strlen(tab_qualifier),strlen(db_name)))))
	{
		where_str[0]=0x0;
		if(tab_owner[0] &&strcmp(tab_owner,"%"))
		{
			sprintf(where_str," and u.name like '%s'",tab_owner);
			b_use_where = true;
		}

		if(tab_name[0] && strcmp(tab_name,"%"))
		{
			if(b_use_where)
				strcat(where_str, " and r.name like '" );
			else
				strcat(where_str," and r.name like '");
			strcat(where_str,tab_name);
			strcat(where_str,"'");
			b_use_where = true;
		}
	
		if(tab_type[0] && !stricmp(tab_type,"TABLE"))
			sprintf(sql_str,"select '%s' as \"TABLE_CAT\",s.schema_name as \"TABLE_SCHEM\","
			"r.table_name as \"TABLE_NAME\",'TABLE' as \"TABLE_TYPE\",' ' as \"REMARKS\" "
			"from all_tables r,all_users u,all_schemas s  "
			"where r.user_id=u.user_id and r.schema_id=s.schema_id %s",db_name,where_str);
		else if(tab_type[0] && !stricmp(tab_type,"VIEW"))
			sprintf(sql_str,"select '%s' as \"TABLE_CAT\",s.schema_name as \"TABLE_SCHEM\","
			"r.view_name as \"TABLE_NAME\",'VIEW' as \"TABLE_TYPE\",' ' as \"REMARKS\" "
			"from all_views r,all_users u,all_schemas s "
			"where r.user_id=u.user_id and r.schema_id=s.schema_id  %s", db_name,where_str);
		else
			sprintf(sql_str,"select '%s' as \"TABLE_CAT\",s.schema_name as \"TABLE_SCHEM\","
			"r.table_name as \"TABLE_NAME\",'TABLE' as \"TABLE_TYPE\",' ' as \"REMARKS\" "
			"from all_tables r,all_users u,all_schemas s  "
			"where r.user_id=u.user_id and r.schema_id=s.schema_id %s ",
			"union all select '%s' as \"TABLE_CAT\",s.schema_name as \"TABLE_SCHEM\","
			"r.view_name as \"TABLE_NAME\",'VIEW' as \"TABLE_TYPE\",' ' as \"REMARKS\" "
			"from all_views r,all_users u,all_schemas s "
			"where r.user_id=u.user_id and r.schema_id=s.schema_id  %s",
			db_name,where_str, db_name,where_str);
		
		return nativeCataQuery(stmt,sql_str);
	}
	else 
	{
		sprintf(sql_str,"select '%s' as \"TABLE_CAT\",s.schema_name as \"TABLE_SCHEM\","
			"r.table_name as \"TABLE_NAME\",'TABLE' as \"TABLE_TYPE\",' ' as \"REMARKS\" "
			"from all_tables limit 0");
		return nativeCataQuery(stmt,sql_str);
	}
}

KCIFld  sql_column_attrs[18] = {
		{NULL,NULL,"TABLE_CAT",			NULL,	TYPE_CHAR,	64,	0,0},
		{NULL,NULL,"TABLE_SCHEM",		NULL,	TYPE_CHAR,	64,	0,0},
		{NULL,NULL,"TABLE_NAME",		NULL,	TYPE_CHAR,	64,	0,0},
		{NULL,NULL,"COLUMN_NAME",		NULL,	TYPE_CHAR,	64,	0,0},
		{NULL,NULL,"DATA_TYPE",			NULL,	TYPE_I2,	2,	0,0},
		{NULL,NULL,"TYPE_NAME",			NULL,	TYPE_CHAR,	20,	0,0},
		{NULL,NULL,"COLUMN_SIZE",		NULL,	TYPE_I4,	4,	0,0},
		{NULL,NULL,"BUFFER_LENGTH",		NULL,	TYPE_I4,	12,	0,0},
		{NULL,NULL,"DECIMAL_DIGITS",	NULL,	TYPE_I2,	2,	0,0},
		{NULL,NULL,"NUM_PREC_RADIX",	NULL,	TYPE_I2,	2,	0,0},
		{NULL,NULL,"NULLABLE",			NULL,	TYPE_I2,	2,	0,0},
		{NULL,NULL,"REMARKS",			NULL,	TYPE_CHAR,	20,	0,0},
		{NULL,NULL,"COLUMN_DEF",		NULL,	TYPE_CHAR,  20,	0,0},
		{NULL,NULL,"SQL_DATA_TYPE",		NULL,	TYPE_I2,	2,	0,0},
		{NULL,NULL,"SQL_DATETIME_SUB",	NULL,	TYPE_I2,	2,	0,0},
		{NULL,NULL,"CHAR_OCTET_LENGTH",	NULL,	TYPE_I4,	4,	0,0},
		{NULL,NULL,"ORDINAL_POSITION",	NULL,	TYPE_I2,	2,	0,0},
		{NULL,NULL,"IS_NULLABLE",		NULL,	TYPE_CHAR,  2,	0,0},
};


/*
@type    : ODBC 1.0 API
@purpose : returns the list of column names in specified tables.
The driver returns this information as a result set on the
specified StatementHandle
*/

SQLRETURN SQL_API SQLColumns(SQLHSTMT hstmt,
							 SQLCHAR FAR *sztab_qualifier,
							 SQLSMALLINT cbtab_qualifier,
							 SQLCHAR FAR *sztab_owner,
							 SQLSMALLINT cbtab_owner,
							 SQLCHAR FAR *sztab_name, 
							 SQLSMALLINT cbtab_name,
							 SQLCHAR FAR *szColumnName,
							 SQLSMALLINT cbColumnName)
{
	char Qualifier_buff[NAME_LEN+1],
		Owner_buff[NAME_LEN+1],
		Name_buff[NAME_LEN+1],
   		*tab_qualifier,
		*tab_owner,
		*tab_name,
		*ColName;
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;
	SQLRETURN	ret;
	bool all_dbs = true;
  	char  sql_str[3000];
	char  where_str[128];
	char  type_str[1024];
	bool  b_use_where = false;

	DBUG_ENTER("SQLTables");
	SQLFreeStmt(hstmt,MYSQL_RESET);
	tab_qualifier=fix_str((char FAR *) Qualifier_buff,sztab_qualifier,
			 cbtab_qualifier);
	tab_owner = fix_str((char FAR*) Owner_buff,sztab_owner,cbtab_owner);
	tab_name = fix_str((char FAR*) Name_buff,sztab_name,cbtab_name);
	ColName = fix_str((char FAR*) Name_buff,szColumnName,cbColumnName);

	/*全部表及视*/
	if (!tab_qualifier[0] || (!strcmp(tab_qualifier,"%")) || 
		!(_casecmp(tab_qualifier,"SQL_ALL_CATALOGS",16)) || 
		!(_casecmp(tab_qualifier,stmt->dbc->db_name,min(strlen(tab_qualifier),strlen(stmt->dbc->db_name)))))
	{
		where_str[0]=0x0;
		if(tab_owner[0] &&strcmp(tab_owner,"%"))
		{
			sprintf(where_str," and \"TABLE_SCHEM\" = '%s'",tab_owner);
			b_use_where = true;
		}

		if(tab_name[0] && strcmp(tab_name,"%"))
		{
			strcat(where_str, " and  \"TABLE_NAME\" = '" );
			strcat(where_str,tab_name);
			strcat(where_str,"'");
			b_use_where = true;
		}

		if(ColName[0] && strcmp(ColName,"%"))
		{
			if(b_use_where)
				strcat(where_str, " and  \"COLUMN_NAME\" = '" );
			else
				strcat(where_str," and \"COLUMN_NAME\" = '");
			strcat(where_str,ColName);
			strcat(where_str,"'");
			b_use_where = true;
		}
	
		sprintf(type_str,"(case a.type_name when 'bool' then %d when 'char' then %d when 'varchar' then %d when 'tiny_int' then %d "
			"when 'smallint' then %d when 'integer' then %d when 'bigint' then %d when 'float' then %d when 'double' then %d "
			"when 'numeric' then %d when 'date' then %d when 'time' then %d  when 'datetime' then %d when 'timestamp' then %d "
			"when 'binary' then %d when 'blob' then %d when 'image' then %d end )",
			SQL_CHAR,SQL_VARCHAR,SQL_CHAR,SQL_TINYINT,
			SQL_SMALLINT,SQL_INTEGER,SQL_BIGINT,SQL_FLOAT,SQL_DOUBLE,
			SQL_NUMERIC,SQL_DATE,SQL_TIME,SQL_DATETIME,SQL_TIMESTAMP,
			SQL_VARBINARY,SQL_LONGVARBINARY,SQL_LONGVARBINARY);
		sprintf(sql_str,
			"select '%s' as \"TABLE_CAT\","
			"s.name as \"TABLE_SCHEM\","
			"t.name as \"TABLE_NAME\","
			"a.name as \"COLUMN_NAME\","
			"%s ::smallint as \"DATA_TYPE\","
			"a.type_name as \"TYPE_NAME\","
			"(case a.type_name when 'char'then a.scale when 'varchar' then  a.scale when 'numeric' then a.scale/65536::integer else 32 end)::integer as \"COLUMN_SIZE\","
			"(case a.type_name when 'char'then a.scale when 'varchar' then  a.scale when 'numeric' then a.scale/65536::integer else 32 end)::integer as \"BUFFER_LENGTH\","
			"(case a.type_name when 'numeric' then a.scale%65536  end)::smallint as \"DECIMAL_DIGITS\","
			"(case a.type_name when 'float' then 2 when 'double' then 2 when 'numeric' then 10  end)::smallint as \" NUM_PREC_RADIX\","
			"(case a.not_null when 't' then %d else %d  end)::smallint as \"NULLABLE\","
			" ' ' as \"REMARKS\","
			"a.\"def_val\" as \"COLUMN_DEF\","
			"%s :: smallint as  \"SQL_DATA_TYPE\","
			" 0 ::smallint as \"SQL_DATETIME_SUB\","
			"(case a.type_name when 'char'then a.scale when 'varchar' then  a.scale else 32 end)::integer as \"CHAR_OCTET_LENGTH\","
			"(a.field_no+1)::integer as \"ORDINAL_POSITION\","
			"(case a.not_null when 't' then 'NO' else 'YES'  end) as \"IS_NULLABLE\" "
			"from sys_tables t,sys_schemas s,sys_columns a where t.id=a.table_id and t.schema_id = s.id %s order by t.name,a.field_no",
			stmt->dbc->db_name,
			type_str,
			SQL_NO_NULLS,
			SQL_NULLABLE,
			type_str,
			where_str);
		
		ret = nativeCataQuery(stmt,sql_str);
		if(ret!=SQL_SUCCESS && ret!=SQL_SUCCESS_WITH_INFO || stmt->row_num==0)
		{
			SQLSTMT	*temp_stmt;
			int			i;
			SQLRETURN	ret1;

			/*将表名看作视名,用select * from view_name limit 0*/
			sprintf(sql_str,"select  * from \"%s\" limit 0",tab_name);
			if(rh_SQLAllocStmt(stmt->dbc,&temp_stmt)!= SQL_SUCCESS)
				return SQL_ERROR;
			ret1 = nativeCataQuery(temp_stmt,sql_str);
			if(ret1==SQL_SUCCESS || ret1==SQL_SUCCESS_WITH_INFO)
			{
				freeResult(stmt);
				stmt->col_infos = sql_column_attrs;
				stmt->field_num = 18;
				stmt->rowset_off = 0;
				stmt->fetch_off  =0;

				/*将字段描述信息转化为记录*/
				for(i=0;i<temp_stmt->field_num;i++)
				{
					int type;
					ulong	len,size,preci,display_size; 
					char *rec_slot[18];
					char  type_id_str[20],
						type_name_str[30],
						scale_str[20],
						dis_len_str[20],
						pre_len_str[20];

					type = getFieldInfo(temp_stmt,&(temp_stmt->col_infos[i]),type_name_str,&len,&size,&preci,&display_size);
					_itoa(type,type_id_str,10);
					_itoa(size,scale_str,10);
					_itoa(preci,pre_len_str,10);
					_itoa(display_size,dis_len_str,10);

					rec_slot[0] = stmt->dbc->db_name;
					rec_slot[1] = "";
					rec_slot[2] = tab_name;
					rec_slot[3]= temp_stmt->col_infos[i].name;
					rec_slot[4] = type_id_str;
					rec_slot[5] = type_name_str;
					rec_slot[6] = scale_str;
					rec_slot[7] = dis_len_str;
					rec_slot[8] = pre_len_str;
					rec_slot[9] = "10";
					rec_slot[10] = "1";
					rec_slot[11] = "";
					rec_slot[12] = "";
					rec_slot[13] = type_id_str;
					rec_slot[14] = "0";
					rec_slot[15] = "20";
					rec_slot[16] = " ";
					rec_slot[17] = "Y";
					appendRecord(stmt,rec_slot);
				}
				SQLFreeStmt(temp_stmt,0);
				/*将预定义记录转入result*/
				stmt->cursor_pos=1;
				stmt->have_srv_cursor=false;
				stmt->col_info_is_const=true;
				stmt->curr_is_over =true;
				stmt->all_is_over = true;
				stmt->state = ST_EXECUTED;
				DBUG_RETURN_STATUS(SQL_SUCCESS);
			}
			else
			{
				SQLFreeStmt(temp_stmt,0);
				return ret;
			}
		}
		else
			return ret;
	}
	else 
	{
		sprintf(sql_str,
			"select  ' ' as \"TABLE_CAT\","
			"' ' as \"TABLE_SCHEM\","
			"' ' as \"TABLE_NAME\","
			"' ' as \"COLUMN_NAME\","
			"0 as \"DATA_TYPE\","
			"' ' as \"TYPE_NAME\","
			"0 as \"COLUMN_SIZE\","
			"0 as \"BUFFER_LENGTH\","
			"0 as \"DECIMAL_DIGITS\","
			"0 as \" NUM_PREC_RADIX\","
			"0 as \"NULLABLE\","
			" ' ' as \"REMARKS\","
			" ' ' as \"COLUMN_DEF\","
			"0 as  \"SQL_DATA_TYPE\","
			" 0 as \"SQL_DATETIME_SUB\","
			"0 as \"CHAR_OCTET_LENGTH\","
			"0 as \"ORDINAL_POSITION\","
			"' ' as \"IS_NULLABLE\" "
			"from sys_tables limit 0");
		
		return  nativeCataQuery(stmt,sql_str); //SQLExecDirect((SQLHSTMT)stmt,sql_str,strlen(sql_str));
	}
}


/* Have DBMS set up result set of Statistics. */
/*
@type    : ODBC 1.0 API
@purpose : retrieves a list of statistics about a single table and the
indexes associated with the table. The driver returns the
information as a result set.
*/

SQLRETURN SQL_API SQLStatistics(SQLHSTMT hstmt,
								SQLCHAR FAR *sztab_qualifier,
								SQLSMALLINT cbtab_qualifier,
								SQLCHAR FAR *sztab_owner,
								SQLSMALLINT cbtab_owner,
								SQLCHAR FAR *sztab_name,
								SQLSMALLINT cbtab_name,
								SQLUSMALLINT fUnique,
								SQLUSMALLINT fAccuracy)
{
	char Qualifier_buff[NAME_LEN+1],
		Owner_buff[NAME_LEN+1],
		Name_buff[NAME_LEN+1],
		*tab_qualifier,
		*tab_owner,
		*tab_name;
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;
	bool all_dbs = true;
  	char  sql_str[3000];
	char  where_str[512];
	bool  b_use_where = false;

	DBUG_ENTER("SQLStatistics");
	SQLFreeStmt(hstmt,MYSQL_RESET);
	tab_qualifier=fix_str((char FAR *) Qualifier_buff,sztab_qualifier,
			 cbtab_qualifier);
	tab_owner=	 fix_str((char FAR*) Owner_buff,sztab_owner,cbtab_owner);
	tab_name=	 fix_str((char FAR*) Name_buff,sztab_name,cbtab_name);


	/*全部表及视*/
	if (!tab_qualifier[0] || (!strcmp(tab_qualifier,"%")) || 
		!(_casecmp(tab_qualifier,"SQL_ALL_CATALOGS",16)) || 
		!(_casecmp(tab_qualifier,stmt->dbc->db_name,min(strlen(tab_qualifier),strlen(stmt->dbc->db_name)))))
	{
		where_str[0]=0x0;
		if(tab_owner[0] &&strcmp(tab_owner,"%"))
		{
			sprintf(where_str," and u.name like '%s' ",tab_owner);
			b_use_where = true;
		}

		if(tab_name[0] && strcmp(tab_name,"%"))
		{
			strcat(where_str, " and r.name like '" );
			strcat(where_str,tab_name);
			strcat(where_str,"'");
			b_use_where = true;
		}

		if(fUnique == SQL_INDEX_UNIQUE)
		{
			strcat(where_str," and (i.is_primary='t' or i.is_unique='t')");
		}
	
		sprintf(sql_str,"select 'china_city' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\","
			"(case when i.is_primary='t' or i.is_unique='t' then %d else %d end) as \"NON_UNIQUE\","
			"r.name as \"INDEX_QUALIFIER\",i.name as \"INDEX_NAME\","
			"(case i.index_type when 0 then %d when 1 then %d when 2 then %d else %d end)::smallint as \"TYPE\","
			"1 ::smallint as \"ORDINAL_POSITION\","
			"i.keys as \"COLUMN_NAME\",'A' as \"ASC_OR_DESC\",NULL::integer as \"CARDINALITY\",NULL ::integer as \"PAGES\","
			"NULL::integer as \"FILTER_CONDITION\" from sys_tables r,sys_users u,sys_schemas s ,sys_indexes i "
			"where r.id=i.table_id and r.user_id=u.id and r.schema_id=s.id  %s",
			SQL_FALSE,
			SQL_INDEX_CLUSTERED,
			SQL_INDEX_OTHER,
			SQL_INDEX_OTHER,
			SQL_INDEX_OTHER,
			SQL_TRUE,
			where_str);
		
		return  nativeCataQuery(stmt,sql_str);//SQLExecDirect((SQLHSTMT)stmt,sql_str,strlen(sql_str));
	}
	else 
	{
		sprintf(sql_str,"select ' ' as \"TABLE_CAT\",'' as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\",'table' as \"TABLE_TYPE\","
			" ' ' as \"REMARKS\" from sys_tables  limit 0");
		return  nativeCataQuery(stmt,sql_str);//SQLExecDirect((SQLHSTMT)stmt,sql_str,strlen(sql_str));
	}
}


/*
@type    : ODBC 1.0 API
@purpose : returns a list of tables and the privileges associated with
each table. The driver returns the information as a result
set on the specified statement.
*/

SQLRETURN SQL_API SQLTablePrivileges(SQLHSTMT hstmt,
									 SQLCHAR FAR *sztab_qualifier,
									 SQLSMALLINT cbtab_qualifier,
									 SQLCHAR FAR *sztab_owner,
									 SQLSMALLINT cbtab_owner,
									 SQLCHAR FAR *sztab_name,
									 SQLSMALLINT cbtab_name)
{
	char Qualifier_buff[NAME_LEN+1],
		Owner_buff[NAME_LEN+1],
		Name_buff[NAME_LEN+1],
		*tab_qualifier,
		*tab_owner,
		*tab_name;
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;
	bool all_dbs = true;
  	char  sql_str[4096];
	char  where_str[128];

	DBUG_ENTER("SQLTablePrivileges");
	SQLFreeStmt(hstmt,MYSQL_RESET);
	tab_qualifier=fix_str((char FAR *) Qualifier_buff,sztab_qualifier,
			 cbtab_qualifier);
	tab_owner=	 fix_str((char FAR*) Owner_buff,sztab_owner,cbtab_owner);
	tab_name=	 fix_str((char FAR*) Name_buff,sztab_name,cbtab_name);	

	/*全部表及视*/
	if (!tab_qualifier[0] || (!strcmp(tab_qualifier,"%")) || 
		!(_casecmp(tab_qualifier,"SQL_ALL_CATALOGS",16)) || 
		!(_casecmp(tab_qualifier,stmt->dbc->db_name,min(strlen(tab_qualifier),strlen(stmt->dbc->db_name)))))
	{
		where_str[0]=0x0;
		if(tab_owner[0] &&strcmp(tab_owner,"%"))
			sprintf(where_str," and \"TABLE_SCHEM\" like '%s'",tab_owner);

		if(tab_name[0] && strcmp(tab_name,"%"))
		{
			strcat(where_str, " and \"TABLE_NAME\" like '" );
			strcat(where_str,tab_name);
			strcat(where_str,"'");
		}
	
		sprintf(sql_str,"select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\","
			" (case u1.name when null then '_SYSTEM' else u1.name end)as \"GRANTOR\",u2.name as \"GRANTEE\","
			" 'SELECT' as \"PRIVILEGE\" ,"
			" (case when bit_and(a.regrant,1)>0 then 'YES' else 'NO' end) as \"IS_GRANTABLE\" " 
			" from sys_tables r,sys_schemas s,sys_acls a left join sys_users u1 on a.grantor_id=u1.id ,sys_users u2 "
			" where r.id=a.obj_id and r.schema_id=s.id and a.grantee_id=u2.id and bit_and(a.authority,1)>0 %s"
			" union "
			" select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\","
			" (case u1.name when null then '_SYSTEM' else u1.name end)as \"GRANTOR\",u2.name as \"GRANTEE\","
			" 'UPDATE' as \"PRIVILEGE\" ,"
			" (case when bit_and(a.regrant,2)>0 then 'YES' else 'NO' end) as \"IS_GRANTABLE\" " 
			" from sys_tables r,sys_schemas s sys_acls a left join sys_users u1 on a.grantor_id=u1.id ,sys_users u2 "
			" where r.id=a.obj_id  and r.schema_id=s.id and a.grantee_id=u2.id and bit_and(a.authority,2)>0 %s"
			" union "
			" select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\","
			" (case u1.name when null then '_SYSTEM' else u1.name end)as \"GRANTOR\",u2.name as \"GRANTEE\","
			" 'INSERT' as \"PRIVILEGE\" ,"
			" (case when bit_and(a.regrant,4)>0 then 'YES' else 'NO' end) as \"IS_GRANTABLE\" " 
			" from sys_tables r,sys_schemas s,sys_acls a left join sys_users u1 on a.grantor_id=u1.id ,sys_users u2 "
			" where r.id=a.obj_id  and r.schema_id=s.id and a.grantee_id=u2.id and bit_and(a.authority,4)>0 %s"
			" union "
			" select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\","
			" (case u1.name when null then '_SYSTEM' else u1.name end)as \"GRANTOR\",u2.name as \"GRANTEE\","
			" 'DELETE' as \"PRIVILEGE\" ,"
			" (case when bit_and(a.regrant,8)>0 then 'YES' else 'NO' end) as \"IS_GRANTABLE\" " 
			" from sys_tables r,sys_schemas s,sys_acls a left join sys_users u1 on a.grantor_id=u1.id ,sys_users u2 "
			" where r.id=a.obj_id  and r.schema_id=s.id and a.grantee_id=u2.id and bit_and(a.authority,8)>0 %s"
			" union "
			" select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\","
			" (case u1.name when null then '_SYSTEM' else u1.name end)as \"GRANTOR\",u2.name as \"GRANTEE\","
			" 'REFERENCES' as \"PRIVILEGE\" ,"
			" (case when bit_and(a.regrant,16)>0 then 'YES' else 'NO' end) as \"IS_GRANTABLE\" " 
			" from sys_tables r,sys_schemas s,sys_acls a left join sys_users u1 on a.grantor_id=u1.id ,sys_users u2 "
			" where r.id=a.obj_id and r.schema_id=s.id and a.grantee_id=u2.id and bit_and(a.authority,16)>0 %s"
			" order by \"TABLE_NAME\"",

			stmt->dbc->db_name,
			where_str,
			stmt->dbc->db_name,
			where_str,
			stmt->dbc->db_name,
			where_str,
			stmt->dbc->db_name,
			where_str,
			stmt->dbc->db_name,
			where_str);
		
		return  nativeCataQuery(stmt,sql_str);//SQLExecDirect((SQLHSTMT)stmt,sql_str,strlen(sql_str));
	}
	else 
	{
		sprintf(sql_str,"select '' as \"TABLE_CAT\",'' as \"TABLE_SCHEM\",'' as \"TABLE_NAME\","
			" ''as \"GRANTOR\",'' as \"GRANTEE\","
			" '' as \"PRIVILEGE\" ,"
			" '' as \"IS_GRANTABLE\" " 
			" from sys_tables limit 0");
		return  nativeCataQuery(stmt,sql_str);//SQLExecDirect((SQLHSTMT)stmt,sql_str,strlen(sql_str));
	}

}

/*
@type    : ODBC 1.0 API
@purpose : returns a list of columns and associated privileges for the
specified table. The driver returns the information as a result
set on the specified StatementHandle.
*/

SQLRETURN SQL_API SQLColumnPrivileges(SQLHSTMT hstmt,
									  SQLCHAR FAR *sztab_qualifier,
									  SQLSMALLINT cbtab_qualifier,
									  SQLCHAR FAR *sztab_owner,
									  SQLSMALLINT cbtab_owner,
									  SQLCHAR FAR *sztab_name,
									  SQLSMALLINT cbtab_name,
									  SQLCHAR FAR *szColumnName,
									  SQLSMALLINT cbColumnName)
{ 

	char Qualifier_buff[NAME_LEN+1],
	Owner_buff[NAME_LEN+1],
	Name_buff[NAME_LEN+1],
  	*tab_qualifier,
	*tab_owner,
	*tab_name,
	*ColName;
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;
	bool all_dbs = true;
  	char  sql_str[4096];
	char  where_str[128];
	bool  b_use_where = false;

	DBUG_ENTER("SQLColumnPrivileges");
	SQLFreeStmt(hstmt,MYSQL_RESET);
	tab_qualifier=fix_str((char FAR *) Qualifier_buff,sztab_qualifier,
			 cbtab_qualifier);
	tab_owner=	 fix_str((char FAR*) Owner_buff,sztab_owner,cbtab_owner);
	tab_name=	 fix_str((char FAR*) Name_buff,sztab_name,cbtab_name);
	ColName = fix_str((char FAR*) Name_buff,szColumnName,cbColumnName);

	/*全部表及视*/
	if (!tab_qualifier[0] || (!strcmp(tab_qualifier,"%")) || 
		!(_casecmp(tab_qualifier,"SQL_ALL_CATALOGS",16)) || 
		!(_casecmp(tab_qualifier,stmt->dbc->db_name,min(strlen(tab_qualifier),strlen(stmt->dbc->db_name)))))
	{
		where_str[0]=0x0;
		if(tab_owner[0] &&strcmp(tab_owner,"%"))
			sprintf(where_str," and \"TABLE_SCHEM\" = '%s'",tab_owner);

		if(tab_name[0] && strcmp(tab_name,"%"))
		{
			strcat(where_str, " and \"TABLE_NAME\" = '" );
			strcat(where_str,tab_name);
			strcat(where_str,"'");
		}

		if(ColName[0] && strcmp(ColName,"%"))
		{
			strcat(where_str, " and \"COLUMN_NAME\" like '" );
			strcat(where_str,ColName);
			strcat(where_str,"'");
		}

	
		sprintf(sql_str,
			"select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\","
			"f.name  as \"COLUMN_NAME\",(case u1.name when null then '_SYSTEM' else u1.name end) as \"GRANTOR\","
			"u2.name as \"GRANTEE\",'SELECT' as \"PRIVILEGE\" ,"
			"(case when bit_and(a.regrant,1)>0 then 'YES' else 'NO' end) as \"IS_GRANTABLE\" "
			"from sys_tables r,sys_schemas s, sys_acls a left join sys_users u1 on a.grantor_id=u1.id,"
			"sys_users u2,sys_columns f  where ((r.id=a.obj_id and a.obj_type=3) "
			"or (f.field_no+r.id*1024)=a.obj_id and a.obj_type=4) and a.grantee_id=u2.id and "
			"f.table_id=r.id and r.schema_id=s.id and bit_and(a.authority,1)>0 %s"
			" union "
			"select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\","
			"f.name  as \"COLUMN_NAME\",(case u1.name when null then '_SYSTEM' else u1.name end) as \"GRANTOR\","
			"u2.name as \"GRANTEE\",'INSERT' as \"PRIVILEGE\" ,"
			"(case when bit_and(a.regrant,2)>0 then 'YES' else 'NO' end) as \"IS_GRANTABLE\" "
			"from sys_tables r,sys_schemas s,sys_acls a left join sys_users u1 on a.grantor_id=u1.id,"
			"sys_users u2,sys_columns f  where ((r.id=a.obj_id and a.obj_type=3) "
			"or (f.field_no+r.id*1024)=a.obj_id and a.obj_type=4) and a.grantee_id=u2.id and "
			"f.table_id=r.id and r.schema_id=s.id and bit_and(a.authority,2)>0 %s"
			" union "
			"select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\","
			"f.name  as \"COLUMN_NAME\",(case u1.name when null then '_SYSTEM' else u1.name end) as \"GRANTOR\","
			"u2.name as \"GRANTEE\",'UPDATE' as \"PRIVILEGE\" ,"
			"(case when bit_and(a.regrant,4)>0 then 'YES' else 'NO' end) as \"IS_GRANTABLE\" "
			"from sys_tables r,sys_schemas s,sys_acls a left join sys_users u1 on a.grantor_id=u1.id,"
			"sys_users u2,sys_columns f  where ((r.id=a.obj_id and a.obj_type=3) "
			"or (f.field_no+r.id*1024)=a.obj_id and a.obj_type=4) and a.grantee_id=u2.id and "
			"f.table_id=r.id and r.schema_id=s.id and bit_and(a.authority,4)>0 %s"
			" union "
			"select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\","
			"f.name  as \"COLUMN_NAME\",(case u1.name when null then '_SYSTEM' else u1.name end) as \"GRANTOR\","
			"u2.name as \"GRANTEE\",'DELETE' as \"PRIVILEGE\" ,"
			"(case when bit_and(a.regrant,8)>0 then 'YES' else 'NO' end) as \"IS_GRANTABLE\" "
			"from sys_tables r,sys_schemas s,sys_acls a left join sys_users u1 on a.grantor_id=u1.id,"
			"sys_users u2,sys_columns f  where ((r.id=a.obj_id and a.obj_type=3) "
			"or (f.field_no+r.id*1024)=a.obj_id and a.obj_type=4) and a.grantee_id=u2.id and "
			"f.table_id=r.id and r.schema_id=s.id and bit_and(a.authority,8)>0 %s"
			" union "
			"select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\","
			"f.name  as \"COLUMN_NAME\",(case u1.name when null then '_SYSTEM' else u1.name end) as \"GRANTOR\","
			"u2.name as \"GRANTEE\",'REFERENCES' as \"PRIVILEGE\" ,"
			"(case when bit_and(a.regrant,16)>0 then 'YES' else 'NO' end) as \"IS_GRANTABLE\" "
			"from sys_tables r,sys_schemas s,sys_acls a left join sys_users u1 on a.grantor_id=u1.id,"
			"sys_users u2,sys_columns f  where ((r.id=a.obj_id and a.obj_type=3) "
			"or (f.field_no+r.id*1024)=a.obj_id and a.obj_type=4) and a.grantee_id=u2.id and "
			"f.table_id=r.id and r.schema_id=s.id and bit_and(a.authority,16)>0 %s"
			" order by \"COLUMN_NAME\"",
			stmt->dbc->db_name,
			where_str,
			stmt->dbc->db_name,
			where_str,
			stmt->dbc->db_name,
			where_str,
			stmt->dbc->db_name,
			where_str,
			stmt->dbc->db_name,
			where_str);
		
		return  nativeCataQuery(stmt,sql_str);//SQLExecDirect((SQLHSTMT)stmt,sql_str,strlen(sql_str));
	}
	else 
	{
		sprintf(sql_str,"select '' as \"TABLE_CAT\",'' as \"TABLE_SCHEM\",'' as \"TABLE_NAME\","
			" ''as \"GRANTOR\",'' as \"GRANTEE\","
			" '' as \"PRIVILEGE\" ,"
			" '' as \"IS_GRANTABLE\" " 
			" from sys_tables limit 0");
		return  nativeCataQuery(stmt,sql_str);//SQLExecDirect((SQLHSTMT)stmt,sql_str,strlen(sql_str));
	}
}


/*
@type    : ODBC 1.0 API
@purpose : retrieves the following information about columns within a
specified table:
- The optimal set of columns that uniquely identifies a row
in the table.
- Columns that are automatically updated when any value in the
row is updated by a transaction
*/
extern char sql_bigint[6];
extern char sql_integer[6];
char *pseudo_field_infos[2][8]=
{
	{"2","rowid",sql_bigint,"bigint","20","20","0","2"},
	{"1","lock",sql_integer,"integer","10","10","0","2"},
};

KCIFld  pseudo_attrs[8] = {
		{NULL,NULL,"SCOPE",			NULL,TYPE_I2,		SQL_C_SHORT,	2,0,0},
		{NULL,NULL,"COLUMN_NAME",	NULL,TYPE_VARCHAR,	SQL_C_CHAR,		8,0,sizeof(int16)},
		{NULL,NULL,"DATA_TYPE",		NULL,TYPE_I2,		SQL_C_SHORT,	2,0,sizeof(PtrField)+sizeof(int16)},
		{NULL,NULL,"TYPE_NAME",		NULL,TYPE_VARCHAR,	SQL_C_CHAR,		8,0,sizeof(PtrField)+sizeof(int16)*2},
		{NULL,NULL,"COLUMN_SIZE",	NULL,TYPE_I4,		SQL_C_LONG,		4,0,sizeof(PtrField)*2+sizeof(int16)*2},
		{NULL,NULL,"BUFFER_LENGTH",	NULL,TYPE_I4,		SQL_C_LONG,		4,0,sizeof(PtrField)*2+sizeof(int16)*2+sizeof(int32)},
		{NULL,NULL,"DECIMAL_DIGITS",	NULL,TYPE_I2,		SQL_C_SHORT,	2,0,sizeof(PtrField)*2+sizeof(int16)*2+sizeof(int32)*2},
		{NULL,NULL,"PSEUDO_COLUMN",	NULL,TYPE_I2,		SQL_C_SHORT,	2,0,sizeof(PtrField)*2+sizeof(int16)*3+sizeof(int32)*2},
};

SQLRETURN SQL_API SQLSpecialColumns(SQLHSTMT hstmt,SQLUSMALLINT fColType,
							SQLCHAR FAR *sztab_qualifier,
							SQLSMALLINT cbtab_qualifier,
							SQLCHAR FAR *sztab_owner,
							SQLSMALLINT cbtab_owner,
							SQLCHAR FAR *sztab_name,
							SQLSMALLINT cbtab_name,
							SQLUSMALLINT fScope,
							SQLUSMALLINT fNullable)
{
	SQLSTMT *stmt = (SQLSTMT*)hstmt;
	char Qualifier_buff[NAME_LEN+1],
		Owner_buff[NAME_LEN+1],
		Name_buff[NAME_LEN+1],
       	*tab_qualifier,
		*tab_owner,
		*tab_name;
  	char  sql_str[4096];
	char  where_str[128];

	DBUG_ENTER("SQLSpecialColumns");
	SQLFreeStmt(hstmt,MYSQL_RESET);
	tab_qualifier=fix_str((char FAR *) Qualifier_buff,sztab_qualifier,
			 cbtab_qualifier);
	tab_owner=	 fix_str((char FAR*) Owner_buff,sztab_owner,cbtab_owner);
	tab_name=	 fix_str((char FAR*) Name_buff,sztab_name,cbtab_name);	

	where_str[0]=0x0;
	if(tab_owner[0] &&strcmp(tab_owner,"%"))
		sprintf(where_str," and u.name = '%s'",tab_owner);

	if(tab_name[0] && strcmp(tab_name,"%"))
	{
		strcat(where_str, " and r.name = '" );
		strcat(where_str,tab_name);
		strcat(where_str,"'");
	}

	/*形成结果集*/
	if(fColType = SQL_BEST_ROWID)
	{
		stmt->col_infos = pseudo_attrs;
		stmt->field_num  = 8;
		/*将预定义记录转入result*/
		appendRecord(stmt,pseudo_field_infos[0]);
		if(fScope<SQL_SCOPE_SESSION)
			appendRecord(stmt,pseudo_field_infos[1]);
		stmt->curr_is_over = true;
		stmt->all_is_over = true;
		stmt->have_srv_cursor=false;
		stmt->col_info_is_const=true;
		stmt->cursor_pos = 1;
		stmt->state = ST_EXECUTED;
	}
	else
	{
		sprintf(sql_str,"select 2 ::smallint as \"SCOPE\",f.name as \"COLUMN_NAME\",%d ::smallint as \"DATA_TYPE\","
			"'timestamp' as \"TYPE_NAME\",19::integer as \"COLUMN_SIZE\",19::integer as \"BUFFER_LENGTH\","
			"0 ::smallint as \"DECIMAL_DIGITS\",%d ::smallint as \"PSEUDO_COLUMN\" "
			"from sys_columns f,sys_tables r,sys_users u where r.user_id=u.id and f.table_id=r.id %s and f.type_name='timestamp' order by f.field_no",
			SQL_TIMESTAMP,SQL_PC_NOT_PSEUDO,where_str);
		return  nativeCataQuery(stmt,sql_str);//SQLExecDirect((SQLHSTMT)stmt,sql_str,strlen(sql_str));
	}
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}


/* Have DBMS set up result set of PrimaryKeys. */

/*
@type    : ODBC 1.0 API
@purpose : returns the column names that make up the primary key for a table.
The driver returns the information as a result set. This function
does not support returning primary keys from multiple tables in
a single call
*/
KCIFld  pk_attrs[6] = {
		{NULL,NULL,"TABLE_CAT",	NULL,TYPE_VARCHAR,	SQL_C_CHAR,	32,	0,0},
		{NULL,NULL,"TABLE_SCHEM",NULL,TYPE_VARCHAR,	SQL_C_CHAR,	32,	0,sizeof(PtrField)},
		{NULL,NULL,"TABLE_NAME",	NULL,TYPE_VARCHAR,	SQL_C_CHAR,	64,	FIELD_NOT_NULL,sizeof(PtrField)*2},
		{NULL,NULL,"COLUMN_NAME",NULL,TYPE_VARCHAR,	SQL_C_CHAR,	32,	FIELD_NOT_NULL,sizeof(PtrField)*3},
		{NULL,NULL,"KEY_SEQ",	NULL,TYPE_I2,		SQL_C_SHORT,2,	FIELD_NOT_NULL,sizeof(PtrField)*4},
		{NULL,NULL,"PK_NAME",	NULL,TYPE_VARCHAR,	SQL_C_CHAR,	32,	0,sizeof(PtrField)*4+sizeof(int16)},
};

void add_pk_info(SQLSTMT *stmt,char *db_name,char *sch_str,char *tab_name,char *col_name,int i,char *pk_name)
{
	char  i_str[10];
	char *rec_slot[6];
	rec_slot[0] = db_name;
	rec_slot[1] = sch_str;
	rec_slot[2] = tab_name;
	rec_slot[3] = col_name;
	_itoa(i,i_str,10);
	rec_slot[4] = i_str;
	rec_slot[5] = pk_name;
	appendRecord(stmt,rec_slot);
	stmt->have_srv_cursor=false;
	stmt->all_is_over = true;
	stmt->curr_is_over = true;
	stmt->col_info_is_const=true;
	stmt->cursor_pos = 1;
}

SQLRETURN SQL_API SQLPrimaryKeys(SQLHSTMT hstmt,
								 SQLCHAR FAR *sztab_qualifier,
								 SQLSMALLINT cbtab_qualifier,
								 SQLCHAR FAR *sztab_owner,
								 SQLSMALLINT cbtab_owner,
								 SQLCHAR FAR *sztab_name,
								 SQLSMALLINT cbtab_name)
{
	SQLSTMT *stmt = (SQLSTMT*)hstmt;
	SQLSTMT *temp_stmt;
	char Qualifier_buff[NAME_LEN+1],
		 Owner_buff[NAME_LEN+1],
		 Name_buff[NAME_LEN+1],
       	 *tab_qualifier,
		 *tab_owner,
		 *tab_name;
  	char  sql_str[1024];
	char  where_str[128];
	RhRow *p_rec;
	int		i;

	DBUG_ENTER("SQLPrimaryKeys");
	SQLFreeStmt(hstmt,MYSQL_RESET);
	tab_qualifier=fix_str((char FAR *) Qualifier_buff,sztab_qualifier,cbtab_qualifier);
	tab_owner=	 fix_str((char FAR*) Owner_buff,sztab_owner,cbtab_owner);
	tab_name=	 fix_str((char FAR*) Name_buff,sztab_name,cbtab_name);	

	where_str[0]=0x0;
	if(tab_owner[0] &&strcmp(tab_owner,"%"))
		sprintf(where_str," and u.name = '%s'",tab_owner);

	if(tab_name[0] && strcmp(tab_name,"%"))
	{
		strcat(where_str, " and r.name = '" );
		strcat(where_str,tab_name);
		strcat(where_str,"'");
	}

	if (!tab_qualifier[0] || (!strcmp(tab_qualifier,"%")) || 
		!(_casecmp(tab_qualifier,"SQL_ALL_CATALOGS",16)) || 
		!(_casecmp(tab_qualifier,stmt->dbc->db_name,min(strlen(tab_qualifier),strlen(stmt->dbc->db_name)))))
	{
		
		sprintf(sql_str,"select  u.name,r.name,i.name,i.keys from sys_indexes i,sys_users u,sys_tables r where r.id=i.table_id and r.user_id=u.id and i.is_primary='t' %s",
			where_str);
		if(rh_SQLAllocStmt(stmt->dbc,&temp_stmt)!= SQL_SUCCESS)
			return SQL_ERROR;
		nativeCataQuery(temp_stmt,sql_str);
	
		if(temp_stmt->row_num)
		{
			stmt->col_infos = pk_attrs;
			stmt->field_num = 6;
			stmt->rowset_off = 0;
			stmt->have_srv_cursor = false;
			stmt->col_info_is_const=true;
			stmt->curr_is_over = true;
			stmt->all_is_over = true;
			stmt->cursor_pos = 1;
	
			for(i=0;i<temp_stmt->row_num;i++)
			{
				char *p_ch;
				char *sch_str;
				char *name_str;
				char *pk_name;
				char *key_str;
				char *p_end;
				int	 j;
						
				p_rec =  nthRow(temp_stmt,i);
				sch_str = getData(p_rec,stmt->col_infos,0,NULL);
				name_str = getData(p_rec,stmt->col_infos,1,NULL);
				pk_name = getData(p_rec,stmt->col_infos,2,NULL);
				key_str = getData(p_rec,stmt->col_infos,3,NULL);
		
				p_end = key_str + strlen(key_str);
				j=0;
				while(key_str<p_end)
				{
					p_ch = strchr(key_str,',');
					if(p_ch)
					{
						*p_ch = 0x0;
						add_pk_info(stmt,stmt->dbc->db_name,sch_str,name_str,key_str,j,pk_name);
						key_str=p_ch+1;
					}
					else
					{
						add_pk_info(stmt,stmt->dbc->db_name,sch_str,name_str,key_str,j,pk_name);
						break;
					}
					j++;
				}
			}
			SQLFreeStmt(temp_stmt,0);
			/*将预定义记录转入result*/
			stmt->have_srv_cursor=false;
			stmt->col_info_is_const=true;
			stmt->curr_is_over=true;
			stmt->all_is_over=true;
			stmt->cursor_pos = 1;
			stmt->state = ST_EXECUTED;
			DBUG_RETURN_STATUS(SQL_SUCCESS);
		}
		else
		{
			sprintf(sql_str,"select '' as \"TABLE_CAT\",''as \"TABLE_SCHEM\",'' as \"TABLE_NAME\",'' as \"COLUMN_NAME\",0 ::smallint as \"KEY_SEQ\","
			" ''as \"PK_NAME\" from sys_tables limit 0 ");
			return  nativeCataQuery(stmt,sql_str);
		}
	}
	/*返回空集*/
	else
	{
		sprintf(sql_str,"select '' as \"TABLE_CAT\",''as \"TABLE_SCHEM\",'' as \"TABLE_NAME\",'' as \"COLUMN_NAME\",0 ::smallint as \"KEY_SEQ\","
			" ''as \"PK_NAME\" from sys_tables limit 0 ");
		return  nativeCataQuery(stmt,sql_str);//SQLExecDirect((SQLHSTMT)stmt,sql_str,strlen(sql_str));
	}
}

/*
Function sets up a result set containing details of the types
supported by mysql.
*/

CATA_FIELD SQL_GET_TYPE_INFO_fields[] = {
	{"TYPE_NAME","GetTypeInfo",NULL,TYPE_CHAR,32,32,true},
	{"DATA_TYPE","GetTypeInfo",NULL,TYPE_I2,2,2,true},
	{"COLUMN_SIZE","GetTypeInfo",NULL,TYPE_I4,10,10},
	{"LITERAL_PREFIX","GetTypeInfo",NULL,TYPE_CHAR,2,2},
	{"LITERAL_SUFFIX","GetTypeInfo",NULL,TYPE_CHAR,2,2},
	{"CREATE_PARAMS","GetTypeInfo",NULL,TYPE_CHAR,15,15},
	{"NULLABLE","GetTypeInfo",NULL,TYPE_I2,2,2,true},
	{"CASE_SENSITIVE","GetTypeInfo",NULL,TYPE_I2,2,2,true},
	{"SEARCHABLE","GetTypeInfo",NULL,TYPE_I2,2,2,true},
	{"UNSIGNED_ATTRIBUTE","GetTypeInfo",NULL,TYPE_I2,2,2},
	{"FIXED_PREC_SCALE","GetTypeInfo",NULL,TYPE_I2,2,2,true},
	{"AUTO_UNIQUE_VALUE","GetTypeInfo",NULL,TYPE_I2,2,2},
	{"LOCAL_TYPE_NAME","GetTypeInfo",NULL,TYPE_CHAR,60,60},
	{"MINIMUM_SCALE","GetTypeInfo",NULL,TYPE_I2,2,2},
	{"MAXIMUM_SCALE","GetTypeInfo",NULL,TYPE_I2,2,2},
	{"SQL_DATATYPE","GetTypeInfo",NULL,TYPE_I2,2,2,true},
	{"SQL_DATETIME_SUB","GetTypeInfo",NULL,TYPE_I2,2,2},
	{"NUM_PREC_RADIX","GetTypeInfo",NULL,TYPE_I4,10,10},
	{"INTERVAL_PRECISION","GetTypeInfo",NULL,TYPE_I2,2,2},
};

/*
@type    : myodbc3 internal
@purpose : function initializaions
*/
char sql_searchable[6],sql_unsearchable[6],sql_nullable[6],sql_no_nulls[6],
sql_bit[6],
sql_tinyint[6],sql_smallint[6],sql_integer[6],sql_bigint[6],sql_float[6],
sql_real[6],sql_double[6], sql_char[6],sql_varchar[6],sql_longvarchar[6],
sql_timestamp[6], sql_decimal[6],sql_numeric[6],sql_varbinary[6],
sql_time[6],sql_date[6],sql_binary[6],sql_longvarbinary[6];

#define RHSQL_DATA_TYPES 18
char *rh_type_infos[RHSQL_DATA_TYPES][19]=
{
/*	{"bool",sql_char,"1","'","'",NULL,sql_nullable,
		"0",sql_searchable,"0","0","0","bool",NULL,NULL,sql_char,
		NULL,NULL,NULL},
*/
    {"TINYINT",sql_tinyint,"3",NULL,NULL,NULL,sql_nullable,
		"0",sql_searchable,"0","0","0","TINYINT",NULL,NULL,sql_tinyint,
		NULL,"10",NULL},
	{"SMALLINT",sql_smallint,"5",NULL,NULL,NULL,sql_nullable,
	"0",sql_searchable,"0","0","0","smallint",NULL,NULL,
	sql_smallint,NULL,"10",NULL},
	{"INTEGER",sql_integer,"10",NULL,NULL,NULL,sql_nullable,
	"0",sql_searchable,"0","0","0","integer",NULL,NULL,sql_integer,
	NULL,"10",NULL},
	
	{"BIGINT",sql_bigint,"19",NULL,NULL,NULL,sql_nullable,
	"0",sql_searchable,"0","0","0","bigint",NULL,NULL,sql_bigint,
	NULL,"10",NULL},
	
	/* SQL_CHAR = 1 */
	{"CHAR",sql_char,"256","'","'","length",sql_nullable,
	"0",sql_searchable,"0","0","0","CHAR",NULL,NULL,sql_char,
	NULL,NULL,NULL},
	{"VARCHAR",sql_varchar,"32768","'","'","length",sql_nullable,
	"0",sql_searchable,"0","0","0","VARCHAR",NULL,NULL,sql_varchar,
	NULL,NULL,NULL},
	/* SQL_REAL = 7 */
	{"FLOAT",sql_real,"7",NULL,NULL,NULL,sql_nullable,
	"0",sql_unsearchable,"0","0","0","FLOAT","0","2",sql_float,
	NULL,"10",NULL},
	{"DOUBLE",sql_float,"15",NULL,NULL,NULL,sql_nullable,
	"0",sql_searchable,"0","0","0","DOUBLE","0","4",sql_float,
	NULL,"2",NULL},
	
	/* SQL_DECIMAL = 3 */
	{"NUMERIC",sql_numeric,"32",NULL,NULL,"precision,scale",sql_nullable,
	"0",sql_searchable,"0","0","0","numeric","0","19",sql_numeric,
	NULL,"10",NULL},
	{"DECIMAL",sql_decimal,"32",NULL,NULL,"precision,scale",sql_nullable,
	"0",sql_searchable,"0","0","0","DECIMAL","0","19",sql_numeric,
	NULL,"10",NULL},
	
	/* SQL_TYPE_DATE = 91 */
	{"DATE",sql_date,"10","'","'",NULL,sql_nullable,
	"0",sql_searchable,"0","0","0","DATE",NULL,NULL,sql_date,
	NULL,NULL,NULL},
	
	/* SQL_TYPE_TIME = 92 */
	{"TIME",sql_time,"6","'","'",NULL,sql_nullable,
	"0",sql_searchable,"0","0","0","TIME",NULL,NULL,sql_time,
	NULL,NULL,NULL},
	
	/* SQL_TYPE_TIMESTAMP = 93 */
	{"DATETIME",sql_timestamp,"21","'","'",NULL,sql_nullable,
	"0",sql_searchable,"0","0","0","DATETIME","0","0",sql_timestamp,
	NULL,NULL,NULL},
	{"TIMESTAMP",sql_timestamp,"14","'","'",NULL,sql_no_nulls,
	"0",sql_searchable,"0","0","0","IMESTAMP","0","0",sql_timestamp,
	NULL,NULL,NULL},
	
	{"BINARY",sql_binary,"256","'","'",NULL,sql_nullable,
	"0",sql_unsearchable,"0","0","0","binary",NULL,NULL,
	sql_varbinary,NULL,NULL,NULL},
	
    {"BLOB",sql_longvarbinary,"2147483647","'","'",NULL,sql_nullable,
	"0",sql_unsearchable,"0","0","0","binary large object",NULL,NULL,
	sql_longvarbinary,NULL,NULL,NULL},
	
	{"IMAGE",sql_longvarbinary,"2147483647","'","'",NULL,sql_nullable,
	"0",sql_unsearchable,"0","0","0","IMAGE",NULL,NULL,
	sql_longvarbinary,NULL,NULL,NULL},
	
	{"CLOB",sql_longvarbinary,"2147483647","'","'",NULL,sql_nullable,
	"0",sql_unsearchable,"0","0","0","CLOB",NULL,NULL,
	sql_longvarbinary,NULL,NULL,NULL},
};

KCIFld  type_attrs[19] = {
		{NULL,NULL,"TYPE_NAME",			NULL,TYPE_CHAR,	SQL_C_CHAR,	16,	0,0},
		{NULL,NULL,"DATA_TYPE",			NULL,TYPE_I2,	SQL_C_SHORT,2,	0,sizeof(PtrField)},
		{NULL,NULL,"COLUMN_SIZE",		NULL,TYPE_I4,	SQL_C_LONG,	10,	0,sizeof(PtrField)+sizeof(int16)},
		{NULL,NULL,"LITERAL_PREFIX",		NULL,TYPE_CHAR,	SQL_C_CHAR,	6,	0,sizeof(PtrField)+sizeof(int16)+sizeof(int32)},
		{NULL,NULL,"LITERAL_SUFFIX",		NULL,TYPE_CHAR,	SQL_C_CHAR,	6,	0,sizeof(PtrField)*2+sizeof(int16)+sizeof(int32)},
		{NULL,NULL,"CREATE_PARAMS",		NULL,TYPE_CHAR,	SQL_C_CHAR,	16,	0,sizeof(PtrField)*3+sizeof(int16)+sizeof(int32)},
		{NULL,NULL,"NULLABLE",			NULL,TYPE_I2,	SQL_C_SHORT,2,	0,sizeof(PtrField)*4+sizeof(int16)+sizeof(int32)},
		{NULL,NULL,"CASE_SENSITIVE",		NULL,TYPE_I2,	SQL_C_SHORT,2,	0,sizeof(PtrField)*4+sizeof(int16)*2+sizeof(int32)},
		{NULL,NULL,"SEARCHABLE",			NULL,TYPE_I2,	SQL_C_SHORT,2,	0,sizeof(PtrField)*4+sizeof(int16)*3+sizeof(int32)},
		{NULL,NULL,"UNSIGNED_ATTRIBUTE",	NULL,TYPE_I2,	SQL_C_SHORT,2,	0,sizeof(PtrField)*4+sizeof(int16)*4+sizeof(int32)},
		{NULL,NULL,"FIXED_PREC_SCALE",	NULL,TYPE_I2,	SQL_C_SHORT,2,	0,sizeof(PtrField)*4+sizeof(int16)*5+sizeof(int32)},
		{NULL,NULL,"AUTO_UNIQUE_VALUE",	NULL,TYPE_I2,	SQL_C_SHORT,2,	0,sizeof(PtrField)*4+sizeof(int16)*6+sizeof(int32)},
		{NULL,NULL,"LOCAL_TYPE_NAME",	NULL,TYPE_CHAR,	SQL_C_CHAR,	32,	0,sizeof(PtrField)*4+sizeof(int16)*7+sizeof(int32)},
		{NULL,NULL,"MINIMUM_SCALE",		NULL,TYPE_I2,	SQL_C_SHORT,2,	0,sizeof(PtrField)*5+sizeof(int16)*7+sizeof(int32)},
		{NULL,NULL,"MAXIMUM_SCALE",		NULL,TYPE_I2,	SQL_C_SHORT,2,	0,sizeof(PtrField)*5+sizeof(int16)*8+sizeof(int32)},
		{NULL,NULL,"SQL_DATATYPE",		NULL,TYPE_I2,	SQL_C_SHORT,2,	0,sizeof(PtrField)*5+sizeof(int16)*9+sizeof(int32)},
		{NULL,NULL,"SQL_DATETIME_SUB",	NULL,TYPE_I2,	SQL_C_SHORT,2,	0,sizeof(PtrField)*5+sizeof(int16)*10+sizeof(int32)},
		{NULL,NULL,"NUM_PREC_RADIX",		NULL,TYPE_I4,	SQL_C_LONG,	4,	0,sizeof(PtrField)*5+sizeof(int16)*11+sizeof(int32)},
		{NULL,NULL,"INTERVAL_PRECISION",	NULL,TYPE_I2,	SQL_C_SHORT,2,	0,sizeof(PtrField)*5+sizeof(int16)*11+sizeof(int32)*2},
};

/*
@type    : ODBC 1.0 API
@purpose : returns info about datata types supported by the data source
*/

SQLRETURN SQL_API SQLGetTypeInfo(SQLHSTMT hstmt, SQLSMALLINT fSqlType)
{
	SQLSTMT *stmt = (SQLSTMT*)hstmt;
	int		i;
	DBUG_ENTER("SQLGetTypeInfo");
	
	/*形成结果集*/
	stmt->fields = type_attrs;
	stmt->f = sizeof(type_attrs)/sizeof(KCIFld);
	/*将预定义记录转入result*/
	for(i=0;i<RHSQL_DATA_TYPES;i++)
		appendRecord(stmt,rh_type_infos[i]);
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}

/*初始化常量字段信息*/
void InitFieldsInfos()
{
	int bytes_n,
		field_num,
		n;

	field_num = sizeof(type_attrs)/sizeof(KCIFld);
	bytes_n = (field_num*2+7)/8;
	for(n=0;n<field_num;n++)
	{
		type_attrs[n].off = bytes_n;
		bytes_n += getTypeLen(type_attrs[n].type_id);
	}

	field_num = sizeof(sql_column_attrs)/sizeof(KCIFld);
	bytes_n = (field_num*2+7)/8;
	for(n=0;n<field_num;n++)
	{
		sql_column_attrs[n].off = bytes_n;
		bytes_n += getTypeLen(sql_column_attrs[n].type_id);
	}

	field_num = sizeof(pseudo_attrs)/sizeof(KCIFld);
	bytes_n = (field_num*2+7)/8;
	for(n=0;n<field_num;n++)
	{
		pseudo_attrs[n].off = bytes_n;
		bytes_n += getTypeLen(pseudo_attrs[n].type_id);
	}

	field_num = sizeof(pk_attrs)/sizeof(KCIFld);
	bytes_n = (field_num*2+7)/8;
	for(n=0;n<field_num;n++)
	{
		pk_attrs[n].off = bytes_n;
		bytes_n += getTypeLen(pk_attrs[n].type_id);
	}
}

void init_getfunctions(void)
{
	/* Make some integers to strings for easy init of string arrays */
	_itoa(SQL_SEARCHABLE,sql_searchable,10);
	_itoa(SQL_UNSEARCHABLE,sql_unsearchable,10);
	_itoa(SQL_NULLABLE,sql_nullable,10);
	_itoa(SQL_NO_NULLS,sql_no_nulls,10);
	_itoa(SQL_BIT,sql_bit,10);
	_itoa(SQL_TINYINT,sql_tinyint,10);
	_itoa(SQL_SMALLINT,sql_smallint,10);
	_itoa(SQL_INTEGER,sql_integer,10);
	_itoa(SQL_BIGINT,sql_bigint,10);
	_itoa(SQL_DECIMAL,sql_decimal,10);
	_itoa(SQL_NUMERIC,sql_numeric,10);
	_itoa(SQL_REAL,sql_real,10);
	_itoa(SQL_FLOAT,sql_float,10);
	_itoa(SQL_DOUBLE,sql_double,10);
	_itoa(SQL_CHAR,sql_char,10);
	_itoa(SQL_VARCHAR,sql_varchar,10);
	_itoa(SQL_LONGVARCHAR,sql_longvarchar,10);
	_itoa(SQL_LONGVARBINARY,sql_longvarbinary,10);
	_itoa(SQL_VARBINARY,sql_varbinary,10);
	_itoa(SQL_BINARY,sql_binary,10);
	
#if (ODBCVER < 0x0300)    
	_itoa(SQL_TIMESTAMP,sql_timestamp,10);
	_itoa(SQL_DATE,sql_date,10);
	_itoa(SQL_TIME,sql_time,10);
	myodbc_sqlstate2_init();
#else
	_itoa(SQL_TYPE_TIMESTAMP,sql_timestamp,10);
	_itoa(SQL_TYPE_DATE,sql_date,10);
	_itoa(SQL_TYPE_TIME,sql_time,10);
#endif
}
/*
@type    : ODBC 1.0 API
@purpose : returns
- A list of foreign keys in the specified table (columns
in the specified table that refer to primary keys in
other tables).
- A list of foreign keys in other tables that refer to the primary
key in the specified table
*/

SQLRETURN SQL_API
SQLForeignKeys(SQLHSTMT hstmt,
			   SQLCHAR FAR *szPktab_qualifier,
			   SQLSMALLINT cbPktab_qualifier,
			   SQLCHAR FAR *szPktab_owner,
			   SQLSMALLINT cbPktab_owner,
			   SQLCHAR FAR *szPktab_name,SQLSMALLINT cbPktab_name,
			   SQLCHAR FAR *szFktab_qualifier,
			   SQLSMALLINT cbFktab_qualifier,
			   SQLCHAR FAR *szFktab_owner,
			   SQLSMALLINT cbFktab_owner,
			   SQLCHAR FAR *szFktab_name,SQLSMALLINT cbFktab_name)
{
	DBUG_ENTER("SQLPrimaryKeys");
	return set_error(hstmt,RHERR_S1000,
		"Driver doesn't support this yet",4000);
}


/*
@type    : ODBC 1.0 API
@purpose : returns the list of procedure names stored in a specific data
source. Procedure is a generic term used to describe an
executable object, or a named entity that can be invoked
using input and output parameters
*/

SQLRETURN SQL_API
SQLProcedures(SQLHSTMT hstmt,
			  SQLCHAR FAR *szProcQualifier,SQLSMALLINT cbProcQualifier,
			  SQLCHAR FAR *szProcOwner,SQLSMALLINT cbProcOwner,
			  SQLCHAR FAR *szProcName,SQLSMALLINT cbProcName)
{
	char  Qualifier_buff[NAME_LEN+1],
	 	  Owner_buff[NAME_LEN+1],
		  Name_buff[NAME_LEN+1],
		 *ProcQualifier,
		 *ProcOwner,
		 *ProcName;

	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;

	bool  all_dbs = true;
  	char  sql_str[1024];
	char  where_str[128];
	bool  b_use_where = false;

	DBUG_ENTER("SQLProcedures");

	SQLFreeStmt(hstmt,MYSQL_RESET);
	
	ProcQualifier = fix_str((char FAR *) Qualifier_buff,szProcQualifier, cbProcQualifier);
	ProcOwner     = fix_str((char FAR *) Owner_buff,    szProcOwner,     cbProcOwner);
	ProcName      =	fix_str((char FAR *) Name_buff,     szProcName,      cbProcName);
	
	/*全部表及视*/
	if (!ProcQualifier[0] || (!strcmp(ProcQualifier,"%")) || 
		!(_casecmp(ProcQualifier,"SQL_ALL_CATALOGS",16)) || 
		!(_casecmp(ProcQualifier,stmt->dbc->db_name,min(strlen(ProcQualifier),strlen(stmt->dbc->db_name)))))
	{
		where_str[0]=0x0;
		if(ProcOwner[0] &&strcmp(ProcOwner,"%"))
		{
			sprintf(where_str," and u.name like '%s'",ProcOwner);
			b_use_where = true;
		}

		if(ProcName[0] && strcmp(ProcName,"%"))
		{
			if(b_use_where)
				strcat(where_str, " and r.name like '" );
			else
				strcat(where_str," and r.name like '");
			strcat(where_str,ProcName);
			strcat(where_str,"'");
			b_use_where = true;
		}
	
		/*
		if(tab_type[0] && !stricmp(tab_type,"TABLE"))
			sprintf(sql_str,"select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\",'TABLE' as \"TABLE_TYPE\","
		" ' ' as \"REMARKS\" from sys_tables r,sys_users u,sys_schemas s  where r.user_id=u.id and r.schema_id=s.id %s",stmt->dbc->db_name,where_str);
		else if(tab_type[0] && !stricmp(tab_type,"VIEW"))
			sprintf(sql_str,"select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\",'VIEW' as \"TABLE_TYPE\","
		" ' ' as \"REMARKS\" from sys_views r,sys_users u,sys_schemas s where r.user_id=u.id and r.schema_id=s.id  %s",stmt->dbc->db_name,where_str,stmt->dbc->db_name,where_str);
		else
			sprintf(sql_str,"select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\",'TABLE' as \"TABLE_TYPE\","
		" ' ' as \"REMARKS\" from sys_tables r,sys_users u,sys_schemas s where r.user_id=u.id and r.schema_id=s.id  %s "
		" union all select '%s' as \"TABLE_CAT\",s.name as \"TABLE_SCHEM\",r.name as \"TABLE_NAME\",'VIEW' as \"TABLE_TYPE\","
		" ' ' as \"REMARKS\" from sys_views r,sys_users u,sys_schemas s where r.user_id=u.id and r.schema_id=s.id %s",stmt->dbc->db_name,where_str,stmt->dbc->db_name,where_str);
		*/
		
		sprintf(sql_str,"select '%s' as \"PROCEDURE_CAT\", s.name as \"PROCEDURE_SCHEM\", r.name as \"PROCEDURE_NAME\","
		        " (case  when  r.ret_type isnull then 1 else 2 end)::smallint as \"PROCEDURE_TYPE\","
		        " ' ' as \"REMARKS\" from sys_procedures r, sys_users u, sys_schemas s  where r.user_id=u.id and r.schema_id=s.id  %s ",
		        stmt->dbc->db_name, where_str);

		return nativeCataQuery(stmt,sql_str);
	}
	else 
	{
		sprintf(sql_str,"select '' as \"PROCEDURE_CAT\", '' as \"PROCEDURE_SCHEM\", r.name as \"PROCEDURE_NAME\", 1 as \"PROCEDURE_TYPE\","
			" ' ' as \"REMARKS\" from sys_procedures r  limit 0");
		return nativeCataQuery(stmt,sql_str);	//SQLExecDirect((SQLHSTMT)stmt,sql_str,strlen(sql_str));
	}

	//return set_error(hstmt,RHERR_S1000,
	//	"Driver doesn't support this yet",4000);
}

/*
@type    : ODBC 1.0 API
@purpose : returns the list of input and output parameters, as well as
the columns that make up the result set for the specified
procedures. The driver returns the information as a result
set on the specified statement
*/

SQLRETURN SQL_API SQLProcedureColumns(SQLHSTMT hstmt,
									  SQLCHAR FAR *szProcQualifier,
									  SQLSMALLINT cbProcQualifier,
									  SQLCHAR FAR *szProcOwner,
									  SQLSMALLINT cbProcOwner,
									  SQLCHAR FAR *szProcName,
									  SQLSMALLINT cbProcName,
									  SQLCHAR FAR *szColumnName,
									  SQLSMALLINT cbColumnName)
{
	
	return set_error(hstmt,RHERR_S1000,
		"Driver doesn't support this yet",4000);
}

