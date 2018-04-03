/*************************************************************************** 
* The following ODBC APIs are implemented in this file:                   *
*                                                                         *
*   SQLRowCount          (ISO 92)                                         *
*   SQLNumResultCols     (ISO 92)                                         *
*   SQLDescribeCol       (ISO 92)                                         *
*   SQLColAttribute      (ISO 92)                                         *
*   SQLColAttributes     (ODBC, Deprecated)                               *
*   SQLBindCol           (ISO 92)                                         *
*   SQLFetch             (ISO 92)                                         *
*   SQLFetchScroll       (ISO 92)                                         *
*   SQLGetData           (ISO 92)                                         *
*   SQLExtendedFetch     (ODBC, Deprecated)                               *
*   SQLMoreResults       (ODBC)                                           *
*                                                                         *
****************************************************************************/
#include <locale.h>
#include <time.h>
#include "odbc.h"

 /*
@purpose : execute the qry_str if it is only prepared. This is needed
because the ODBC standard allows calling some functions
before SQLExecute().
*/
static SQLRETURN check_result(SQLSTMT FAR *stmt)
{
	SQLRETURN error;
	
	switch (stmt->state) 
	{
	case ST_UNKNOWN:
		error=setError(stmt,"24000","Invalid cursor state",0);
		break;
	case ST_PREPARED:
		if ((error=SQLExecute(stmt)) == SQL_SUCCESS)
			stmt->state=ST_PRE_EXECUTED;  /* mark for execute */
		break;
	case ST_PRE_EXECUTED:
	case ST_EXECUTED:
		error=SQL_SUCCESS;
	}
	return(error);
}

/*
@type    : ODBC 1.0 API
@purpose : returns the number of columns in a result set
*/

SQLRETURN SQL_API SQLNumResultCols(SQLHSTMT  hstmt, SQLSMALLINT FAR *pccol)
{
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;
	DBUG_ENTER("SQLNumResultCols");		
	*pccol=stmt->fld_num;
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}

/*
@type    : ODBC 1.0 API
@purpose : returns the result column name, type, column size, decimal
digits, and nullabilityfor one column in the result set
*/

SQLRETURN SQL_API
SQLDescribeCol(SQLHSTMT hstmt, SQLUSMALLINT icol,
			   SQLCHAR FAR *szColName,
			   SQLSMALLINT cbColNameMax,
			   SQLSMALLINT FAR *pcbColName,
			   SQLSMALLINT FAR *pfSqlType,
			   SQLUINTEGER FAR *pcbColDef,
			   SQLSMALLINT FAR *pibScale,
			   SQLSMALLINT FAR *pfNullable)
{
	SQLRETURN	error;
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;
	KCIFld	    *p_attr;
	char		buff[64];
	ulong		len,size,preci,display_size;
	int			type;
	
	DBUG_ENTER("SQLDescribeCol");
	if ((error=check_result(stmt)) != SQL_SUCCESS)
		DBUG_RETURN_STATUS(error);
	if (!stmt->fields)
		DBUG_RETURN(setError(stmt,"07005","No result set",0));
	
	/*icol==0表示取bookmark信息*/
	if(icol==0)
	{
		strcpy(buff,"BOOKMARK");
		len = size = 20;
		preci = 0;
	}
	else
	{
		icol--;
		p_attr = &(stmt->fields[icol]);
		type = getFieldInfo(stmt,p_attr,buff,&len,&size,&preci,&display_size);
	}
	if (pfSqlType)
		*pfSqlType= type;
	if (pcbColDef)
		*pcbColDef = size;
	if (pibScale)
		*pibScale = (SQLSMALLINT)preci;
	if (pfNullable)
		*pfNullable= (((p_attr->flag & (FIELD_NOT_NULL)) ==
		FIELD_NOT_NULL) ? SQL_NO_NULLS : SQL_NULLABLE);
	
	DBUG_RETURN(copy_str_data(SQL_HANDLE_STMT, stmt, szColName,
		cbColNameMax, pcbColName, p_attr->col_name));
}

/*
@type    : myodbc3 internal
@purpose : rerunrs column atribute values
*/

SQLRETURN SQL_API
getColumnAttr(SQLHSTMT     StatementHandle,
			 SQLUSMALLINT  ColumnNumber,
			 SQLUSMALLINT FieldIdentifier,
			 SQLPOINTER   CharacterAttributePtr,
			 SQLSMALLINT  BufferLength,
			 SQLSMALLINT  *StringLengthPtr,
			 SQLPOINTER   NumericAttributePtr)
{
	
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) StatementHandle;
	KCIFld  bk_attr;
	SQLSMALLINT str_length;
	SQLPOINTER strparam=0;
	SQLPOINTER nparam=0;
	KCIFld  *p_attr;
	ulong	 trans_len,size,preci,display_size;
	
	SQLRETURN error;
	DBUG_ENTER("get_col_atrr");
	DBUG_PRINT("enter",("column: %d  type: %d",ColumnNumber, FieldIdentifier));
	
	if (check_result(stmt) != SQL_SUCCESS)
		DBUG_RETURN_STATUS(SQL_ERROR);
	
	if (!stmt->fields)
		DBUG_RETURN(setError(stmt,"07005","No result set",0));
#ifdef CHECK_EXTRA_ARGUMENTS
	if (ColumnNumber > stmt->result->field_count)
		DBUG_RETURN(set_error(StatementHandle, RHERR_07009,NULL,0));
#endif
	if (!StringLengthPtr)
		StringLengthPtr= &str_length;
	
	if (!CharacterAttributePtr)
		CharacterAttributePtr= nparam;
	
	if (!NumericAttributePtr)
		NumericAttributePtr= strparam;
	
	if ((error=check_result(stmt)) != SQL_SUCCESS)
		DBUG_RETURN_STATUS(error);
	
	if (FieldIdentifier == SQL_DESC_COUNT ||
		FieldIdentifier == SQL_COLUMN_COUNT )
	{
		*(int *)NumericAttributePtr= stmt->fld_num;
		DBUG_RETURN_STATUS(SQL_SUCCESS);
	}
	if (FieldIdentifier == SQL_DESC_TYPE && ColumnNumber == 0)
	{
		*(int *) NumericAttributePtr=SQL_INTEGER;
		DBUG_RETURN_STATUS(SQL_SUCCESS);
	}
	
	if(ColumnNumber==0)
	{
		bk_attr.type_id = TYPE_I8;
		bk_attr.flag = 0;
		bk_attr.scale = 8;
		bk_attr.col_name = "BOOKMARK";
		p_attr = &bk_attr;
	}
	else
		p_attr = &(stmt->fields[ColumnNumber-1]);
	switch (FieldIdentifier) 
	{
	case SQL_DESC_AUTO_UNIQUE_VALUE:
		*(int *)NumericAttributePtr = (p_attr->flag &(FIELD_IS_SERIAL) ?
            SQL_TRUE : SQL_FALSE);
    break;
		  /* We need support from server, when aliasing is there */
	case SQL_DESC_BASE_COLUMN_NAME:
		{
			DBUG_RETURN(copy_str_data(SQL_HANDLE_STMT, stmt, (uchar*)CharacterAttributePtr,
				BufferLength,StringLengthPtr, p_attr->col_name));
		}
	case SQL_DESC_LABEL:
	case SQL_DESC_NAME:
	case SQL_COLUMN_NAME:
		{
			if(p_attr->col_label)
				DBUG_RETURN(copy_str_data(SQL_HANDLE_STMT, stmt, (uchar*)CharacterAttributePtr,
					BufferLength,StringLengthPtr,p_attr->col_label));
			else
				DBUG_RETURN(copy_str_data(SQL_HANDLE_STMT, stmt, (uchar*)CharacterAttributePtr,
					BufferLength,StringLengthPtr, p_attr->col_name));
		}
	case SQL_DESC_BASE_TABLE_NAME:
	case SQL_DESC_TABLE_NAME:
		{
			char *table_name = p_attr->tab_name;
			if(!table_name)
				table_name = "";
			DBUG_RETURN(copy_str_data(SQL_HANDLE_STMT, stmt, (uchar*)CharacterAttributePtr,
			      BufferLength,StringLengthPtr,table_name));
		}
	case SQL_DESC_CASE_SENSITIVE:
		*(int *)NumericAttributePtr =  SQL_FALSE;
		break;
	case SQL_DESC_CATALOG_NAME:
		DBUG_RETURN(copy_str_data(SQL_HANDLE_STMT, stmt, (uchar*)CharacterAttributePtr,
            BufferLength, StringLengthPtr,
			""));//stmt->dbc->db_name));

	case SQL_DESC_DISPLAY_SIZE:
		(void) getFieldInfo(stmt,p_attr,0,&trans_len,&size,&preci,
				  &display_size);
		*(int *)NumericAttributePtr = display_size;
		break;

	case SQL_DESC_FIXED_PREC_SCALE: /* need to verify later */
	{
		if( p_attr->type_id == TYPE_NUM)
			*(int *)NumericAttributePtr = SQL_TRUE;
		else
			*(int *)NumericAttributePtr = SQL_FALSE;
		break;
	}
  case SQL_COLUMN_LENGTH:
  case SQL_DESC_LENGTH:
  case SQL_DESC_OCTET_LENGTH: /* need to validate again for octet length..*/
    (void) getFieldInfo(stmt,p_attr,0,&trans_len,&size,&preci,
				  &display_size);
    *(int *)NumericAttributePtr = trans_len;
    break;

  case SQL_DESC_LITERAL_PREFIX:
  case SQL_DESC_LITERAL_SUFFIX:
    switch (p_attr->type_id)  
	{
	case TYPE_DATE:
    case TYPE_DATETIME:
    case TYPE_CHAR:
    case TYPE_TIME:

      DBUG_RETURN(copy_str_data(SQL_HANDLE_STMT, stmt,(uchar*)CharacterAttributePtr,
				BufferLength,StringLengthPtr,"'"));

    default:
      DBUG_RETURN(copy_str_data(SQL_HANDLE_STMT, stmt, (uchar*)CharacterAttributePtr,
				BufferLength,StringLengthPtr,""));  
	}
	case SQL_DESC_NULLABLE:
  case SQL_COLUMN_NULLABLE:
    *(int *)NumericAttributePtr = (((p_attr->flag &
            (FIELD_IS_SERIAL|FIELD_IS_PRIMARY))) ?
            SQL_NO_NULLS :
            SQL_NULLABLE);
    break;

  case SQL_DESC_NUM_PREC_RADIX:
    switch(p_attr->type_id)  
	{
	case TYPE_I1:
	case TYPE_I2:
    case TYPE_I4:
    case TYPE_I8:
  case TYPE_NUM:
      *(int *)NumericAttributePtr = 10;
      break;

    case TYPE_R4:
    case TYPE_R8:
      *(int *) NumericAttributePtr = 2;
      break;

    default:
      *(int *)NumericAttributePtr = 0;
      break;
    }
    break;

  case SQL_COLUMN_PRECISION:
  case SQL_DESC_PRECISION:
    (void )getFieldInfo(stmt,p_attr,0,&trans_len,&size,&preci,
				  &display_size);
    *(int *)NumericAttributePtr = size;
    break;

  case SQL_COLUMN_SCALE:
  case SQL_DESC_SCALE:
	 (void )getFieldInfo(stmt,p_attr,0,&trans_len,&size,&preci,
				  &display_size);
    *(int *)NumericAttributePtr = preci;
    break;

  case SQL_DESC_SCHEMA_NAME:
    DBUG_RETURN(copy_str_data(SQL_HANDLE_STMT, stmt, (uchar*)CharacterAttributePtr,
			      BufferLength,StringLengthPtr, ""));

  case SQL_DESC_SEARCHABLE:
	  if(p_attr->type_id==TYPE_BINARY ||p_attr->type_id==TYPE_BLOB)
		*(int *)NumericAttributePtr = SQL_UNSEARCHABLE;
	  else 
		*(int *)NumericAttributePtr = SQL_SEARCHABLE;
    break;

  case SQL_DESC_TYPE:
  case SQL_DESC_CONCISE_TYPE:
    *(int *)NumericAttributePtr =
      getFieldInfo(stmt, p_attr, 0, &trans_len, &size,&preci,
           &display_size);
    break;

  case SQL_DESC_LOCAL_TYPE_NAME:
  case SQL_DESC_TYPE_NAME:
  {
    char buff[40];
    (void)getFieldInfo(stmt,p_attr,(char *)buff,&trans_len,
         &size,&preci, &display_size);
    DBUG_RETURN(copy_str_data(SQL_HANDLE_STMT, stmt, (uchar*)CharacterAttributePtr,
			      BufferLength,StringLengthPtr, buff));
  }

  case SQL_DESC_UNNAMED:
    *(int *)NumericAttributePtr=SQL_NAMED;
    break;

  case SQL_DESC_UNSIGNED:
    *(int *) NumericAttributePtr= SQL_FALSE;
    break;

  case SQL_DESC_UPDATABLE:
	 
	 *(int *)NumericAttributePtr= SQL_ATTR_READWRITE_UNKNOWN;
    break;
  /* Hack : Fix for the error from ADO 'rs.resync' "Key value for this 
            row was changed or deleted at the data store.  The local 
            row is now deleted. This should also fix some Multi-step 
            generated error cases from ADO 
  */
  case SQL_MY_PRIMARY_KEY: /* MSSQL extension !! */
    *(int *)NumericAttributePtr= ((p_attr->flag & (FIELD_IS_PRIMARY|FIELD_IS_SERIAL))?
					 SQL_TRUE : SQL_FALSE);
    break;

  default:
    DBUG_PRINT("warning",("Type: %d is not supported by the driver",
			  FieldIdentifier));
	printf("unkwon");
 }
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}

/*
@type    : ODBC 3.0 API
@purpose : rerunrs column atribute values
*/

SQLRETURN SQL_API
SQLColAttribute(SQLHSTMT  StatementHandle,
				SQLUSMALLINT ColumnNumber,
				SQLUSMALLINT FieldIdentifier,
				SQLPOINTER  CharacterAttributePtr,
				SQLSMALLINT BufferLength,
				SQLSMALLINT *StringLengthPtr,
				SQLPOINTER  NumericAttributePtr)
{
	DBUG_ENTER("SQLColAttribute");
	return getColumnAttr(StatementHandle, ColumnNumber,
		FieldIdentifier, CharacterAttributePtr,
		BufferLength, StringLengthPtr,
		NumericAttributePtr);
}

/*
@type    : ODBC 1.0 API
@purpose : rerunrs column atribute values
*/

SQLRETURN SQL_API
SQLColAttributes(SQLHSTMT hstmt,SQLUSMALLINT icol,
				 SQLUSMALLINT fDescType,
				 SQLPOINTER rgbDesc, SQLSMALLINT cbDescMax,
				 SQLSMALLINT FAR *pcbDesc,
				 int FAR *pfDesc)
{
	SQLRETURN result=SQL_SUCCESS;
	DBUG_ENTER("SQLColAttributes");
	
	result = getColumnAttr(hstmt, icol, fDescType, rgbDesc,
		cbDescMax, pcbDesc, pfDesc);
	DBUG_RETURN_STATUS(result);
}

/*
@type    : ODBC 1.0 API
@purpose : binds application data buffers to columns in the result set
*/

SQLRETURN SQL_API
SQLBindCol(SQLHSTMT hstmt, 
		   SQLUSMALLINT icol,
		   SQLSMALLINT fCType, 
		   SQLPOINTER rgbValue,
		   int  cbValueMax, 
		   int FAR *pcbValue)
{
	SQLSTMT  *stmt=(SQLSTMT*) hstmt;
	DBUG_ENTER("SQLBindCol");
	//todo
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}

/*
@type    : ODBC 1.0 API
@purpose : fetches the next rowset of data from the result set and
returns data for all bound columns
*/

SQLRETURN SQL_API SQLFetch(SQLHSTMT hstmt)
{
	SQLSTMT	*stmt=(SQLSTMT*) hstmt;
	DBUG_ENTER("SQLFetch");
	//todo  按stmtopt中设置的fetch行数进行fetch操作
	return SQL_SUCCESS;
}

/*
@purpose : fetches the specified rowset of data from the result set and
returns data for all bound columns. Rowsets can be specified
at an absolute or relative position
*/
SQLRETURN SQL_API
rh_SQLExtendedFetch(SQLHSTMT hstmt, SQLUSMALLINT fFetchType,
					int irow, SQLUINTEGER FAR *pcrow,
					SQLUSMALLINT FAR *rgfRowStatus)
{

	SQLSTMT	FAR *stmt=(SQLSTMT FAR*) hstmt;
	DBUG_ENTER("SQLExtendedFetch");
	
	switch (fFetchType) 
	{
	case SQL_FETCH_NEXT:
	case SQL_FETCH_PRIOR:
	case SQL_FETCH_FIRST:
	case SQL_FETCH_LAST:
		break;
	case SQL_FETCH_ABSOLUTE:
	case SQL_FETCH_RELATIVE:
		break;
	default:
		DBUG_RETURN(setError(stmt,RHERR_S1106,"Fetch type error.",0));
	}
	//todo
	return SQL_SUCCESS;
}
	
/*
@type    : ODBC 1.0 API
@purpose : fetches the specified rowset of data from the result set and
returns data for all bound columns. Rowsets can be specified
at an absolute or relative position
*/

SQLRETURN SQL_API
	SQLExtendedFetch(SQLHSTMT hstmt,SQLUSMALLINT fFetchType,
	int irow,
	SQLUINTEGER FAR *pcrow,
	SQLUSMALLINT FAR *rgfRowStatus)
{
	return rh_SQLExtendedFetch(hstmt, fFetchType, irow,
		pcrow, rgfRowStatus);
}
	
/*
@type    : ODBC 3.0 API
@purpose : fetches the specified rowset of data from the result set and
returns data for all bound columns. Rowsets can be specified
at an absolute or relative position
*/

SQLRETURN SQL_API
	SQLFetchScroll(SQLHSTMT    StatementHandle,
	SQLSMALLINT FetchOrientation,
	int  FetchOffset)
{
	SQLRETURN result=SQL_SUCCESS;
	STMT_OPTIONS *options = &((SQLSTMT FAR *)StatementHandle)->stmt_options;
	DBUG_ENTER("SQLFetchScroll");
	
	result = rh_SQLExtendedFetch(StatementHandle, FetchOrientation,
		FetchOffset,
		options->rowsFetchedPtr,
		options->rowStatusPtr);
	DBUG_RETURN_STATUS(result);
}
	
/*
@type    : ODBC 1.0 API
@purpose : retrieves data for a single column in the result set. It can
be called multiple times to retrieve variable-length data
in parts
*/

SQLRETURN SQL_API
SQLGetData(SQLHSTMT hstmt,SQLUSMALLINT icol,
		   SQLSMALLINT fCType,SQLPOINTER rgbValue,
		   int cbValueMax, int FAR *pcbValue)
{
	return SQL_SUCCESS;
}

/*
@type    : ODBC 1.0 API
@purpose : determines whether more results are available on a statement
containing SELECT, UPDATE, INSERT, or DELETE statements and,
if so, initializes processing for those results
*/

SQLRETURN SQL_API SQLMoreResults(SQLHSTMT  hstmt)
{
	DBUG_ENTER("SQLMoreResults");
	DBUG_RETURN_STATUS(SQL_NO_DATA_FOUND);
}

/*
@type    : ODBC 1.0 API
@purpose : returns the number of rows affected by an UPDATE, INSERT,
or DELETE statement;an SQL_ADD, SQL_UPDATE_BY_BOOKMARK,
or SQL_DELETE_BY_BOOKMARK operation in SQLBulkOperations;
or an SQL_UPDATE or SQL_DELETE operation in SQLSetPos
*/

SQLRETURN SQL_API SQLRowCount(SQLHSTMT hstmt, int FAR *pcrow)
{
	SQLSTMT FAR *stmt=(SQLSTMT FAR*) hstmt;
	DBUG_ENTER("SQLRowCount");
	
	if(stmt->fields)
		*pcrow = -1;
	else
		*pcrow= (int) stmt->updt_num;
	DBUG_RETURN_STATUS(SQL_SUCCESS);
}

