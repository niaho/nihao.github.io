#pragma once
/*
NAME
kci.h - V8 Oracle Call Interface public definitions
*/
#ifndef __KCI_H__
#define __KCI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <kci_def.h> 
	/*
	* since sqllib uses both KCIdef and KCIdfn the following defines
	* need to be guarded
	*/
#ifndef KCI_FLAGS 
#define KCI_FLAGS

	/* KCI_*_PIECE defines the piece types that are returned or set
	*/
#define KCI_ONE_PIECE   0                   /* there or this is the only piece */
#define KCI_FIRST_PIECE 1                   /* the first of many pieces */
#define KCI_NEXT_PIECE  2                   /* the next of many pieces */
#define KCI_LAST_PIECE  3                   /* the last piece of this column */
#endif

#ifndef SQLDEF 

	/* input data types */
#define SQLT_CHR  1                        /* (ORANET TYPE) character string */
#define SQLT_NUM  2                        /* (ORANET TYPE) oracle numeric */
#define SQLT_INT  3                        /* (ORANET TYPE) integer */
#define SQLT_FLT  4                   /* (ORANET TYPE) Floating point number */
#define SQLT_STR  5                                /* zero terminated string */
#define SQLT_VNU  6                        /* NUM with preceding length byte */
#define SQLT_PDN  7                  /* (ORANET TYPE) Packed Decimal Numeric */
#define SQLT_LNG  8                                                  /* long */
#define SQLT_VCS  9                             /* Variable character string */
#define SQLT_NON  10                      /* Null/empty PCC Descriptor entry */
#define SQLT_RID  11                                                /* rowid */
#define SQLT_DAT  12                                /* date in oracle format */
#define SQLT_VBI  15                                 /* binary in VCS format */
#define SQLT_BFLOAT 21                                /* Native Binary float*/
#define SQLT_BDOUBLE 22                             /* NAtive binary double */
#define SQLT_BIN  23                                  /* binary data(DTYBIN) */
#define SQLT_LBI  24                                          /* long binary */
#define SQLT_UIN  68                                     /* unsigned integer */
#define SQLT_SLS  91                        /* Display sign leading separate */
#define SQLT_LVC  94                                  /* Longer longs (char) */
#define SQLT_LVB  95                                   /* Longer long binary */
#define SQLT_AFC  96                                      /* Ansi fixed char */
#define SQLT_AVC  97                                        /* Ansi Var char */
#define SQLT_IBFLOAT  100                          /* binary float canonical */
#define SQLT_IBDOUBLE 101                         /* binary double canonical */
#define SQLT_CUR  102                                        /* cursor  type */
#define SQLT_RDD  104                                    /* rowid descriptor */
#define SQLT_LAB  105                                          /* label type */
#define SQLT_OSL  106                                        /* oslabel type */

#define SQLT_NTY  108                                   /* named object type */
#define SQLT_REF  110                                            /* ref type */
#define SQLT_CLOB 112                                       /* character lob */
#define SQLT_BLOB 113                                          /* binary lob */
#define SQLT_BFILEE 114                                   /* binary file lob */
#define SQLT_CFILEE 115                                /* character file lob */
#define SQLT_RSET 116                                     /* result set type */
#define SQLT_NCO  122      /* named collection type (varray or nested table) */
#define SQLT_VST  155                                      /* KCIString type */
#define SQLT_ODT  156                                        /* KCIDate type */

	/* datetimes and intervals */
#define SQLT_DATE                      184                      /* ANSI Date */
#define SQLT_TIME                      185                           /* TIME */
#define SQLT_TIME_TZ                   186            /* TIME WITH TIME ZONE */
#define SQLT_TIMESTAMP                 187                      /* TIMESTAMP */
#define SQLT_TIMESTAMP_TZ              188       /* TIMESTAMP WITH TIME ZONE */
#define SQLT_INTERVAL_YM               189         /* INTERVAL YEAR TO MONTH */
#define SQLT_INTERVAL_DS               190         /* INTERVAL DAY TO SECOND */
#define SQLT_TIMESTAMP_LTZ             232        /* TIMESTAMP WITH LOCAL TZ */



#define SQLT_PNTY   241              /* pl/sql representation of named types */

	/* some pl/sql specific types */
#define SQLT_REC    250                     /* pl/sql 'record' (or %rowtype) */
#define SQLT_TAB    251                            /* pl/sql 'indexed table' */
#define SQLT_BOL    252                                  /* pl/sql 'boolean' */

	/* cxcheng: this has been added for backward compatibility -
	it needs to be here because KCIdfn.h can get included ahead of sqldef.h */
#define SQLT_FILE SQLT_BFILEE                             /* binary file lob */
#define SQLT_CFILE SQLT_CFILEE
#define SQLT_BFILE SQLT_BFILEE

	/* CHAR/NCHAR/VARCHAR2/NVARCHAR2/CLOB/NCLOB char set "form" information */
#define SQLCS_IMPLICIT 1     /* for CHAR, VARCHAR2, CLOB w/o a specified set */
#define SQLCS_NCHAR    2                  /* for NCHAR, NCHAR VARYING, NCLOB */
#define SQLCS_EXPLICIT 3   /* for CHAR, etc, with "CHARACTER SET ..." syntax */
#define SQLCS_FLEXIBLE 4                 /* for PL/SQL "flexible" parameters */
#define SQLCS_LIT_NULL 5      /* for typecheck of NULL and empty_clob() lits */
#endif

/*-----------------------------Handle Types----------------------------------*/
	/* handle types range from 1 - 49 */
#define KCI_HTYPE_FIRST          1             /* start value of handle type */
#define KCI_HTYPE_ENV            1                     /* environment handle */
#define KCI_HTYPE_ERROR          2                           /* error handle */
#define KCI_HTYPE_SVCCTX         3                         /* service handle */
#define KCI_HTYPE_STMT           4                       /* statement handle */
#define KCI_HTYPE_BIND           5                            /* bind handle */
#define KCI_HTYPE_DEFINE         6                          /* define handle */
#define KCI_HTYPE_DESCRIBE       7                        /* describe handle */
#define KCI_HTYPE_SERVER         8                          /* server handle */
#define KCI_HTYPE_SESSION        9                  /* authentication handle */
#define KCI_HTYPE_AUTHINFO      KCI_HTYPE_SESSION  /* SessionGet auth handle */
#define KCI_HTYPE_TRANS         10                     /* transaction handle */
#define KCI_HTYPE_COMPLEXOBJECT 11        /* complex object retrieval handle */
#define KCI_HTYPE_SECURITY      12                        /* security handle */
#define KCI_HTYPE_SUBSCRIPTION  13                    /* subscription handle */
#define KCI_HTYPE_DIRPATH_CTX   14                    /* direct path context */
#define KCI_HTYPE_DIRPATH_COLUMN_ARRAY 15        /* direct path column array */
#define KCI_HTYPE_DIRPATH_STREAM       16              /* direct path stream */
#define KCI_HTYPE_PROC                 17                  /* process handle */
#define KCI_HTYPE_DIRPATH_FN_CTX       18    /* direct path function context */
#define KCI_HTYPE_DIRPATH_FN_COL_ARRAY 19          /* dp object column array */
#define KCI_HTYPE_XADSESSION    20                  /* access driver session */
#define KCI_HTYPE_XADTABLE      21                    /* access driver table */
#define KCI_HTYPE_XADFIELD      22                    /* access driver field */
#define KCI_HTYPE_XADGRANULE    23                  /* access driver granule */
#define KCI_HTYPE_XADRECORD     24                   /* access driver record */
#define KCI_HTYPE_XADIO         25                      /* access driver I/O */
#define KCI_HTYPE_CPOOL         26                 /* connection pool handle */
#define KCI_HTYPE_SPOOL         27                    /* session pool handle */
#define KCI_HTYPE_ADMIN         28                           /* admin handle */
#define KCI_HTYPE_EVENT         29                        /* HA event handle */

#define KCI_HTYPE_LAST          29            /* last value of a handle type */

	/*---------------------------------------------------------------------------*/


	/*-------------------------Descriptor Types----------------------------------*/
	/* descriptor values range from 50 - 255 */
#define KCI_DTYPE_FIRST 50                 /* start value of descriptor type */
#define KCI_DTYPE_LOB 50                                     /* lob  locator */
#define KCI_DTYPE_SNAP 51                             /* snapshot descriptor */
#define KCI_DTYPE_RSET 52                           /* result set descriptor */
#define KCI_DTYPE_PARAM 53  /* a parameter descriptor obtained from ocigparm */
#define KCI_DTYPE_ROWID  54                              /* rowid descriptor */
#define KCI_DTYPE_COMPLEXOBJECTCOMP  55
	/* complex object retrieval descriptor */
#define KCI_DTYPE_FILE 56                                /* File Lob locator */
#define KCI_DTYPE_AQENQ_OPTIONS 57                        /* enqueue options */
#define KCI_DTYPE_AQDEQ_OPTIONS 58                        /* dequeue options */
#define KCI_DTYPE_AQMSG_PROPERTIES 59                  /* message properties */
#define KCI_DTYPE_AQAGENT 60                                     /* aq agent */
#define KCI_DTYPE_LOCATOR 61                                  /* LOB locator */
#define KCI_DTYPE_INTERVAL_YM 62                      /* Interval year month */
#define KCI_DTYPE_INTERVAL_DS 63                      /* Interval day second */
#define KCI_DTYPE_AQNFY_DESCRIPTOR  64               /* AQ notify descriptor */
#define KCI_DTYPE_DATE 65                            /* Date */
#define KCI_DTYPE_TIME 66                            /* Time */
#define KCI_DTYPE_TIME_TZ 67                         /* Time with timezone */
#define KCI_DTYPE_TIMESTAMP 68                       /* Timestamp */
#define KCI_DTYPE_TIMESTAMP_TZ 69                /* Timestamp with timezone */
#define KCI_DTYPE_TIMESTAMP_LTZ 70             /* Timestamp with local tz */
#define KCI_DTYPE_UCB           71               /* user callback descriptor */
#define KCI_DTYPE_SRVDN         72              /* server DN list descriptor */
#define KCI_DTYPE_SIGNATURE     73                              /* signature */
#define KCI_DTYPE_RESERVED_1    74              /* reserved for internal use */
#define KCI_DTYPE_AQLIS_OPTIONS 75                      /* AQ listen options */
#define KCI_DTYPE_AQLIS_MSG_PROPERTIES 76             /* AQ listen msg props */
#define KCI_DTYPE_CHDES         77     /* Top level change notification desc */
#define KCI_DTYPE_TABLE_CHDES   78      /* Table change descriptor           */
#define KCI_DTYPE_ROW_CHDES     79       /* Row change descriptor            */
#define KCI_DTYPE_CQDES  80                       /* Query change descriptor */
#define KCI_DTYPE_LOB_REGION    81            /* LOB Share region descriptor */
#define KCI_DTYPE_RESERVED_82   82                               /* reserved */
#define KCI_DTYPE_LAST          82        /* last value of a descriptor type */

	/*---------------------------------------------------------------------------*/

	/*--------------------------------LOB types ---------------------------------*/
#define KCI_TEMP_BLOB 1                /* LOB type - BLOB ------------------ */
#define KCI_TEMP_CLOB 2                /* LOB type - CLOB ------------------ */
	/*---------------------------------------------------------------------------*/

	/*-------------------------Object Ptr Types----------------------------------*/
#define KCI_OTYPE_NAME 1                                      /* object name */
#define KCI_OTYPE_REF  2                                       /* REF to TDO */
#define KCI_OTYPE_PTR  3                                       /* PTR to TDO */
	/*---------------------------------------------------------------------------*/

	/*=============================Attribute Types===============================*/
	/*
	Note: All attributes are global.  New attibutes should be added to the end
	of the list. Before you add an attribute see if an existing one can be
	used for your handle. Note: this does not mean reuse an existing number;
	it means use an attribute name from another handle for your handle.

	If you see any holes please use the holes first. However, be very
	careful, as this file is not fully ordered.

	*/
	/*===========================================================================*/


#define KCI_ATTR_FNCODE  1                          /* the KCI function code */
#define KCI_ATTR_OBJECT   2 /* is the environment initialized in object mode */
#define KCI_ATTR_NONBLOCKING_MODE  3                    /* non blocking mode */
#define KCI_ATTR_SQLCODE  4                                  /* the SQL verb */
#define KCI_ATTR_ENV  5                            /* the environment handle */
#define KCI_ATTR_SERVER 6                               /* the server handle */
#define KCI_ATTR_SESSION 7                        /* the user session handle */
#define KCI_ATTR_TRANS   8                         /* the transaction handle */
#define KCI_ATTR_ROW_COUNT   9                  /* the rows processed so far */
#define KCI_ATTR_SQLFNCODE 10               /* the SQL verb of the statement */
#define KCI_ATTR_PREFETCH_ROWS  11    /* sets the number of rows to prefetch */
#define KCI_ATTR_NESTED_PREFETCH_ROWS 12 /* the prefetch rows of nested table*/
#define KCI_ATTR_PREFETCH_MEMORY 13         /* memory limit for rows fetched */
#define KCI_ATTR_NESTED_PREFETCH_MEMORY 14   /* memory limit for nested rows */
#define KCI_ATTR_CHAR_COUNT  15 
	/* this specifies the bind and define size in characters */
#define KCI_ATTR_PDSCL   16                          /* packed decimal scale */
#define KCI_ATTR_FSPRECISION KCI_ATTR_PDSCL   
	/* fs prec for datetime data types */
#define KCI_ATTR_PDPRC   17                         /* packed decimal format */
#define KCI_ATTR_LFPRECISION KCI_ATTR_PDPRC 
	/* fs prec for datetime data types */
#define KCI_ATTR_PARAM_COUNT 18       /* number of column in the select list */
#define KCI_ATTR_ROWID   19                                     /* the rowid */
#define KCI_ATTR_CHARSET  20                      /* the character set value */
#define KCI_ATTR_NCHAR   21                                    /* NCHAR type */
#define KCI_ATTR_USERNAME 22                           /* username attribute */
#define KCI_ATTR_PASSWORD 23                           /* password attribute */
#define KCI_ATTR_STMT_TYPE   24                            /* statement type */
#define KCI_ATTR_INTERNAL_NAME   25             /* user friendly global name */
#define KCI_ATTR_EXTERNAL_NAME   26      /* the internal name for global txn */
#define KCI_ATTR_XID     27           /* XOPEN defined global transaction id */
#define KCI_ATTR_TRANS_LOCK 28                                            /* */
#define KCI_ATTR_TRANS_NAME 29    /* string to identify a global transaction */
#define KCI_ATTR_HEAPALLOC 30                /* memory allocated on the heap */
#define KCI_ATTR_CHARSET_ID 31                           /* Character Set ID */
#define KCI_ATTR_CHARSET_FORM 32                       /* Character Set Form */
#define KCI_ATTR_MAXDATA_SIZE 33       /* Maximumsize of data on the server  */
#define KCI_ATTR_CACHE_OPT_SIZE 34              /* object cache optimal size */
#define KCI_ATTR_CACHE_MAX_SIZE 35   /* object cache maximum size percentage */
#define KCI_ATTR_PINOPTION 36             /* object cache default pin option */
#define KCI_ATTR_ALLOC_DURATION 37
	/* object cache default allocation duration */
#define KCI_ATTR_PIN_DURATION 38        /* object cache default pin duration */
#define KCI_ATTR_FDO          39       /* Format Descriptor object attribute */
#define KCI_ATTR_POSTPROCESSING_CALLBACK 40
	/* Callback to process outbind data */
#define KCI_ATTR_POSTPROCESSING_CONTEXT 41
	/* Callback context to process outbind data */
#define KCI_ATTR_ROWS_RETURNED 42
	/* Number of rows returned in current iter - for Bind handles */
#define KCI_ATTR_FOCBK        43              /* Failover Callback attribute */
#define KCI_ATTR_IN_V8_MODE   44 /* is the server/service context in V8 mode */
#define KCI_ATTR_LOBEMPTY     45                              /* empty lob ? */
#define KCI_ATTR_SESSLANG     46                  /* session language handle */

#define KCI_ATTR_VISIBILITY             47                     /* visibility */
#define KCI_ATTR_RELATIVE_MSGID         48            /* relative message id */
#define KCI_ATTR_SEQUENCE_DEVIATION     49             /* sequence deviation */

#define KCI_ATTR_CONSUMER_NAME          50                  /* consumer name */
#define KCI_ATTR_DEQ_MODE               51                   /* dequeue mode */
#define KCI_ATTR_NAVIGATION             52                     /* navigation */
#define KCI_ATTR_WAIT                   53                           /* wait */
#define KCI_ATTR_DEQ_MSGID              54             /* dequeue message id */

#define KCI_ATTR_PRIORITY               55                       /* priority */
#define KCI_ATTR_DELAY                  56                          /* delay */
#define KCI_ATTR_EXPIRATION             57                     /* expiration */
#define KCI_ATTR_CORRELATION            58                 /* correlation id */
#define KCI_ATTR_ATTEMPTS               59                  /* # of attempts */
#define KCI_ATTR_RECIPIENT_LIST         60                 /* recipient list */
#define KCI_ATTR_EXCEPTION_QUEUE        61           /* exception queue name */
#define KCI_ATTR_ENQ_TIME               62 /* enqueue time (only KCIAttrGet) */
#define KCI_ATTR_MSG_STATE              63/* message state (only KCIAttrGet) */
	/* NOTE: 64-66 used below */
#define KCI_ATTR_AGENT_NAME             64                     /* agent name */
#define KCI_ATTR_AGENT_ADDRESS          65                  /* agent address */
#define KCI_ATTR_AGENT_PROTOCOL         66                 /* agent protocol */
#define KCI_ATTR_USER_PROPERTY          67                  /* user property */
#define KCI_ATTR_SENDER_ID              68                      /* sender id */
#define KCI_ATTR_ORIGINAL_MSGID         69            /* original message id */

#define KCI_ATTR_QUEUE_NAME             70                     /* queue name */
#define KCI_ATTR_NFY_MSGID              71                     /* message id */
#define KCI_ATTR_MSG_PROP               72             /* message properties */

#define KCI_ATTR_NUM_DML_ERRORS         73       /* num of errs in array DML */
#define KCI_ATTR_DML_ROW_OFFSET         74        /* row offset in the array */

	/* AQ array error handling uses DML method of accessing errors */
#define KCI_ATTR_AQ_NUM_ERRORS          KCI_ATTR_NUM_DML_ERRORS
#define KCI_ATTR_AQ_ERROR_INDEX         KCI_ATTR_DML_ROW_OFFSET

#define KCI_ATTR_DATEFORMAT             75     /* default date format string */
#define KCI_ATTR_BUF_ADDR               76                 /* buffer address */
#define KCI_ATTR_BUF_SIZE               77                    /* buffer size */

	/* For values 78 - 80, see DirPathAPI attribute section in this file */

#define KCI_ATTR_NUM_ROWS               81 /* number of rows in column array */
	/* NOTE that KCI_ATTR_NUM_COLS is a column
	* array attribute too.
	*/
#define KCI_ATTR_COL_COUNT              82        /* columns of column array
	processed so far.       */
#define KCI_ATTR_STREAM_OFFSET          83  /* str off of last row processed */
#define KCI_ATTR_SHARED_HEAPALLOC       84    /* Shared Heap Allocation Size */

#define KCI_ATTR_SERVER_GROUP           85              /* server group name */

#define KCI_ATTR_MIGSESSION             86   /* migratable session attribute */

#define KCI_ATTR_NOCACHE                87                 /* Temporary LOBs */

#define KCI_ATTR_MEMPOOL_SIZE           88                      /* Pool Size */
#define KCI_ATTR_MEMPOOL_INSTNAME       89                  /* Instance name */
#define KCI_ATTR_MEMPOOL_APPNAME        90               /* Application name */
#define KCI_ATTR_MEMPOOL_HOMENAME       91            /* Home Directory name */
#define KCI_ATTR_MEMPOOL_MODEL          92     /* Pool Model (proc,thrd,both)*/
#define KCI_ATTR_MODES                  93                          /* Modes */

#define KCI_ATTR_SUBSCR_NAME            94           /* name of subscription */
#define KCI_ATTR_SUBSCR_CALLBACK        95            /* associated callback */
#define KCI_ATTR_SUBSCR_CTX             96    /* associated callback context */
#define KCI_ATTR_SUBSCR_PAYLOAD         97             /* associated payload */
#define KCI_ATTR_SUBSCR_NAMESPACE       98           /* associated namespace */

#define KCI_ATTR_PROXY_CREDENTIALS      99         /* Proxy user credentials */
#define KCI_ATTR_INITIAL_CLIENT_ROLES  100       /* Initial client role list */

#define KCI_ATTR_UNK              101                   /* unknown attribute */
#define KCI_ATTR_NUM_COLS         102                   /* number of columns */
#define KCI_ATTR_LIST_COLUMNS     103        /* parameter of the column list */
#define KCI_ATTR_RDBA             104           /* DBA of the segment header */
#define KCI_ATTR_CLUSTERED        105      /* whether the table is clustered */
#define KCI_ATTR_PARTITIONED      106    /* whether the table is partitioned */
#define KCI_ATTR_INDEX_ONLY       107     /* whether the table is index only */
#define KCI_ATTR_LIST_ARGUMENTS   108      /* parameter of the argument list */
#define KCI_ATTR_LIST_SUBPROGRAMS 109    /* parameter of the subprogram list */
#define KCI_ATTR_REF_TDO          110          /* REF to the type descriptor */
#define KCI_ATTR_LINK             111              /* the database link name */
#define KCI_ATTR_MIN              112                       /* minimum value */
#define KCI_ATTR_MAX              113                       /* maximum value */
#define KCI_ATTR_INCR             114                     /* increment value */
#define KCI_ATTR_CACHE            115   /* number of sequence numbers cached */
#define KCI_ATTR_ORDER            116     /* whether the sequence is ordered */
#define KCI_ATTR_HW_MARK          117                     /* high-water mark */
#define KCI_ATTR_TYPE_SCHEMA      118                  /* type's schema name */
#define KCI_ATTR_TIMESTAMP        119             /* timestamp of the object */
#define KCI_ATTR_NUM_ATTRS        120                /* number of sttributes */
#define KCI_ATTR_NUM_PARAMS       121                /* number of parameters */
#define KCI_ATTR_OBJID            122       /* object id for a table or view */
#define KCI_ATTR_PTYPE            123           /* type of info described by */
#define KCI_ATTR_PARAM            124                /* parameter descriptor */
#define KCI_ATTR_OVERLOAD_ID      125     /* overload ID for funcs and procs */
#define KCI_ATTR_TABLESPACE       126                    /* table name space */
#define KCI_ATTR_TDO              127                       /* TDO of a type */
#define KCI_ATTR_LTYPE            128                           /* list type */
#define KCI_ATTR_PARSE_ERROR_OFFSET 129                /* Parse Error offset */
#define KCI_ATTR_IS_TEMPORARY     130          /* whether table is temporary */
#define KCI_ATTR_IS_TYPED         131              /* whether table is typed */
#define KCI_ATTR_DURATION         132         /* duration of temporary table */
#define KCI_ATTR_IS_INVOKER_RIGHTS 133                  /* is invoker rights */
#define KCI_ATTR_OBJ_NAME         134           /* top level schema obj name */
#define KCI_ATTR_OBJ_SCHEMA       135                         /* schema name */
#define KCI_ATTR_OBJ_ID           136          /* top level schema object id */
#define KCI_ATTR_LIST_PKG_TYPES   137  /* parameter of the package type list */

	/* For values 137 - 141, see DirPathAPI attribute section in this file */


#define KCI_ATTR_TRANS_TIMEOUT              142       /* transaction timeout */
#define KCI_ATTR_SERVER_STATUS              143/* state of the server handle */
#define KCI_ATTR_STATEMENT                  144 /* statement txt in stmt hdl */

	/* For value 145, see DirPathAPI attribute section in this file */

#define KCI_ATTR_DEQCOND                    146         /* dequeue condition */
#define KCI_ATTR_RESERVED_2                 147                  /* reserved */


#define KCI_ATTR_SUBSCR_RECPT               148 /* recepient of subscription */
#define KCI_ATTR_SUBSCR_RECPTPROTO          149    /* protocol for recepient */

	/* For values 150 - 151, see DirPathAPI attribute section in this file */

#define KCI_ATTR_LDAP_HOST       153              /* LDAP host to connect to */
#define KCI_ATTR_LDAP_PORT       154              /* LDAP port to connect to */
#define KCI_ATTR_BIND_DN         155                              /* bind DN */
#define KCI_ATTR_LDAP_CRED       156       /* credentials to connect to LDAP */
#define KCI_ATTR_WALL_LOC        157               /* client wallet location */
#define KCI_ATTR_LDAP_AUTH       158           /* LDAP authentication method */
#define KCI_ATTR_LDAP_CTX        159        /* LDAP adminstration context DN */
#define KCI_ATTR_SERVER_DNS      160      /* list of registration server DNs */

#define KCI_ATTR_DN_COUNT        161             /* the number of server DNs */
#define KCI_ATTR_SERVER_DN       162                  /* server DN attribute */

#define KCI_ATTR_MAXCHAR_SIZE               163     /* max char size of data */

#define KCI_ATTR_CURRENT_POSITION           164 /* for scrollable result sets*/

	/* Added to get attributes for ref cursor to statement handle */
#define KCI_ATTR_RESERVED_3                 165                  /* reserved */
#define KCI_ATTR_RESERVED_4                 166                  /* reserved */

	/* For value 167, see DirPathAPI attribute section in this file */

#define KCI_ATTR_DIGEST_ALGO                168          /* digest algorithm */
#define KCI_ATTR_CERTIFICATE                169               /* certificate */
#define KCI_ATTR_SIGNATURE_ALGO             170       /* signature algorithm */
#define KCI_ATTR_CANONICAL_ALGO             171    /* canonicalization algo. */
#define KCI_ATTR_PRIVATE_KEY                172               /* private key */
#define KCI_ATTR_DIGEST_VALUE               173              /* digest value */
#define KCI_ATTR_SIGNATURE_VAL              174           /* signature value */
#define KCI_ATTR_SIGNATURE                  175                 /* signature */

	/* attributes for setting KCI stmt caching specifics in p_svctx */
#define KCI_ATTR_STMTCACHESIZE              176     /* size of the stm cache */

	/* --------------------------- Connection Pool Attributes ------------------ */
#define KCI_ATTR_CONN_NOWAIT               178
#define KCI_ATTR_CONN_BUSY_COUNT            179
#define KCI_ATTR_CONN_OPEN_COUNT            180
#define KCI_ATTR_CONN_TIMEOUT               181
#define KCI_ATTR_STMT_STATE                 182
#define KCI_ATTR_CONN_MIN                   183
#define KCI_ATTR_CONN_MAX                   184
#define KCI_ATTR_CONN_INCR                  185

	/* For value 187, see DirPathAPI attribute section in this file */

#define KCI_ATTR_NUM_OPEN_STMTS             188     /* open stmts in session */
#define KCI_ATTR_DESCRIBE_NATIVE            189  /* get native info via desc */

#define KCI_ATTR_BIND_COUNT                 190   /* number of bind postions */
#define KCI_ATTR_HANDLE_POSITION            191 /* pos of bind/define handle */
#define KCI_ATTR_RESERVED_5                 192                 /* reserverd */
#define KCI_ATTR_SERVER_BUSY                193 /* call in progress on server*/

	/* For value 194, see DirPathAPI attribute section in this file */

	/* notification presentation for recipient */
#define KCI_ATTR_SUBSCR_RECPTPRES           195
#define KCI_ATTR_TRANSFORMATION             196 /* AQ message transformation */

#define KCI_ATTR_ROWS_FETCHED               197 /* rows fetched in last call */

	/* --------------------------- Snapshot attributes ------------------------- */
#define KCI_ATTR_SCN_BASE                   198             /* snapshot base */
#define KCI_ATTR_SCN_WRAP                   199             /* snapshot wrap */

	/* --------------------------- Miscellanous attributes --------------------- */
#define KCI_ATTR_RESERVED_6                 200                  /* reserved */
#define KCI_ATTR_READONLY_TXN               201           /* txn is readonly */
#define KCI_ATTR_RESERVED_7                 202                  /* reserved */
#define KCI_ATTR_ERRONEOUS_COLUMN           203 /* position of erroneous col */
#define KCI_ATTR_RESERVED_8                 204                  /* reserved */
#define KCI_ATTR_ASM_VOL_SPRT               205     /* ASM volume supported? */

	/* For value 206, see DirPathAPI attribute section in this file */

#define KCI_ATTR_INST_TYPE                  207      /* oracle instance type */
	/******USED attribute 208 for  KCI_ATTR_SPOOL_STMTCACHESIZE*******************/

#define KCI_ATTR_ENV_UTF16                  209     /* is env in utf16 mode? */
#define KCI_ATTR_RESERVED_9                 210                  /* reserved */
#define KCI_ATTR_RESERVED_10                211                  /* reserved */

	/* For values 212 and 213, see DirPathAPI attribute section in this file */

#define KCI_ATTR_RESERVED_12                214                  /* reserved */
#define KCI_ATTR_RESERVED_13                215                  /* reserved */
#define KCI_ATTR_IS_EXTERNAL                216 /* whether table is external */


	/* -------------------------- SQLSTMT Handle Attributes ------------------ */

#define KCI_ATTR_RESERVED_15                217                  /* reserved */
#define KCI_ATTR_STMT_IS_RETURNING          218 /* stmt has returning clause */
#define KCI_ATTR_RESERVED_16                219                  /* reserved */
#define KCI_ATTR_RESERVED_17                220                  /* reserved */
#define KCI_ATTR_RESERVED_18                221                  /* reserved */

	/* --------------------------- session attributes ---------------------------*/
#define KCI_ATTR_RESERVED_19                222                  /* reserved */
#define KCI_ATTR_RESERVED_20                223                  /* reserved */
#define KCI_ATTR_CURRENT_SCHEMA             224            /* Current Schema */
#define KCI_ATTR_RESERVED_21                415                  /* reserved */
#define KCI_ATTR_LAST_LOGON_TIME_UTC        463/* Last Successful Logon Time */

	/* ------------------------- notification subscription ----------------------*/
#define KCI_ATTR_SUBSCR_QOSFLAGS            225                 /* QOS flags */
#define KCI_ATTR_SUBSCR_PAYLOADCBK          226          /* Payload callback */
#define KCI_ATTR_SUBSCR_TIMEOUT             227                   /* Timeout */
#define KCI_ATTR_SUBSCR_NAMESPACE_CTX       228         /* Namespace context */
#define KCI_ATTR_SUBSCR_CQ_QOSFLAGS         229
	/* change notification (CQ) specific QOS flags */
#define KCI_ATTR_SUBSCR_CQ_REGID            230
	/* change notification registration id */
#define KCI_ATTR_SUBSCR_NTFN_GROUPING_CLASS        231/* ntfn grouping class */
#define KCI_ATTR_SUBSCR_NTFN_GROUPING_VALUE        232/* ntfn grouping value */
#define KCI_ATTR_SUBSCR_NTFN_GROUPING_TYPE         233 /* ntfn grouping type */
#define KCI_ATTR_SUBSCR_NTFN_GROUPING_START_TIME   234/* ntfn grp start time */
#define KCI_ATTR_SUBSCR_NTFN_GROUPING_REPEAT_COUNT 235 /* ntfn grp rep count */
#define KCI_ATTR_AQ_NTFN_GROUPING_MSGID_ARRAY      236 /* aq grp msgid array */
#define KCI_ATTR_AQ_NTFN_GROUPING_COUNT            237  /* ntfns recd in grp */

	/* ----------------------- row callback attributes ------------------------- */
#define KCI_ATTR_BIND_ROWCBK                301         /* bind row callback */
#define KCI_ATTR_BIND_ROWCTX                302 /* ctx for bind row callback */
#define KCI_ATTR_SKIP_BUFFER                303  /* skip buffer in array ops */

	/* ----------------------- XStream API attributes -------------------------- */
#define KCI_ATTR_XSTREAM_ACK_INTERVAL       350      /* XStream ack interval */
#define KCI_ATTR_XSTREAM_IDLE_TIMEOUT       351      /* XStream idle timeout */

	/*-----  Db Change Notification (CQ) statement handle attributes------------ */
#define KCI_ATTR_CQ_QUERYID               304
	/* ------------- DB Change Notification reg handle attributes ---------------*/
#define KCI_ATTR_CHNF_TABLENAMES          401 /* out: array of table names   */
#define KCI_ATTR_CHNF_ROWIDS              402     /* in: rowids needed */ 
#define KCI_ATTR_CHNF_OPERATIONS          403
	/* in: notification operation filter*/
#define KCI_ATTR_CHNF_CHANGELAG           404
	/* txn lag between notifications  */

	/* DB Change: Notification Descriptor attributes -----------------------*/
#define KCI_ATTR_CHDES_DBNAME            405    /* source database    */
#define KCI_ATTR_CHDES_NFYTYPE           406    /* notification type flags */
#define KCI_ATTR_CHDES_XID               407    /* XID  of the transaction */
#define KCI_ATTR_CHDES_TABLE_CHANGES     408/* array of table chg descriptors*/

#define KCI_ATTR_CHDES_TABLE_NAME        409    /* table name */
#define KCI_ATTR_CHDES_TABLE_OPFLAGS     410    /* table operation flags */
#define KCI_ATTR_CHDES_TABLE_ROW_CHANGES 411   /* array of changed rows   */
#define KCI_ATTR_CHDES_ROW_ROWID         412   /* rowid of changed row    */
#define KCI_ATTR_CHDES_ROW_OPFLAGS       413   /* row operation flags     */

	/* SQLSTMT handle attribute for db change notification */
#define KCI_ATTR_CHNF_REGHANDLE          414   /* IN: subscription handle  */
#define KCI_ATTR_NETWORK_FILE_DESC       415   /* network file descriptor */

	/* client name for single session proxy */
#define KCI_ATTR_PROXY_CLIENT            416

	/* 415 is already taken - see KCI_ATTR_RESERVED_21 */

	/* TDE attributes on the Table */
#define KCI_ATTR_TABLE_ENC         417/* does table have any encrypt columns */
#define KCI_ATTR_TABLE_ENC_ALG     418         /* Table encryption Algorithm */
#define KCI_ATTR_TABLE_ENC_ALG_ID  419 /* Internal Id of encryption Algorithm*/

	/* -------- Attributes related to SQLSTMT cache callback ----------------- */
#define KCI_ATTR_STMTCACHE_CBKCTX           420    /* opaque context on stmt */
#define KCI_ATTR_STMTCACHE_CBK              421 /* callback fn for stmtcache */

	/*---------------- Query change descriptor attributes -----------------------*/
#define KCI_ATTR_CQDES_OPERATION 422
#define KCI_ATTR_CQDES_TABLE_CHANGES 423
#define KCI_ATTR_CQDES_QUERYID 424


#define KCI_ATTR_CHDES_QUERIES 425 /* Top level change desc array of queries */

	/* Please use from 143 */

	/* -------- Internal statement attributes ------- */
#define KCI_ATTR_RESERVED_26                422      

	/* 424 is used by KCI_ATTR_DRIVER_NAME */
	/* --------- Attributes added to support server side session pool ---------- */
#define KCI_ATTR_CONNECTION_CLASS  425
#define KCI_ATTR_PURITY            426

#define KCI_ATTR_PURITY_DEFAULT    0x00
#define KCI_ATTR_PURITY_NEW        0x01
#define KCI_ATTR_PURITY_SELF       0x02

	/* -------- Attributes for Times Ten --------------------------*/
#define KCI_ATTR_RESERVED_28               426                   /* reserved */
#define KCI_ATTR_RESERVED_29               427                   /* reserved */
#define KCI_ATTR_RESERVED_30               428                   /* reserved */
#define KCI_ATTR_RESERVED_31               429                   /* reserved */
#define KCI_ATTR_RESERVED_32               430                   /* reserved */
#define KCI_ATTR_RESERVED_41               454                   /* reserved */

	/* ----------- Reserve internal attributes for workload replay  ------------ */
#define KCI_ATTR_RESERVED_33               433
#define KCI_ATTR_RESERVED_34               434

	/* statement attribute */
#define KCI_ATTR_RESERVED_36               444

	/* -------- Attributes for Network Session Time Out--------------------------*/
#define KCI_ATTR_SEND_TIMEOUT               435           /* NS send timeout */
#define KCI_ATTR_RECEIVE_TIMEOUT            436        /* NS receive timeout */

	/*--------- Attributes related to LOB prefetch------------------------------ */
#define KCI_ATTR_DEFAULT_LOBPREFETCH_SIZE     438   /* default prefetch size */
#define KCI_ATTR_LOBPREFETCH_SIZE             439           /* prefetch size */
#define KCI_ATTR_LOBPREFETCH_LENGTH           440 /* prefetch length & chunk */

	/*--------- Attributes related to LOB Deduplicate Regions ------------------ */
#define KCI_ATTR_LOB_REGION_PRIMARY       442         /* Primary LOB Locator */
#define KCI_ATTR_LOB_REGION_PRIMOFF       443     /* Offset into Primary LOB */ 
#define KCI_ATTR_LOB_REGION_OFFSET        445               /* Region Offset */
#define KCI_ATTR_LOB_REGION_LENGTH        446   /* Region Length Bytes/Chars */
#define KCI_ATTR_LOB_REGION_MIME          447            /* Region mime type */

	/*--------------------Attribute to fetch ROWID ------------------------------*/
#define KCI_ATTR_FETCH_ROWID                 448

	/* server attribute */
#define KCI_ATTR_RESERVED_37              449

	/*-------------------Attributes for KCI column security-----------------------*/
#define KCI_ATTR_NO_COLUMN_AUTH_WARNING  450
#define KCI_ATTR_XDS_POLICY_STATUS       451

#define KCI_XDS_POLICY_NONE     0
#define KCI_XDS_POLICY_ENABLED  1
#define KCI_XDS_POLICY_UNKNOWN  2

	/* --------------- ip address attribute in environment handle -------------- */
#define KCI_ATTR_SUBSCR_IPADDR         452       /* ip address to listen on  */

	/* server attribute */
#define KCI_ATTR_RESERVED_40           453  
#define KCI_ATTR_RESERVED_42           455
#define KCI_ATTR_RESERVED_43           456

	/* statement attribute */
#define KCI_ATTR_UB8_ROW_COUNT         457         /* ub8 value of row count */

	/* ------------- round trip callback attributes in the process  handle ----- */
#define KCI_ATTR_RESERVED_458          458                       /* reserved */
#define KCI_ATTR_RESERVED_459          459                       /* reserved */

	/* invisible columns attributes */
#define KCI_ATTR_SHOW_INVISIBLE_COLUMNS    460  /* invisible columns support */
#define KCI_ATTR_INVISIBLE_COL             461  /* invisible columns support */

	/* support at most once transaction semantics */
#define KCI_ATTR_LTXID                     462     /* logical transaction id */

	/* statement handle attribute */
#define KCI_ATTR_IMPLICIT_RESULT_COUNT     463

#define KCI_ATTR_RESERVED_464              464
#define KCI_ATTR_RESERVED_465              465
#define KCI_ATTR_RESERVED_466              466
#define KCI_ATTR_RESERVED_467              467

	/* SQL translation profile session attribute */
#define KCI_ATTR_SQL_TRANSLATION_PROFILE   468

	/* Per Iteration array DML rowcount attribute */
#define KCI_ATTR_DML_ROW_COUNT_ARRAY       469
#define KCI_ATTR_RESERVED_470              470

	/* session handle attribute */
#define KCI_ATTR_MAX_OPEN_CURSORS          471

	/* Can application failover and recover from this error?
	* e.g. ORA-03113 is recoverable while ORA-942 (table or view does not exist)
	* is not.
	*/
#define KCI_ATTR_ERROR_IS_RECOVERABLE      472

	/* ONS specific private attribute  */
#define KCI_ATTR_RESERVED_473              473 

	/* Attribute to check if ILM Write Access Tracking is enabled or not */
#define KCI_ATTR_ILM_TRACK_WRITE    474

	/* Notification subscription failure callback and context */
#define KCI_ATTR_SUBSCR_FAILURE_CBK   477
#define KCI_ATTR_SUBSCR_FAILURE_CTX   478

	/* Reserved */
#define KCI_ATTR_RESERVED_479              479
#define KCI_ATTR_RESERVED_480              480
#define KCI_ATTR_RESERVED_481              481
#define KCI_ATTR_RESERVED_482              482

	/* A SQL translation profile with FOREIGN_SQL_SYNTAX attribute is set in the
	* database session.
	*/
#define KCI_ATTR_TRANS_PROFILE_FOREIGN     483

	/* is a transaction active on the session? */
#define KCI_ATTR_TRANSACTION_IN_PROGRESS   484

	/* add attribute for DBOP: DataBase OPeration */
#define KCI_ATTR_DBOP                      485

	/* FAN-HA private attribute */
#define KCI_ATTR_RESERVED_486              486

	/* reserved */
#define KCI_ATTR_RESERVED_487              487

#define KCI_ATTR_RESERVED_488              488

#define KCI_ATTR_VARTYPE_MAXLEN_COMPAT     489

	/* Max Lifetime for session */
#define KCI_ATTR_SPOOL_MAX_LIFETIME_SESSION 490

#define KCI_ATTR_RESERVED_491              491

#define KCI_ATTR_RESERVED_492              492

#define KCI_ATTR_RESERVED_493              493                   /* reserved */

#define KCI_ATTR_ITERS_PROCESSED           494

#define KCI_ATTR_BREAK_ON_NET_TIMEOUT      495           /* Break on timeout */

	/* NEW ATTRIBUTES MUST BE ADDED  HERE */

	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/
#define KCI_ATTR_DIRPATH_RESERVED_9        2000                  /* reserved */

	/*------ Attribute for 10iR2 for column encryption for Direct Path API ------*/
#define KCI_ATTR_DIRPATH_RESERVED_10       2001                  /* reserved */
#define KCI_ATTR_DIRPATH_RESERVED_11       2002                  /* reserved */

	/*------ Attribute to determine last column successfully converted ----------*/
#define KCI_ATTR_CURRENT_ERRCOL            2003      /* current error column */

	/*--Attributes for 11gR1 for multiple subtype support in Direct Path API - */
#define KCI_ATTR_DIRPATH_SUBTYPE_INDEX     2004  /* sbtyp indx for attribute */

#define KCI_ATTR_DIRPATH_RESERVED_12       2005                  /* reserved */
#define KCI_ATTR_DIRPATH_RESERVED_13       2006                  /* reserver */

	/*--Attribute for partitioning constraint optimization in Direct Path API  */
#define KCI_ATTR_DIRPATH_RESERVED_14       2007                  /* reserved */

	/*--Attribute for interval partitioning in Direct Path API  */
#define KCI_ATTR_DIRPATH_RESERVED_15       2008                  /* reserved */

	/*--Attribute for interval partitioning in Direct Path API  */
#define KCI_ATTR_DIRPATH_RESERVED_16       2009                  /* reserved */

	/*--Attribute for allowing parallel lob loads in Direct Path API */
#define KCI_ATTR_DIRPATH_RESERVED_17       2010                  /* reserved */

	/*--Attribute for process order number of table being loaded/unloaded        */
#define KCI_ATTR_DIRPATH_RESERVED_18       2011                  /* reserved */

#define KCI_ATTR_DIRPATH_RESERVED_19       2012                  /* reserved */

#define KCI_ATTR_DIRPATH_NO_INDEX_ERRORS   2013                  /* reserved */

	/*--Attribute for private sqlldr no index errors                             */
#define KCI_ATTR_DIRPATH_RESERVED_20       2014                  /* reserved */

	/*--Attribute for private sqlldr partition memory limit                      */
#define KCI_ATTR_DIRPATH_RESERVED_21       2015                  /* reserved */

#define KCI_ATTR_DIRPATH_RESERVED_22       2016                  /* reserved */

	/*--Attribute to use caller's transaction rather than starting on in kpodpp  */
#define KCI_ATTR_DIRPATH_USE_ACTIVE_TRANS  2017                  /* reserved */

	/*--Attribute  for recnum column                                             */
#define KCI_ATTR_DIRPATH_RESERVED_23       2018                  /* reserved */

	/*--Attribute  for long varchar columns                                      */
#define KCI_ATTR_DIRPATH_RESERVED_24       2019                  /* reserved */

	/*--Attribute for reject replacement chars                                   */
#define KCI_ATTR_DIRPATH_REJECT_ROWS_REPCHR 2020

	/*--Attribute for CLOB xmltype dumpfile format                               */
#define KCI_ATTR_DIRPATH_RESERVED_25       2021                  /* reserved */

	/*--Attribute for PGA spill limit                                            */
#define KCI_ATTR_DIRPATH_PGA_LIM           2022

	/*--Atribute for number of passes to load partitions                         */
#define KCI_ATTR_DIRPATH_SPILL_PASSES      2023

	/*--Attribute for direct path flags                                          */
#define KCI_ATTR_DIRPATH_FLAGS             2024
	/* Assign new public flag to next higher bit in the 16 low order bits. e.g.
	* define KCI_ATTR_DIRPATH_FLAGS_<first>   0x00000001
	* define KCI_ATTR_DIRPATH_FLAGS_<second>  0x00000002
	* define KCI_ATTR_DIRPATH_FLAGS_<third>   0x00000004
	*/
#define KCI_ATTR_DIRPATH_FLAGS_RESERVED    0xFFFF0000      /* Reserved flags */


	/* DB Change: Event types ---------------*/
#define KCI_EVENT_NONE 0x0                                           /* None */
#define KCI_EVENT_STARTUP 0x1                            /* Startup database */
#define KCI_EVENT_SHUTDOWN 0x2                          /* Shutdown database */
#define KCI_EVENT_SHUTDOWN_ANY 0x3                       /* Startup instance */
#define KCI_EVENT_DROP_DB 0x4                            /* Drop database    */
#define KCI_EVENT_DEREG 0x5                     /* Subscription deregistered */
#define KCI_EVENT_OBJCHANGE 0x6                /* Object change notification */
#define KCI_EVENT_QUERYCHANGE 0x7                     /* query result change */

	/* DB Change: Operation types -----------*/
#define KCI_OPCODE_ALLROWS 0x1                      /* all rows invalidated  */
#define KCI_OPCODE_ALLOPS 0x0                /* interested in all operations */
#define KCI_OPCODE_INSERT 0x2                                     /*  INSERT */
#define KCI_OPCODE_UPDATE 0x4                                     /*  UPDATE */
#define KCI_OPCODE_DELETE 0x8                                      /* DELETE */
#define KCI_OPCODE_ALTER 0x10                                       /* ALTER */
#define KCI_OPCODE_DROP 0x20                                   /* DROP TABLE */
#define KCI_OPCODE_UNKNOWN 0x40                           /* GENERIC/ UNKNOWN*/

	/* -------- client side character and national character set ids ----------- */
#define KCI_ATTR_ENV_CHARSET_ID   KCI_ATTR_CHARSET_ID   /* charset id in env */
#define KCI_ATTR_ENV_NCHARSET_ID  KCI_ATTR_NCHARSET_ID /* ncharset id in env */

	/* ----------------------- ha event callback attributes -------------------- */
#define KCI_ATTR_EVTCBK                     304               /* ha callback */
#define KCI_ATTR_EVTCTX                     305       /* ctx for ha callback */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/* ------------------ User memory attributes (all handles) ----------------- */
#define KCI_ATTR_USER_MEMORY               306     /* pointer to user memory */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/* ------- unauthorised access and user action auditing banners ------------ */
#define KCI_ATTR_ACCESS_BANNER              307             /* access banner */
#define KCI_ATTR_AUDIT_BANNER               308              /* audit banner */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/* ----------------- port no attribute in environment  handle  ------------- */
#define KCI_ATTR_SUBSCR_PORTNO              390  /* port no to listen        */

#define KCI_ATTR_RESERVED_35                437
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/*------------- Supported Values for protocol for recepient -----------------*/
#define KCI_SUBSCR_PROTO_KCI                0                         /* oci */
#define KCI_SUBSCR_PROTO_MAIL               1                        /* mail */
#define KCI_SUBSCR_PROTO_SERVER             2                      /* server */
#define KCI_SUBSCR_PROTO_HTTP               3                        /* http */
#define KCI_SUBSCR_PROTO_MAX                4       /* max current protocols */

	/*------------- Supported Values for presentation for recepient -------------*/
#define KCI_SUBSCR_PRES_DEFAULT             0                     /* default */
#define KCI_SUBSCR_PRES_XML                 1                         /* xml */
#define KCI_SUBSCR_PRES_MAX                 2   /* max current presentations */

	/*------------- Supported QOS values for notification registrations ---------*/
#define KCI_SUBSCR_QOS_RELIABLE             0x01                 /* reliable */
#define KCI_SUBSCR_QOS_PAYLOAD              0x02         /* payload delivery */
#define KCI_SUBSCR_QOS_REPLICATE            0x04    /* replicate to director */
	/* internal qos - 12c secure ntfns with client initiated connections */
#define KCI_SUBSCR_QOS_SECURE               0x08  /* secure payload delivery */
#define KCI_SUBSCR_QOS_PURGE_ON_NTFN        0x10      /* purge on first ntfn */
#define KCI_SUBSCR_QOS_MULTICBK             0x20  /* multi instance callback */
	/* 0x40 is used for a internal flag */
#define KCI_SUBSCR_QOS_HAREG                0x80                   /* HA reg */
	/* non-durable registration. For now supported only with secure ntfns */
#define KCI_SUBSCR_QOS_NONDURABLE           0x100         /* non-durable reg */
#define KCI_SUBSCR_QOS_ASYNC_DEQ            0x200         /* Asyncronous Deq */
#define KCI_SUBSCR_QOS_AUTO_ACK             0x400    /* auto acknowledgement */
#define KCI_SUBSCR_QOS_TX_ACK               0x800         /* transacted acks */

	/* ----QOS flags specific to change notification/ continuous queries CQ -----*/
#define KCI_SUBSCR_CQ_QOS_QUERY  0x01            /* query level notification */
#define KCI_SUBSCR_CQ_QOS_BEST_EFFORT 0x02       /* best effort notification */
#define KCI_SUBSCR_CQ_QOS_CLQRYCACHE 0x04            /* client query caching */

	/*------------- Supported Values for notification grouping class ------------*/
#define KCI_SUBSCR_NTFN_GROUPING_CLASS_TIME 1                        /* time */

	/*------------- Supported Values for notification grouping type -------------*/
#define KCI_SUBSCR_NTFN_GROUPING_TYPE_SUMMARY 1                   /* summary */
#define KCI_SUBSCR_NTFN_GROUPING_TYPE_LAST    2                      /* last */

	/* ----- Temporary attribute value for UCS2/UTF16 character set ID -------- */
#define KCI_UCS2ID            1000                        /* UCS2 charset ID */
#define KCI_UTF16ID           1000                       /* UTF16 charset ID */

	/* -------------------------- Implicit Result types ------------------------ */
#define KCI_RESULT_TYPE_SELECT                1

	/*============================== End KCI Attribute Types ====================*/

	/*---------------- Server Handle Attribute Values ---------------------------*/

	/* KCI_ATTR_SERVER_STATUS */
#define KCI_SERVER_NOT_CONNECTED        0x0 
#define KCI_SERVER_NORMAL               0x1 

	/*---------------------------------------------------------------------------*/

	/*------------------------- Supported Namespaces  ---------------------------*/
#define KCI_SUBSCR_NAMESPACE_ANONYMOUS   0            /* Anonymous Namespace */
#define KCI_SUBSCR_NAMESPACE_AQ          1                /* Advanced Queues */
#define KCI_SUBSCR_NAMESPACE_DBCHANGE    2            /* change notification */
#define KCI_SUBSCR_NAMESPACE_RESERVED1   3
#define KCI_SUBSCR_NAMESPACE_MAX         4          /* Max Name Space Number */


	/*-------------------------Credential Types----------------------------------*/
#define KCI_CRED_RDBMS       1                 /* database username/password */
#define KCI_CRED_EXT         2            /* externally provided credentials */
#define KCI_CRED_PROXY       3                       /* proxy authentication */
#define KCI_CRED_RESERVED_1  4                                   /* reserved */
#define KCI_CRED_RESERVED_2  5                                   /* reserved */
#define KCI_CRED_RESERVED_3  6                                   /* reserved */
	/*---------------------------------------------------------------------------*/

	/*------------------------Error Return Values--------------------------------*/
#define KCI_SUCCESS 0                      /* maps to SQL_SUCCESS of SAG CLI */
#define KCI_SUCCESS_WITH_INFO 1             /* maps to SQL_SUCCESS_WITH_INFO */
#define KCI_RESERVED_FOR_INT_USE 200                            /* reserved */ 
#define KCI_NO_DATA 100                               /* maps to SQL_NO_DATA */
#define KCI_ERROR -1                                    /* maps to SQL_ERROR */
#define KCI_INVALID_HANDLE -2                  /* maps to SQL_INVALID_HANDLE */
#define KCI_NEED_DATA 99                            /* maps to SQL_NEED_DATA */
#define KCI_STILL_EXECUTING -3123                   /* KCI would block error */
	/*---------------------------------------------------------------------------*/

	/*--------------------- User Callback Return Values -------------------------*/
#define KCI_CONTINUE -24200    /* Continue with the body of the KCI function */
#define KCI_ROWCBK_DONE   -24201              /* done with user row callback */
	/*---------------------------------------------------------------------------*/

	/*------------------DateTime and Interval check Error codes------------------*/

	/* DateTime Error Codes used by KCIDateTimeCheck() */
#define   KCI_DT_INVALID_DAY         0x1                          /* Bad day */
#define   KCI_DT_DAY_BELOW_VALID     0x2      /* Bad DAy Low/high bit (1=low)*/
#define   KCI_DT_INVALID_MONTH       0x4                       /*  Bad MOnth */
#define   KCI_DT_MONTH_BELOW_VALID   0x8   /* Bad MOnth Low/high bit (1=low) */
#define   KCI_DT_INVALID_YEAR        0x10                        /* Bad YeaR */
#define   KCI_DT_YEAR_BELOW_VALID    0x20  /*  Bad YeaR Low/high bit (1=low) */
#define   KCI_DT_INVALID_HOUR        0x40                       /*  Bad HouR */
#define   KCI_DT_HOUR_BELOW_VALID    0x80   /* Bad HouR Low/high bit (1=low) */
#define   KCI_DT_INVALID_MINUTE      0x100                     /* Bad MiNute */
#define   KCI_DT_MINUTE_BELOW_VALID  0x200 /*Bad MiNute Low/high bit (1=low) */
#define   KCI_DT_INVALID_SECOND      0x400                    /*  Bad SeCond */
#define   KCI_DT_SECOND_BELOW_VALID  0x800  /*bad second Low/high bit (1=low)*/
#define   KCI_DT_DAY_MISSING_FROM_1582 0x1000     
	/*  Day is one of those "missing" from 1582 */
#define   KCI_DT_YEAR_ZERO           0x2000       /* Year may not equal zero */
#define   KCI_DT_INVALID_TIMEZONE    0x4000                 /*  Bad Timezone */
#define   KCI_DT_INVALID_FORMAT      0x8000         /* Bad date format input */


	/* Interval Error Codes used by KCIInterCheck() */
#define   KCI_INTER_INVALID_DAY         0x1                       /* Bad day */
#define   KCI_INTER_DAY_BELOW_VALID     0x2  /* Bad DAy Low/high bit (1=low) */
#define   KCI_INTER_INVALID_MONTH       0x4                     /* Bad MOnth */
#define   KCI_INTER_MONTH_BELOW_VALID   0x8 /*Bad MOnth Low/high bit (1=low) */
#define   KCI_INTER_INVALID_YEAR        0x10                     /* Bad YeaR */
#define   KCI_INTER_YEAR_BELOW_VALID    0x20 /*Bad YeaR Low/high bit (1=low) */
#define   KCI_INTER_INVALID_HOUR        0x40                     /* Bad HouR */
#define   KCI_INTER_HOUR_BELOW_VALID    0x80 /*Bad HouR Low/high bit (1=low) */
#define   KCI_INTER_INVALID_MINUTE      0x100                  /* Bad MiNute */
#define   KCI_INTER_MINUTE_BELOW_VALID  0x200 
	/*Bad MiNute Low/high bit(1=low) */
#define   KCI_INTER_INVALID_SECOND      0x400                  /* Bad SeCond */
#define   KCI_INTER_SECOND_BELOW_VALID  0x800   
	/*bad second Low/high bit(1=low) */
#define   KCI_INTER_INVALID_FRACSEC     0x1000      /* Bad Fractional second */
#define   KCI_INTER_FRACSEC_BELOW_VALID 0x2000  
	/* Bad fractional second Low/High */


	/*------------------------Parsing Syntax Types-------------------------------*/
#define KCI_V7_SYNTAX 2       /* V815 language - for backwards compatibility */
#define KCI_V8_SYNTAX 3       /* V815 language - for backwards compatibility */
#define KCI_NTV_SYNTAX 1    /* Use what so ever is the native lang of server */
#define KCI_FOREIGN_SYNTAX UB4MAXVAL /* Foreign syntax - require translation */
	/* these values must match the values defined in kpul.h */
	/*---------------------------------------------------------------------------*/

	/*------------------------(Scrollable Cursor) Fetch Options-------------------
	* For non-scrollable cursor, the only valid (and default) orientation is
	* KCI_FETCH_NEXT
	*/
#define KCI_FETCH_CURRENT    0x00000001      /* refetching current position  */
#define KCI_FETCH_NEXT       0x00000002                          /* next row */
#define KCI_FETCH_FIRST      0x00000004       /* first row of the result set */
#define KCI_FETCH_LAST       0x00000008    /* the last row of the result set */
#define KCI_FETCH_PRIOR      0x00000010  /* previous row relative to current */
#define KCI_FETCH_ABSOLUTE   0x00000020        /* absolute offset from first */
#define KCI_FETCH_RELATIVE   0x00000040        /* offset relative to current */
#define KCI_FETCH_RESERVED_1 0x00000080                          /* reserved */
#define KCI_FETCH_RESERVED_2 0x00000100                          /* reserved */
#define KCI_FETCH_RESERVED_3 0x00000200                          /* reserved */
#define KCI_FETCH_RESERVED_4 0x00000400                          /* reserved */
#define KCI_FETCH_RESERVED_5 0x00000800                          /* reserved */
#define KCI_FETCH_RESERVED_6 0x00001000                          /* reserved */

	/*---------------------------------------------------------------------------*/

	/*------------------------Bind and Define Options----------------------------*/
#define KCI_SB2_IND_PTR       0x00000001                           /* unused */
#define KCI_DATA_AT_EXEC      0x00000002             /* data at execute time */
#define KCI_DYNAMIC_FETCH     0x00000002                /* fetch dynamically */
#define KCI_PIECEWISE         0x00000004          /* piecewise DMLs or fetch */
#define KCI_DEFINE_RESERVED_1 0x00000008                         /* reserved */
#define KCI_BIND_RESERVED_2   0x00000010                         /* reserved */
#define KCI_DEFINE_RESERVED_2 0x00000020                         /* reserved */
#define KCI_BIND_SOFT         0x00000040              /* soft bind or define */
#define KCI_DEFINE_SOFT       0x00000080              /* soft bind or define */
#define KCI_BIND_RESERVED_3   0x00000100                         /* reserved */
#define KCI_IOV               0x00000200   /* For scatter gather bind/define */
	/*---------------------------------------------------------------------------*/

	/*-----------------------------  Various Modes ------------------------------*/
#define KCI_DEFAULT         0x00000000 
	/* the default value for parameters and attributes */
	/*-------------KCIInitialize Modes / KCICreateEnvironment Modes -------------*/
#define KCI_THREADED        0x00000001      /* appl. in threaded environment */
#define KCI_OBJECT          0x00000002  /* application in object environment */
#define KCI_EVENTS          0x00000004  /* application is enabled for events */
#define KCI_RESERVED1       0x00000008                           /* reserved */
#define KCI_SHARED          0x00000010  /* the application is in shared mode */
#define KCI_RESERVED2       0x00000020                           /* reserved */
	/* The following *TWO* are only valid for KCICreateEnvironment call */
#define KCI_NO_UCB          0x00000040 /* No user callback called during ini */
#define KCI_NO_MUTEX        0x00000080 /* the environment handle will not be */
	/*  protected by a mutex internally */
#define KCI_SHARED_EXT      0x00000100              /* Used for shared forms */
	/************************** 0x00000200 free **********************************/
#define KCI_ALWAYS_BLOCKING 0x00000400    /* all connections always blocking */
	/************************** 0x00000800 free **********************************/
#define KCI_USE_LDAP        0x00001000            /* allow  LDAP connections */
#define KCI_REG_LDAPONLY    0x00002000              /* only register to LDAP */
#define KCI_UTF16           0x00004000        /* mode for all UTF16 metadata */
#define KCI_AFC_PAD_ON      0x00008000 
	/* turn on AFC blank padding when rlenp present */
#define KCI_ENVCR_RESERVED3 0x00010000                           /* reserved */
#define KCI_NEW_LENGTH_SEMANTICS  0x00020000   /* adopt new length semantics */
	/* the new length semantics, always bytes, is used by KCIEnvNlsCreate */
#define KCI_NO_MUTEX_STMT   0x00040000           /* Do not mutex stmt handle */
#define KCI_MUTEX_ENV_ONLY  0x00080000  /* Mutex only the environment handle */
#define KCI_SUPPRESS_NLS_VALIDATION   0x00100000  /* suppress nls validation */
	/* nls validation suppression is on by default;
	use KCI_ENABLE_NLS_VALIDATION to disable it */
#define KCI_MUTEX_TRY                 0x00200000    /* try and acquire mutex */
#define KCI_NCHAR_LITERAL_REPLACE_ON  0x00400000 /* nchar literal replace on */
#define KCI_NCHAR_LITERAL_REPLACE_OFF 0x00800000 /* nchar literal replace off*/
#define KCI_ENABLE_NLS_VALIDATION     0x01000000    /* enable nls validation */
#define KCI_ENVCR_RESERVED4           0x02000000                 /* reserved */
#define KCI_ENVCR_RESERVED5           0x04000000                 /* reserved */
#define KCI_ENVCR_RESERVED6           0x08000000                 /* reserved */
#define KCI_ENVCR_RESERVED7           0x10000000                 /* reserved */

	/* client initiated notification listener connections, applicable only for
	12c queues and above */
#define KCI_SECURE_NOTIFICATION        0x20000000 
#define KCI_DISABLE_DIAG               0x40000000     /* disable diagnostics */
	/*---------------------------------------------------------------------------*/
	/*------------------------KCIConnectionpoolCreate Modes----------------------*/

#define KCI_CPOOL_REINITIALIZE 0x111

	/*---------------------------------------------------------------------------*/
	/*--------------------------------- KCILogon2 Modes -------------------------*/

#define KCI_LOGON2_SPOOL       0x0001     /* Use session pool */
#define KCI_LOGON2_CPOOL       KCI_CPOOL  /* Use connection pool */
#define KCI_LOGON2_STMTCACHE   0x0004     /* Use Stmt Caching */
#define KCI_LOGON2_PROXY       0x0008     /* Proxy authentiaction */

	/*---------------------------------------------------------------------------*/
	/*------------------------- KCISessionPoolCreate Modes ----------------------*/

#define KCI_SPC_REINITIALIZE 0x0001   /* Reinitialize the session pool */
#define KCI_SPC_HOMOGENEOUS  0x0002   /* Session pool is homogeneneous */
#define KCI_SPC_STMTCACHE    0x0004   /* Session pool has stmt cache */
#define KCI_SPC_NO_RLB       0x0008 /* Do not enable Runtime load balancing. */ 

	/*---------------------------------------------------------------------------*/
	/*--------------------------- KCISessionGet Modes ---------------------------*/

#define KCI_SESSGET_SPOOL      0x0001     /* SessionGet called in SPOOL mode */
#define KCI_SESSGET_CPOOL      KCI_CPOOL  /* SessionGet called in CPOOL mode */
#define KCI_SESSGET_STMTCACHE  0x0004                 /* Use statement cache */
#define KCI_SESSGET_CREDPROXY  0x0008     /* SessionGet called in proxy mode */
#define KCI_SESSGET_CREDEXT    0x0010     
#define KCI_SESSGET_SPOOL_MATCHANY 0x0020
#define KCI_SESSGET_PURITY_NEW     0x0040 
#define KCI_SESSGET_PURITY_SELF    0x0080 
#define KCI_SESSGET_SYSDBA    0x0100    /* SessionGet with SYSDBA privileges */

	/*---------------------------------------------------------------------------*/
	/*------------------------ATTR Values for Session Pool-----------------------*/
	/* Attribute values for KCI_ATTR_SPOOL_GETMODE */
#define KCI_SPOOL_ATTRVAL_WAIT     0         /* block till you get a session */
#define KCI_SPOOL_ATTRVAL_NOWAIT   1    /* error out if no session avaliable */
#define KCI_SPOOL_ATTRVAL_FORCEGET 2  /* get session even if max is exceeded */

	/*---------------------------------------------------------------------------*/
	/*--------------------------- KCISessionRelease Modes -----------------------*/

#define KCI_SESSRLS_DROPSESS 0x0001                    /* Drop the Session */
#define KCI_SESSRLS_RETAG    0x0002                   /* Retag the session */

	/*---------------------------------------------------------------------------*/
	/*----------------------- KCISessionPoolDestroy Modes -----------------------*/

#define KCI_SPD_FORCE        0x0001       /* Force the sessions to terminate. 
	Even if there are some busy
	sessions close them */

	/*---------------------------------------------------------------------------*/
	/*----------------------------- SQLSTMT States ----------------------------*/

#define KCI_STMT_STATE_INITIALIZED  0x0001
#define KCI_STMT_STATE_EXECUTED     0x0002
#define KCI_STMT_STATE_END_OF_FETCH 0x0003

	/*---------------------------------------------------------------------------*/

	/*----------------------------- KCIMemStats Modes ---------------------------*/
#define KCI_MEM_INIT        0x01 
#define KCI_MEM_CLN         0x02 
#define KCI_MEM_FLUSH       0x04 
#define KCI_DUMP_HEAP       0x80

#define KCI_CLIENT_STATS    0x10 
#define KCI_SERVER_STATS    0x20 

	/*----------------------------- KCIEnvInit Modes ----------------------------*/
	/* NOTE: NO NEW MODES SHOULD BE ADDED HERE BECAUSE THE RECOMMENDED METHOD
	* IS TO USE THE NEW KCICreateEnvironment MODES.
	*/
#define KCI_ENV_NO_UCB 0x01         /* A user callback will not be called in
	KCIEnvInit() */
#define KCI_ENV_NO_MUTEX 0x08 /* the environment handle will not be protected
	by a mutex internally */

	/*---------------------------------------------------------------------------*/

	/*------------------------ Prepare Modes ------------------------------------*/
#define KCI_NO_SHARING        0x01      /* turn off statement handle sharing */
#define KCI_PREP_RESERVED_1   0x02                               /* reserved */
#define KCI_PREP_AFC_PAD_ON   0x04          /* turn on blank padding for AFC */
#define KCI_PREP_AFC_PAD_OFF  0x08         /* turn off blank padding for AFC */
	/*---------------------------------------------------------------------------*/

	/*---------------------------------------------------------------------------*/

	/*----------------------- Execution Modes -----------------------------------*/
#define KCI_BATCH_MODE             0x00000001 /* batch the oci stmt for exec */
#define KCI_EXACT_FETCH            0x00000002  /* fetch exact rows specified */
	/* #define                         0x00000004                      available */
#define KCI_STMT_SCROLLABLE_READONLY \
                                   0x00000008 /* if result set is scrollable */
#define KCI_DESCRIBE_ONLY          0x00000010 /* only describe the statement */
#define KCI_COMMIT_ON_SUCCESS      0x00000020  /* commit, if successful exec */
#define KCI_NON_BLOCKING           0x00000040                /* non-blocking */
#define KCI_BATCH_ERRORS           0x00000080  /* batch errors in array dmls */
#define KCI_PARSE_ONLY             0x00000100    /* only parse the statement */
#define KCI_EXACT_FETCH_RESERVED_1 0x00000200                    /* reserved */
#define KCI_SHOW_DML_WARNINGS      0x00000400   
	/* return KCI_SUCCESS_WITH_INFO for delete/update w/no where clause */
#define KCI_EXEC_RESERVED_2        0x00000800                    /* reserved */
#define KCI_DESC_RESERVED_1        0x00001000                    /* reserved */
#define KCI_EXEC_RESERVED_3        0x00002000                    /* reserved */
#define KCI_EXEC_RESERVED_4        0x00004000                    /* reserved */
#define KCI_EXEC_RESERVED_5        0x00008000                    /* reserved */
#define KCI_EXEC_RESERVED_6        0x00010000                    /* reserved */
#define KCI_RESULT_CACHE           0x00020000   /* hint to use query caching */
#define KCI_NO_RESULT_CACHE        0x00040000  /*hint to bypass query caching*/
#define KCI_EXEC_RESERVED_7        0x00080000                    /* reserved */
#define KCI_RETURN_ROW_COUNT_ARRAY 0x00100000 /* Per Iter DML Row Count mode */
	/*---------------------------------------------------------------------------*/

	/*------------------------Authentication Modes-------------------------------*/
#define KCI_MIGRATE         0x00000001            /* migratable auth context */
#define KCI_SYSDBA          0x00000002           /* for SYSDBA authorization */
#define KCI_SYSOPER         0x00000004          /* for SYSOPER authorization */
#define KCI_PRELIM_AUTH     0x00000008      /* for preliminary authorization */
#define KCIP_ICACHE         0x00000010             /* Private KCI cache mode */
#define KCI_AUTH_RESERVED_1 0x00000020                           /* reserved */
#define KCI_STMT_CACHE      0x00000040            /* enable KCI Stmt Caching */
#define KCI_STATELESS_CALL  0x00000080         /* stateless at call boundary */
#define KCI_STATELESS_TXN   0x00000100          /* stateless at txn boundary */
#define KCI_STATELESS_APP   0x00000200    /* stateless at user-specified pts */
#define KCI_AUTH_RESERVED_2 0x00000400                           /* reserved */
#define KCI_AUTH_RESERVED_3 0x00000800                           /* reserved */
#define KCI_AUTH_RESERVED_4 0x00001000                           /* reserved */
#define KCI_AUTH_RESERVED_5 0x00002000                           /* reserved */
#define KCI_SYSASM          0x00008000           /* for SYSASM authorization */
#define KCI_AUTH_RESERVED_6 0x00010000                           /* reserved */
#define KCI_SYSBKP          0x00020000        /* for SYSBACKUP authorization */
#define KCI_SYSDGD          0x00040000            /* for SYSDG authorization */
#define KCI_SYSKMT          0x00080000            /* for SYSKM authorization */

	/*---------------------------------------------------------------------------*/

	/*------------------------Session End Modes----------------------------------*/
#define KCI_SESSEND_RESERVED_1 0x0001                            /* reserved */
#define KCI_SESSEND_RESERVED_2 0x0002                            /* reserved */
	/*---------------------------------------------------------------------------*/

	/*------------------------Attach Modes---------------------------------------*/

	/* The following attach modes are the same as the UPI modes defined in
	* UPIDEF.H.  Do not use these values externally.
	*/

#define KCI_FASTPATH         0x0010              /* Attach in fast path mode */
#define KCI_ATCH_RESERVED_1  0x0020                              /* reserved */
#define KCI_ATCH_RESERVED_2  0x0080                              /* reserved */
#define KCI_ATCH_RESERVED_3  0x0100                              /* reserved */
#define KCI_CPOOL            0x0200  /* Attach using server handle from pool */
#define KCI_ATCH_RESERVED_4  0x0400                              /* reserved */
#define KCI_ATCH_RESERVED_5  0x2000                              /* reserved */
#define KCI_ATCH_ENABLE_BEQ  0x4000        /* Allow bequeath connect strings */
#define KCI_ATCH_RESERVED_6  0x8000                              /* reserved */
#define KCI_ATCH_RESERVED_7  0x10000                              /* reserved */
#define KCI_ATCH_RESERVED_8  0x20000                             /* reserved */

#define KCI_SRVATCH_RESERVED5 0x01000000                         /* reserved */
#define KCI_SRVATCH_RESERVED6 0x02000000                         /* reserved */

	/*---------------------KCIStmtPrepare2 Modes---------------------------------*/
#define KCI_PREP2_CACHE_SEARCHONLY    0x0010                  /* ONly Search */
#define KCI_PREP2_GET_PLSQL_WARNINGS  0x0020         /* Get PL/SQL warnings  */
#define KCI_PREP2_RESERVED_1          0x0040                     /* reserved */
#define KCI_PREP2_RESERVED_2          0x0080                     /* reserved */
#define KCI_PREP2_RESERVED_3          0x0100                     /* reserved */
#define KCI_PREP2_RESERVED_4          0x0200                     /* reserved */
#define KCI_PREP2_IMPL_RESULTS_CLIENT 0x0400  /* client for implicit results */
#define KCI_PREP2_RESERVED_5          0x0800                     /* reserved */

	/*---------------------KCIStmtRelease Modes----------------------------------*/
#define KCI_STRLS_CACHE_DELETE      0x0010              /* Delete from Cache */

	/*---------------------KCIHanlde Mgmt Misc Modes-----------------------------*/
#define KCI_STM_RESERVED4   0x00100000                           /* reserved */

	/*-----------------------------End Various Modes ----------------------------*/

	/*------------------------Piece Information----------------------------------*/
#define KCI_PARAM_IN 0x01                                    /* in parameter */
#define KCI_PARAM_OUT 0x02                                  /* out parameter */
	/*---------------------------------------------------------------------------*/

	/*------------------------ Transaction Start Flags --------------------------*/
	/* NOTE: KCI_TRANS_JOIN and KCI_TRANS_NOMIGRATE not supported in 8.0.X       */
#define KCI_TRANS_NEW          0x00000001 /* start a new local or global txn */
#define KCI_TRANS_JOIN         0x00000002     /* join an existing global txn */
#define KCI_TRANS_RESUME       0x00000004    /* resume the global txn branch */
#define KCI_TRANS_PROMOTE      0x00000008 /* promote the local txn to global */
#define KCI_TRANS_STARTMASK    0x000000ff  /* mask for start operation flags */

#define KCI_TRANS_READONLY     0x00000100            /* start a readonly txn */
#define KCI_TRANS_READWRITE    0x00000200          /* start a read-write txn */
#define KCI_TRANS_SERIALIZABLE 0x00000400        /* start a serializable txn */
#define KCI_TRANS_ISOLMASK     0x0000ff00  /* mask for start isolation flags */

#define KCI_TRANS_LOOSE        0x00010000        /* a loosely coupled branch */
#define KCI_TRANS_TIGHT        0x00020000        /* a tightly coupled branch */
#define KCI_TRANS_TYPEMASK     0x000f0000      /* mask for branch type flags */

#define KCI_TRANS_NOMIGRATE    0x00100000      /* non migratable transaction */
#define KCI_TRANS_SEPARABLE    0x00200000  /* separable transaction (8.1.6+) */
#define KCI_TRANS_OTSRESUME    0x00400000      /* OTS resuming a transaction */
#define KCI_TRANS_OTHRMASK     0xfff00000      /* mask for other start flags */


	/*---------------------------------------------------------------------------*/

	/*------------------------ Transaction End Flags ----------------------------*/
#define KCI_TRANS_TWOPHASE      0x01000000           /* use two phase commit */
#define KCI_TRANS_WRITEBATCH    0x00000001  /* force cmt-redo for local txns */
#define KCI_TRANS_WRITEIMMED    0x00000002              /* no force cmt-redo */
#define KCI_TRANS_WRITEWAIT     0x00000004               /* no sync cmt-redo */
#define KCI_TRANS_WRITENOWAIT   0x00000008   /* sync cmt-redo for local txns */
	/*---------------------------------------------------------------------------*/

	/*------------------------- AQ Constants ------------------------------------
	* NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
	* The following constants must match the PL/SQL dbms_aq constants
	* NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
	*/
	/* ------------------------- Visibility flags -------------------------------*/
#define KCI_ENQ_IMMEDIATE       1   /* enqueue is an independent transaction */
#define KCI_ENQ_ON_COMMIT       2  /* enqueue is part of current transaction */

	/* ----------------------- Dequeue mode flags -------------------------------*/
#define KCI_DEQ_BROWSE          1   /* read message without acquiring a lock */
#define KCI_DEQ_LOCKED          2   /* read and obtain write lock on message */
#define KCI_DEQ_REMOVE          3          /* read the message and delete it */
#define KCI_DEQ_REMOVE_NODATA   4    /* delete message w'o returning payload */
#define KCI_DEQ_GETSIG          5                      /* get signature only */

	/* ----------------- Dequeue navigation flags -------------------------------*/
#define KCI_DEQ_FIRST_MSG        1     /* get first message at head of queue */
#define KCI_DEQ_NEXT_MSG         3         /* next message that is available */
#define KCI_DEQ_NEXT_TRANSACTION 2    /* get first message of next txn group */
#define KCI_DEQ_FIRST_MSG_MULTI_GROUP 4 
	/* start from first message and array deq across txn groups */
#define KCI_DEQ_MULT_TRANSACTION 5        /* array dequeue across txn groups */
#define KCI_DEQ_NEXT_MSG_MULTI_GROUP KCI_DEQ_MULT_TRANSACTION 
	/* array dequeue across txn groups */

	/* ----------------- Dequeue Option Reserved flags ------------------------- */
#define KCI_DEQ_RESERVED_1      0x000001

	/* --------------------- Message states -------------------------------------*/
#define KCI_MSG_WAITING         1 /* the message delay has not yet completed */
#define KCI_MSG_READY           0    /* the message is ready to be processed */
#define KCI_MSG_PROCESSED       2          /* the message has been processed */
#define KCI_MSG_EXPIRED         3    /* message has moved to exception queue */

	/* --------------------- Sequence deviation ---------------------------------*/
#define KCI_ENQ_BEFORE          2  /* enqueue message before another message */
#define KCI_ENQ_TOP             3     /* enqueue message before all messages */

	/* ------------------------- Visibility flags -------------------------------*/
#define KCI_DEQ_IMMEDIATE       1   /* dequeue is an independent transaction */
#define KCI_DEQ_ON_COMMIT       2  /* dequeue is part of current transaction */

	/* ------------------------ Wait --------------------------------------------*/
#define KCI_DEQ_WAIT_FOREVER    -1   /* wait forever if no message available */
#define KCI_NTFN_GROUPING_FOREVER -1  /* send grouping notifications forever */
#define KCI_DEQ_NO_WAIT         0  /* do not wait if no message is available */

#define KCI_FLOW_CONTROL_NO_TIMEOUT      -1
	/* streaming enqueue: no timeout for flow control */

	/* ------------------------ Delay -------------------------------------------*/
#define KCI_MSG_NO_DELAY        0        /* message is available immediately */

	/* ------------------------- Expiration -------------------------------------*/
#define KCI_MSG_NO_EXPIRATION -1                /* message will never expire */

#define KCI_MSG_PERSISTENT_OR_BUFFERED   3
#define KCI_MSG_BUFFERED                 2
#define KCI_MSG_PERSISTENT               1

	/* ----------------------- Reserved/AQE pisdef flags ------------------------*/
#define KCI_AQ_RESERVED_1      0x0002
#define KCI_AQ_RESERVED_2      0x0004
#define KCI_AQ_RESERVED_3      0x0008
#define KCI_AQ_RESERVED_4      0x0010

#define KCI_AQ_STREAMING_FLAG  0x02000000

	/* AQ JMS message types */
#define KCI_AQJMS_RAW_MSG    0x00000001                       /* raw message */
#define KCI_AQJMS_TEXT_MSG   0x00000002                      /* text message */
#define KCI_AQJMS_MAP_MSG    0x00000004                       /* map message */
#define KCI_AQJMS_BYTE_MSG   0x00000008                      /* byte message */
#define KCI_AQJMS_STREAM_MSG 0x00000010                    /* stream message */
#define KCI_AQJMS_ADT_MSG    0x00000020                       /* adt message */

	/* AQ JMS Message streaming flags */
#define KCI_AQMSG_FIRST_CHUNK 0x00000001           /* first chunk of message */
#define KCI_AQMSG_NEXT_CHUNK  0x00000002            /* next chunk of message */
#define KCI_AQMSG_LAST_CHUNK  0x00000004            /* last chunk of message */


	/* ------------------------------ Replay Info -------------------------------*/
#define KCI_AQ_LAST_ENQUEUED     0
#define KCI_AQ_LAST_ACKNOWLEDGED 1

	/* -------------------------- END AQ Constants ----------------------------- */

	/* --------------------END DateTime and Interval Constants ------------------*/

	/*-----------------------Object Types----------------------------------------*/
	/*-----------Object Types **** Not to be Used **** --------------------------*/
	/* Deprecated */
#define KCI_OTYPE_UNK           0
#define KCI_OTYPE_TABLE         1
#define KCI_OTYPE_VIEW          2
#define KCI_OTYPE_SYN           3
#define KCI_OTYPE_PROC          4
#define KCI_OTYPE_FUNC          5
#define KCI_OTYPE_PKG           6
#define KCI_OTYPE_STMT          7
	/*---------------------------------------------------------------------------*/

	/*=======================Describe Handle Parameter Attributes ===============*/
	/*
	These attributes were historically orthogonal to the other set of attributes
	defined above.  These attrubutes are to be used only for the describe
	handle.  However, this contributes to confusion during attribute
	creation. All new attributes for handles and descriptors should be added
	above where it says "NEW ATTRIBUTES MUST BE ADDED HERE."
	*/
	/*===========================================================================*/
	/* Attributes common to Columns and Stored Procs */
#define KCI_ATTR_DATA_SIZE      1                /* maximum size of the data */
#define KCI_ATTR_DATA_TYPE      2     /* the SQL type of the column/argument */
#define KCI_ATTR_DISP_SIZE      3                        /* the display size */
#define KCI_ATTR_NAME           4         /* the name of the column/argument */
#define KCI_ATTR_PRECISION      5                /* precision if number type */
#define KCI_ATTR_SCALE          6                    /* scale if number type */
#define KCI_ATTR_IS_NULL        7                            /* is it null ? */
#define KCI_ATTR_TYPE_NAME      8
	/* name of the named data type or a package name for package private types */
#define KCI_ATTR_SCHEMA_NAME    9             /* the schema name */
#define KCI_ATTR_SUB_NAME       10      /* type name if package private type */
#define KCI_ATTR_POSITION       11
	/* relative position of col/arg in the list of cols/args */
#define KCI_ATTR_PACKAGE_NAME   12           /* package name of package type */
	/* complex object retrieval parameter attributes */
#define KCI_ATTR_COMPLEXOBJECTCOMP_TYPE         50 
#define KCI_ATTR_COMPLEXOBJECTCOMP_TYPE_LEVEL   51
#define KCI_ATTR_COMPLEXOBJECT_LEVEL            52
#define KCI_ATTR_COMPLEXOBJECT_COLL_OUTOFLINE   53

	/* Only Columns */
#define KCI_ATTR_DISP_NAME      100                      /* the display name */
#define KCI_ATTR_ENCC_SIZE      101                   /* encrypted data size */
#define KCI_ATTR_COL_ENC        102                 /* column is encrypted ? */
#define KCI_ATTR_COL_ENC_SALT   103          /* is encrypted column salted ? */
#define KCI_ATTR_COL_PROPERTIES 104          /* column properties */
	/*--------------------------------------------------------------
	Flags coresponding to the column properties
	----------------------------------------------------------------*/
#define KCI_ATTR_COL_PROPERTY_IS_IDENTITY           0x0000000000000001 
#define KCI_ATTR_COL_PROPERTY_IS_GEN_ALWAYS         0x0000000000000002 
#define KCI_ATTR_COL_PROPERTY_IS_GEN_BY_DEF_ON_NULL 0x0000000000000004 

	/*Only Stored Procs */
#define KCI_ATTR_OVERLOAD       210           /* is this position overloaded */
#define KCI_ATTR_LEVEL          211            /* level for structured types */
#define KCI_ATTR_HAS_DEFAULT    212                   /* has a default value */
#define KCI_ATTR_IOMODE         213                         /* in, out inout */
#define KCI_ATTR_RADIX          214                       /* returns a radix */
#define KCI_ATTR_NUM_ARGS       215             /* total number of arguments */

	/* only named type attributes */
#define KCI_ATTR_TYPECODE                  216       /* object or collection */
#define KCI_ATTR_COLLECTION_TYPECODE       217     /* varray or nested table */
#define KCI_ATTR_VERSION                   218      /* user assigned version */
#define KCI_ATTR_IS_INCOMPLETE_TYPE        219 /* is this an incomplete type */
#define KCI_ATTR_IS_SYSTEM_TYPE            220              /* a system type */
#define KCI_ATTR_IS_PREDEFINED_TYPE        221          /* a predefined type */
#define KCI_ATTR_IS_TRANSIENT_TYPE         222           /* a transient type */
#define KCI_ATTR_IS_SYSTEM_GENERATED_TYPE  223      /* system generated type */
#define KCI_ATTR_HAS_NESTED_TABLE          224 /* contains nested table attr */
#define KCI_ATTR_HAS_LOB                   225        /* has a lob attribute */
#define KCI_ATTR_HAS_FILE                  226       /* has a file attribute */
#define KCI_ATTR_COLLECTION_ELEMENT        227 /* has a collection attribute */
#define KCI_ATTR_NUM_TYPE_ATTRS            228  /* number of attribute types */
#define KCI_ATTR_LIST_TYPE_ATTRS           229    /* list of type attributes */
#define KCI_ATTR_NUM_TYPE_METHODS          230     /* number of type methods */
#define KCI_ATTR_LIST_TYPE_METHODS         231       /* list of type methods */
#define KCI_ATTR_MAP_METHOD                232         /* map method of type */
#define KCI_ATTR_ORDER_METHOD              233       /* order method of type */

	/* only collection element */
#define KCI_ATTR_NUM_ELEMS                 234         /* number of elements */

	/* only type methods */
#define KCI_ATTR_ENCAPSULATION             235        /* encapsulation level */
#define KCI_ATTR_IS_SELFISH                236             /* method selfish */
#define KCI_ATTR_IS_VIRTUAL                237                    /* virtual */
#define KCI_ATTR_IS_INLINE                 238                     /* inline */
#define KCI_ATTR_IS_CONSTANT               239                   /* constant */
#define KCI_ATTR_HAS_RESULT                240                 /* has result */
#define KCI_ATTR_IS_CONSTRUCTOR            241                /* constructor */
#define KCI_ATTR_IS_DESTRUCTOR             242                 /* destructor */
#define KCI_ATTR_IS_OPERATOR               243                   /* operator */
#define KCI_ATTR_IS_MAP                    244               /* a map method */
#define KCI_ATTR_IS_ORDER                  245               /* order method */
#define KCI_ATTR_IS_RNDS                   246  /* read no data state method */
#define KCI_ATTR_IS_RNPS                   247      /* read no process state */
#define KCI_ATTR_IS_WNDS                   248 /* write no data state method */
#define KCI_ATTR_IS_WNPS                   249     /* write no process state */

#define KCI_ATTR_DESC_PUBLIC               250              /* public object */

	/* Object Cache Enhancements : attributes for User Constructed Instances     */
#define KCI_ATTR_CACHE_CLIENT_CONTEXT      251
#define KCI_ATTR_UCI_CONSTRUCT             252
#define KCI_ATTR_UCI_DESTRUCT              253
#define KCI_ATTR_UCI_COPY                  254
#define KCI_ATTR_UCI_PICKLE                255
#define KCI_ATTR_UCI_UNPICKLE              256
#define KCI_ATTR_UCI_REFRESH               257

	/* for type inheritance */
#define KCI_ATTR_IS_SUBTYPE                258
#define KCI_ATTR_SUPERTYPE_SCHEMA_NAME     259
#define KCI_ATTR_SUPERTYPE_NAME            260

	/* for schemas */
#define KCI_ATTR_LIST_OBJECTS              261  /* list of objects in schema */

	/* for database */
#define KCI_ATTR_NCHARSET_ID               262                /* char set id */
#define KCI_ATTR_LIST_SCHEMAS              263            /* list of schemas */
#define KCI_ATTR_MAX_PROC_LEN              264       /* max procedure length */
#define KCI_ATTR_MAX_COLUMN_LEN            265     /* max column name length */
#define KCI_ATTR_CURSOR_COMMIT_BEHAVIOR    266     /* cursor commit behavior */
#define KCI_ATTR_MAX_CATALOG_NAMELEN       267         /* catalog namelength */
#define KCI_ATTR_CATALOG_LOCATION          268           /* catalog location */
#define KCI_ATTR_SAVEPOINT_SUPPORT         269          /* savepoint support */
#define KCI_ATTR_NOWAIT_SUPPORT            270             /* nowait support */
#define KCI_ATTR_AUTOCOMMIT_DDL            271             /* autocommit DDL */
#define KCI_ATTR_LOCKING_MODE              272               /* locking mode */

	/* for externally initialized context */
#define KCI_ATTR_APPCTX_SIZE               273 /* count of context to be init*/
#define KCI_ATTR_APPCTX_LIST               274 /* count of context to be init*/
#define KCI_ATTR_APPCTX_NAME               275 /* name  of context to be init*/
#define KCI_ATTR_APPCTX_ATTR               276 /* attr  of context to be init*/
#define KCI_ATTR_APPCTX_VALUE              277 /* value of context to be init*/

	/* for client id propagation */
#define KCI_ATTR_CLIENT_IDENTIFIER         278   /* value of client id to set*/

	/* for inheritance - part 2 */
#define KCI_ATTR_IS_FINAL_TYPE             279            /* is final type ? */
#define KCI_ATTR_IS_INSTANTIABLE_TYPE      280     /* is instantiable type ? */
#define KCI_ATTR_IS_FINAL_METHOD           281          /* is final method ? */
#define KCI_ATTR_IS_INSTANTIABLE_METHOD    282   /* is instantiable method ? */
#define KCI_ATTR_IS_OVERRIDING_METHOD      283     /* is overriding method ? */

#define KCI_ATTR_DESC_SYNBASE              284   /* Describe the base object */


#define KCI_ATTR_CHAR_USED                 285      /* char length semantics */
#define KCI_ATTR_CHAR_SIZE                 286                /* char length */

	/* SQLJ support */
#define KCI_ATTR_IS_JAVA_TYPE              287 /* is java implemented type ? */

	/* N-Tier support */
#define KCI_ATTR_DISTINGUISHED_NAME        300        /* use DN as user name */
#define KCI_ATTR_KERBEROS_TICKET           301   /* Kerberos ticket as cred. */

	/* for multilanguage debugging */
#define KCI_ATTR_ORA_DEBUG_JDWP            302   /* ORA_DEBUG_JDWP attribute */

#define KCI_ATTR_EDITION                   288                /* ORA_EDITION */

#define KCI_ATTR_RESERVED_14               303                   /* reserved */

	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/



	/*---------------------------End Describe Handle Attributes -----------------*/

	/* For values 303 - 307, see DirPathAPI attribute section in this file */

	/* ----------------------- Session Pool Attributes ------------------------- */
#define KCI_ATTR_SPOOL_TIMEOUT              308           /* session timeout */
#define KCI_ATTR_SPOOL_GETMODE              309          /* session get mode */
#define KCI_ATTR_SPOOL_BUSY_COUNT           310        /* busy session count */
#define KCI_ATTR_SPOOL_OPEN_COUNT           311        /* open session count */
#define KCI_ATTR_SPOOL_MIN                  312         /* min session count */
#define KCI_ATTR_SPOOL_MAX                  313         /* max session count */
#define KCI_ATTR_SPOOL_INCR                 314   /* session increment count */
#define KCI_ATTR_SPOOL_STMTCACHESIZE        208   /*Stmt cache size of pool  */
#define KCI_ATTR_SPOOL_AUTH                 460 /* Auth handle on pool handle*/
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/

	/*------------------------------End Session Pool Attributes -----------------*/
	/*---------------------------- For XML Types ------------------------------- */
	/* For table, view and column */
#define KCI_ATTR_IS_XMLTYPE          315         /* Is the type an XML type? */
#define KCI_ATTR_XMLSCHEMA_NAME      316               /* Name of XML Schema */
#define KCI_ATTR_XMLELEMENT_NAME     317              /* Name of XML Element */
#define KCI_ATTR_XMLSQLTYPSCH_NAME   318    /* SQL type's schema for XML Ele */
#define KCI_ATTR_XMLSQLTYPE_NAME     319     /* Name of SQL type for XML Ele */
#define KCI_ATTR_XMLTYPE_STORED_OBJ  320       /* XML type stored as object? */
#define KCI_ATTR_XMLTYPE_BINARY_XML  422       /* XML type stored as binary? */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/*---------------------------- For Subtypes ------------------------------- */
	/* For type */
#define KCI_ATTR_HAS_SUBTYPES        321                    /* Has subtypes? */
#define KCI_ATTR_NUM_SUBTYPES        322               /* Number of subtypes */
#define KCI_ATTR_LIST_SUBTYPES       323                 /* List of subtypes */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/* XML flag */
#define KCI_ATTR_XML_HRCHY_ENABLED   324               /* hierarchy enabled? */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/* Method flag */
#define KCI_ATTR_IS_OVERRIDDEN_METHOD 325           /* Method is overridden? */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/* For values 326 - 335, see DirPathAPI attribute section in this file */

	/*------------- Attributes for 10i Distributed Objects ----------------------*/
#define KCI_ATTR_OBJ_SUBS                   336 /* obj col/tab substitutable */

	/* For values 337 - 338, see DirPathAPI attribute section in this file */

	/*---------- Attributes for 10i XADFIELD (NLS language, territory -----------*/
#define KCI_ATTR_XADFIELD_RESERVED_1        339                  /* reserved */
#define KCI_ATTR_XADFIELD_RESERVED_2        340                  /* reserved */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/

	/*------------- Kerberos Secure Client Identifier ---------------------------*/
#define KCI_ATTR_KERBEROS_CID               341 /* Kerberos db service ticket*/
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/



	/*------------------------ Attributes for Rules objects ---------------------*/
#define KCI_ATTR_CONDITION                  342            /* rule condition */
#define KCI_ATTR_COMMENT                    343                   /* comment */
#define KCI_ATTR_VALUE                      344             /* Anydata value */
#define KCI_ATTR_EVAL_CONTEXT_OWNER         345        /* eval context owner */
#define KCI_ATTR_EVAL_CONTEXT_NAME          346         /* eval context name */
#define KCI_ATTR_EVALUATION_FUNCTION        347        /* eval function name */
#define KCI_ATTR_VAR_TYPE                   348             /* variable type */
#define KCI_ATTR_VAR_VALUE_FUNCTION         349   /* variable value function */
#define KCI_ATTR_VAR_METHOD_FUNCTION        350  /* variable method function */
#define KCI_ATTR_ACTION_CONTEXT             351            /* action context */
#define KCI_ATTR_LIST_TABLE_ALIASES         352     /* list of table aliases */
#define KCI_ATTR_LIST_VARIABLE_TYPES        353    /* list of variable types */
#define KCI_ATTR_TABLE_NAME                 356                /* table name */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/* For values 357 - 359, see DirPathAPI attribute section in this file */

#define KCI_ATTR_MESSAGE_CSCN               360              /* message cscn */
#define KCI_ATTR_MESSAGE_DSCN               361              /* message dscn */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/*--------------------- Audit Session ID ------------------------------------*/
#define KCI_ATTR_AUDIT_SESSION_ID           362          /* Audit session ID */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/*--------------------- Kerberos TGT Keys -----------------------------------*/
#define KCI_ATTR_KERBEROS_KEY               363  /* n-tier Kerberos cred key */
#define KCI_ATTR_KERBEROS_CID_KEY           364    /* SCID Kerberos cred key */


#define KCI_ATTR_TRANSACTION_NO             365         /* AQ enq txn number */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/*----------------------- Attributes for End To End Tracing -----------------*/
#define KCI_ATTR_MODULE                     366        /* module for tracing */
#define KCI_ATTR_ACTION                     367        /* action for tracing */
#define KCI_ATTR_CLIENT_INFO                368               /* client info */
#define KCI_ATTR_COLLECT_CALL_TIME          369         /* collect call time */
#define KCI_ATTR_CALL_TIME                  370         /* extract call time */
#define KCI_ATTR_ECONTEXT_ID                371      /* execution-id context */
#define KCI_ATTR_ECONTEXT_SEQ               372  /*execution-id sequence num */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/



	/*------------------------------ Session attributes -------------------------*/
#define KCI_ATTR_SESSION_STATE              373             /* session state */
#define KCI_SESSION_STATELESS  1                             /* valid states */
#define KCI_SESSION_STATEFUL   2
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


#define KCI_ATTR_SESSION_STATETYPE          374        /* session state type */
#define KCI_SESSION_STATELESS_DEF  0                    /* valid state types */
#define KCI_SESSION_STATELESS_CAL  1
#define KCI_SESSION_STATELESS_TXN  2
#define KCI_SESSION_STATELESS_APP  3
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


#define KCI_ATTR_SESSION_STATE_CLEARED      376     /* session state cleared */
#define KCI_ATTR_SESSION_MIGRATED           377       /* did session migrate */
#define KCI_ATTR_SESSION_PRESERVE_STATE     388    /* preserve session state */
#define KCI_ATTR_DRIVER_NAME                424               /* Driver Name */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/* -------------------------- Admin Handle Attributes ---------------------- */

#define KCI_ATTR_ADMIN_PFILE                389    /* client-side param file */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/*----------------------- Attributes for End To End Tracing -----------------*/
	/* -------------------------- HA Event Handle Attributes ------------------- */

#define KCI_ATTR_HOSTNAME         390                /* SYS_CONTEXT hostname */
#define KCI_ATTR_DBNAME           391                  /* SYS_CONTEXT dbname */
#define KCI_ATTR_INSTNAME         392           /* SYS_CONTEXT instance name */
#define KCI_ATTR_SERVICENAME      393            /* SYS_CONTEXT service name */
#define KCI_ATTR_INSTSTARTTIME    394      /* v$instance instance start time */
#define KCI_ATTR_HA_TIMESTAMP     395                          /* event time */
#define KCI_ATTR_RESERVED_22      396                            /* reserved */
#define KCI_ATTR_RESERVED_23      397                            /* reserved */
#define KCI_ATTR_RESERVED_24      398                            /* reserved */
#define KCI_ATTR_DBDOMAIN         399                           /* db domain */
#define KCI_ATTR_RESERVED_27      425                            /* reserved */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


#define KCI_ATTR_EVENTTYPE        400                          /* event type */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/

#define KCI_EVENTTYPE_HA            0  /* valid value for KCI_ATTR_EVENTTYPE */

#define KCI_ATTR_HA_SOURCE        401
	/* valid values for KCI_ATTR_HA_SOURCE */
#define KCI_HA_SOURCE_INSTANCE            0 
#define KCI_HA_SOURCE_DATABASE            1
#define KCI_HA_SOURCE_NODE                2
#define KCI_HA_SOURCE_SERVICE             3
#define KCI_HA_SOURCE_SERVICE_MEMBER      4
#define KCI_HA_SOURCE_ASM_INSTANCE        5
#define KCI_HA_SOURCE_SERVICE_PRECONNECT  6
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


#define KCI_ATTR_HA_STATUS        402
#define KCI_HA_STATUS_DOWN          0 /* valid values for KCI_ATTR_HA_STATUS */
#define KCI_HA_STATUS_UP            1

#define KCI_ATTR_HA_SRVFIRST      403

#define KCI_ATTR_HA_SRVNEXT       404
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/

	/* ------------------------- Server Handle Attributes -----------------------*/

#define KCI_ATTR_TAF_ENABLED      405

	/* Extra notification attributes */
#define KCI_ATTR_NFY_FLAGS        406 

#define KCI_ATTR_MSG_DELIVERY_MODE 407        /* msg delivery mode */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/

#define KCI_ATTR_DB_CHARSET_ID     416       /* database charset ID */
#define KCI_ATTR_DB_NCHARSET_ID    417       /* database ncharset ID */
#define KCI_ATTR_RESERVED_25       418                           /* reserved */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


#define KCI_ATTR_FLOW_CONTROL_TIMEOUT       423  /* AQ: flow control timeout */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/

#define KCI_ATTR_ENV_NLS_LANGUAGE           424 
#define KCI_ATTR_ENV_NLS_TERRITORY          425 

	/*---------------------------------------------------------------------------*/
	/* ------------------DirPathAPI attribute Section----------------------------*/
	/* All DirPathAPI attributes are in this section of the file.  Existing      */
	/* attributes prior to this section being created are assigned values < 2000 */
	/* Add new DirPathAPI attributes to this section and their assigned value    */
	/* should be whatever the last entry is + 1.                                 */

	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/*------------- Supported Values for Direct Path Stream Version -------------*/
#define KCI_DIRPATH_STREAM_VERSION_1        100
#define KCI_DIRPATH_STREAM_VERSION_2        200
#define KCI_DIRPATH_STREAM_VERSION_3        300                   /* default */


#define KCI_ATTR_DIRPATH_MODE           78  /* mode of direct path operation */
#define KCI_ATTR_DIRPATH_NOLOG          79               /* nologging option */
#define KCI_ATTR_DIRPATH_PARALLEL       80     /* parallel (temp seg) option */

#define KCI_ATTR_DIRPATH_SORTED_INDEX    137 /* index that data is sorted on */

	/* direct path index maint method (see oci8dp.h) */
#define KCI_ATTR_DIRPATH_INDEX_MAINT_METHOD 138

	/* parallel load: db file, initial and next extent sizes */

#define KCI_ATTR_DIRPATH_FILE               139      /* DB file to load into */
#define KCI_ATTR_DIRPATH_STORAGE_INITIAL    140       /* initial extent size */
#define KCI_ATTR_DIRPATH_STORAGE_NEXT       141          /* next extent size */
	/* direct path index maint method (see oci8dp.h) */
#define KCI_ATTR_DIRPATH_SKIPINDEX_METHOD   145

	/* 8.2 dpapi support of ADTs */
#define KCI_ATTR_DIRPATH_EXPR_TYPE  150        /* expr type of KCI_ATTR_NAME */

	/* For the direct path API there are three data formats:
	* TEXT   - used mainly by SQL*Loader, data is in textual form
	* STREAM - used by datapump, data is in stream loadable form
	* KCI    - used by KCI programs utilizing the DpApi, data is in binary form
	*/
#define KCI_ATTR_DIRPATH_INPUT         151 
#define KCI_DIRPATH_INPUT_TEXT        0x01                           /* text */
#define KCI_DIRPATH_INPUT_STREAM      0x02              /* stream (datapump) */
#define KCI_DIRPATH_INPUT_KCI         0x04                   /* binary (oci) */
#define KCI_DIRPATH_INPUT_UNKNOWN     0x08

#define KCI_ATTR_DIRPATH_FN_CTX             167  /* fn ctx ADT attrs or args */

#define KCI_ATTR_DIRPATH_OID                187   /* loading into an OID col */
#define KCI_ATTR_DIRPATH_SID                194   /* loading into an SID col */
#define KCI_ATTR_DIRPATH_OBJ_CONSTR         206 /* obj type of subst obj tbl */

	/* Attr to allow setting of the stream version PRIOR to calling Prepare */
#define KCI_ATTR_DIRPATH_STREAM_VERSION     212      /* version of the stream*/

#define KCIP_ATTR_DIRPATH_VARRAY_INDEX      213       /* varray index column */

	/*------------- Supported Values for Direct Path Date cache -----------------*/
#define KCI_ATTR_DIRPATH_DCACHE_NUM         303        /* date cache entries */
#define KCI_ATTR_DIRPATH_DCACHE_SIZE        304          /* date cache limit */
#define KCI_ATTR_DIRPATH_DCACHE_MISSES      305         /* date cache misses */
#define KCI_ATTR_DIRPATH_DCACHE_HITS        306           /* date cache hits */
#define KCI_ATTR_DIRPATH_DCACHE_DISABLE     307 /* on set: disable datecache 
	* on overflow.
	* on get: datecache disabled?
	* could be due to overflow
	* or others                  */

	/*------------- Attributes for 10i Updates to the DirPath API ---------------*/
#define KCI_ATTR_DIRPATH_RESERVED_7         326                 /* reserved */
#define KCI_ATTR_DIRPATH_RESERVED_8         327                 /* reserved */
#define KCI_ATTR_DIRPATH_CONVERT            328 /* stream conversion needed? */
#define KCI_ATTR_DIRPATH_BADROW             329        /* info about bad row */
#define KCI_ATTR_DIRPATH_BADROW_LENGTH      330    /* length of bad row info */
#define KCI_ATTR_DIRPATH_WRITE_ORDER        331         /* column fill order */
#define KCI_ATTR_DIRPATH_GRANULE_SIZE       332   /* granule size for unload */
#define KCI_ATTR_DIRPATH_GRANULE_OFFSET     333    /* offset to last granule */
#define KCI_ATTR_DIRPATH_RESERVED_1         334                  /* reserved */
#define KCI_ATTR_DIRPATH_RESERVED_2         335                  /* reserved */

	/*------ Attributes for 10i DirPathAPI conversion (NLS lang, terr, cs) ------*/
#define KCI_ATTR_DIRPATH_RESERVED_3         337                  /* reserved */
#define KCI_ATTR_DIRPATH_RESERVED_4         338                  /* reserved */
#define KCI_ATTR_DIRPATH_RESERVED_5         357                  /* reserved */
#define KCI_ATTR_DIRPATH_RESERVED_6         358                  /* reserved */

#define KCI_ATTR_DIRPATH_LOCK_WAIT          359    /* wait for lock in dpapi */
	/* ATTRIBUTE NUMBERS ARE GLOBAL
	*
	* DO NOT ADD NEW ATTRIBUTES HERE -- ADD WHERE IT SAYS
	* "NEW ATTRIBUTES MUST BE ADDED  HERE"
	*/


	/* ------------------End of DirPathAPI attribute Section --------------------*/
	/*---------------------------------------------------------------------------*/


	/*---------------- Describe Handle Parameter Attribute Values ---------------*/

	/* KCI_ATTR_CURSOR_COMMIT_BEHAVIOR */
#define KCI_CURSOR_OPEN   0 
#define KCI_CURSOR_CLOSED 1

	/* KCI_ATTR_CATALOG_LOCATION */
#define KCI_CL_START 0
#define KCI_CL_END   1

	/* KCI_ATTR_SAVEPOINT_SUPPORT */
#define KCI_SP_SUPPORTED   0
#define KCI_SP_UNSUPPORTED 1

	/* KCI_ATTR_NOWAIT_SUPPORT */
#define KCI_NW_SUPPORTED   0
#define KCI_NW_UNSUPPORTED 1

	/* KCI_ATTR_AUTOCOMMIT_DDL */
#define KCI_AC_DDL    0
#define KCI_NO_AC_DDL 1

	/* KCI_ATTR_LOCKING_MODE */
#define KCI_LOCK_IMMEDIATE 0
#define KCI_LOCK_DELAYED   1

	/* ------------------- Instance type attribute values -----------------------*/
#define KCI_INSTANCE_TYPE_UNKNOWN  0
#define KCI_INSTANCE_TYPE_RDBMS    1
#define KCI_INSTANCE_TYPE_OSM      2
#define KCI_INSTANCE_TYPE_PROXY    3
#define KCI_INSTANCE_TYPE_IOS      4


	/* ---------------- ASM Volume Device Support attribute values --------------*/
#define KCI_ASM_VOLUME_UNSUPPORTED 0
#define KCI_ASM_VOLUME_SUPPORTED   1

	/*---------------------------------------------------------------------------*/

	/*---------------------------KCIPasswordChange-------------------------------*/
#define KCI_AUTH         0x08        /* Change the password but do not login */


	/*------------------------Other Constants------------------------------------*/
#define KCI_MAX_FNS   100                     /* max number of KCI Functions */
#define KCI_SQLSTATE_SIZE 5  
#define KCI_ERROR_MAXMSG_SIZE   1024         /* max size of an error message */
#define KCI_ERROR_MAXMSG_SIZE2  3072 /* new len max size of an error message */
#define KCI_LOBMAXSIZE MINUB4MAXVAL                 /* maximum lob data size */
#define KCI_ROWID_LEN             23
#define KCI_LOB_CONTENTTYPE_MAXSIZE 128  /* max size of securefile contenttype */
#define KCI_LOB_CONTENTTYPE_MAXBYTESIZE  KCI_LOB_CONTENTTYPE_MAXSIZE 
	/*---------------------------------------------------------------------------*/

	/*------------------------ Fail Over Events ---------------------------------*/
#define KCI_FO_END          0x00000001
#define KCI_FO_ABORT        0x00000002   
#define KCI_FO_REAUTH       0x00000004
#define KCI_FO_BEGIN        0x00000008 
#define KCI_FO_ERROR        0x00000010
	/*---------------------------------------------------------------------------*/

	/*------------------------ Fail Over Callback Return Codes ------------------*/
#define KCI_FO_RETRY        25410
	/*---------------------------------------------------------------------------*/

	/*------------------------- Fail Over Types ---------------------------------*/
#define KCI_FO_NONE           0x00000001
#define KCI_FO_SESSION        0x00000002
#define KCI_FO_SELECT         0x00000004
#define KCI_FO_TXNAL          0x00000008
	/*---------------------------------------------------------------------------*/

	/*--------------------- KCI_ATTR_VARTYPE_MAXLEN_COMPAT values ---------------*/
#define KCI_ATTR_MAXLEN_COMPAT_STANDARD 1
#define KCI_ATTR_MAXLEN_COMPAT_EXTENDED 2
	/*---------------------------------------------------------------------------*/


	/*-----------------------Function Codes--------------------------------------*/
#define KCI_FNCODE_INITIALIZE     1                         /* KCIInitialize */
#define KCI_FNCODE_HANDLEALLOC  2                          /* KCIHandleAlloc */
#define KCI_FNCODE_HANDLEFREE  3                            /* KCIHandleFree */
#define KCI_FNCODE_DESCRIPTORALLOC  4                  /* KCIDescriptorAlloc */
#define KCI_FNCODE_DESCRIPTORFREE  5                    /* KCIDescriptorFree */
#define KCI_FNCODE_ENVINIT   6                                 /* KCIEnvInit */
#define KCI_FNCODE_SERVERATTACH   7                       /* KCIServerAttach */
#define KCI_FNCODE_SERVERDETACH   8                       /* KCIServerDetach */
	/* unused         9 */
#define KCI_FNCODE_SESSIONBEGIN  10                       /* KCISessionBegin */
#define KCI_FNCODE_SESSIONEND   11                          /* KCISessionEnd */
#define KCI_FNCODE_PASSWORDCHANGE   12                  /* KCIPasswordChange */
#define KCI_FNCODE_STMTPREPARE   13                        /* KCIStmtPrepare */
	/* unused       14- 16 */
#define KCI_FNCODE_BINDDYNAMIC   17                        /* KCIBindDynamic */
#define KCI_FNCODE_BINDOBJECT  18                           /* KCIBindObject */
	/* 19 unused */
#define KCI_FNCODE_BINDARRAYOFSTRUCT   20            /* KCIBindArrayOfStruct */
#define KCI_FNCODE_STMTEXECUTE  21                         /* KCIStmtExecute */
	/* unused 22-24 */
#define KCI_FNCODE_DEFINEOBJECT  25                       /* KCIDefineObject */
#define KCI_FNCODE_DEFINEDYNAMIC   26                    /* KCIDefineDynamic */
#define KCI_FNCODE_DEFINEARRAYOFSTRUCT  27         /* KCIDefineArrayOfStruct */
#define KCI_FNCODE_STMTFETCH   28                            /* KCIStmtFetch */
#define KCI_FNCODE_STMTGETBIND   29                    /* KCIStmtGetBindInfo */
	/* 30, 31 unused */
#define KCI_FNCODE_DESCRIBEANY  32                         /* KCIDescribeAny */
#define KCI_FNCODE_TRANSSTART  33                           /* KCITransStart */
#define KCI_FNCODE_TRANSDETACH  34                         /* KCITransDetach */
#define KCI_FNCODE_TRANSCOMMIT  35                         /* KCITransCommit */
	/* 36 unused */
#define KCI_FNCODE_ERRORGET   37                              /* KCIErrorGet */
#define KCI_FNCODE_LOBOPENFILE  38                         /* KCILobFileOpen */
#define KCI_FNCODE_LOBCLOSEFILE  39                       /* KCILobFileClose */
	/* 40 was LOBCREATEFILE, unused */
	/* 41 was KCILobFileDelete, unused  */
#define KCI_FNCODE_LOBCOPY  42                                 /* KCILobCopy */
#define KCI_FNCODE_LOBAPPEND  43                             /* KCILobAppend */
#define KCI_FNCODE_LOBERASE  44                               /* KCILobErase */
#define KCI_FNCODE_LOBLENGTH  45                          /* KCILobGetLength */
#define KCI_FNCODE_LOBTRIM  46                                 /* KCILobTrim */
#define KCI_FNCODE_LOBREAD  47                                 /* KCILobRead */
#define KCI_FNCODE_LOBWRITE  48                               /* KCILobWrite */
	/* 49 unused */
#define KCI_FNCODE_SVCCTXBREAK 50                                /* KCIBreak */
#define KCI_FNCODE_SERVERVERSION  51                     /* KCIServerVersion */

#define KCI_FNCODE_KERBATTRSET 52                          /* KCIKerbAttrSet */

#define KCI_FNCODE_SERVERRELEASE 53                      /* KCIServerRelease */

#define KCI_FNCODE_ATTRGET 54                                  /* KCIAttrGet */
#define KCI_FNCODE_ATTRSET 55                                  /* KCIAttrSet */
#define KCI_FNCODE_PARAMSET 56                                /* KCIParamSet */
#define KCI_FNCODE_PARAMGET 57                                /* KCIParamGet */
#define KCI_FNCODE_STMTGETPIECEINFO   58              /* KCIStmtGetPieceInfo */
#define KCI_FNCODE_LDATOSVCCTX 59                          /* KCILdaToSvcCtx */
	/* 60 unused */
#define KCI_FNCODE_STMTSETPIECEINFO   61              /* KCIStmtSetPieceInfo */
#define KCI_FNCODE_TRANSFORGET 62                          /* KCITransForget */
#define KCI_FNCODE_TRANSPREPARE 63                        /* KCITransPrepare */
#define KCI_FNCODE_TRANSROLLBACK  64                     /* KCITransRollback */
#define KCI_FNCODE_DEFINEBYPOS 65                          /* KCIDefineByPos */
#define KCI_FNCODE_BINDBYPOS 66                              /* KCIBindByPos */
#define KCI_FNCODE_BINDBYNAME 67                            /* KCIBindByName */
#define KCI_FNCODE_LOBASSIGN  68                             /* KCILobAssign */
#define KCI_FNCODE_LOBISEQUAL  69                           /* KCILobIsEqual */
#define KCI_FNCODE_LOBISINIT  70                      /* KCILobLocatorIsInit */

#define KCI_FNCODE_LOBENABLEBUFFERING  71           /* KCILobEnableBuffering */
#define KCI_FNCODE_LOBCHARSETID  72                       /* KCILobCharSetID */
#define KCI_FNCODE_LOBCHARSETFORM  73                   /* KCILobCharSetForm */
#define KCI_FNCODE_LOBFILESETNAME  74                   /* KCILobFileSetName */
#define KCI_FNCODE_LOBFILEGETNAME  75                   /* KCILobFileGetName */
#define KCI_FNCODE_LOGON 76                                      /* KCILogon */
#define KCI_FNCODE_LOGOFF 77                                    /* KCILogoff */
#define KCI_FNCODE_LOBDISABLEBUFFERING 78          /* KCILobDisableBuffering */
#define KCI_FNCODE_LOBFLUSHBUFFER 79                    /* KCILobFlushBuffer */
#define KCI_FNCODE_LOBLOADFROMFILE 80                  /* KCILobLoadFromFile */

#define KCI_FNCODE_LOBOPEN  81                                 /* KCILobOpen */
#define KCI_FNCODE_LOBCLOSE  82                               /* KCILobClose */
#define KCI_FNCODE_LOBISOPEN  83                             /* KCILobIsOpen */
#define KCI_FNCODE_LOBFILEISOPEN  84                     /* KCILobFileIsOpen */
#define KCI_FNCODE_LOBFILEEXISTS  85                     /* KCILobFileExists */
#define KCI_FNCODE_LOBFILECLOSEALL  86                 /* KCILobFileCloseAll */
#define KCI_FNCODE_LOBCREATETEMP  87                /* KCILobCreateTemporary */
#define KCI_FNCODE_LOBFREETEMP  88                    /* KCILobFreeTemporary */
#define KCI_FNCODE_LOBISTEMP  89                        /* KCILobIsTemporary */

#define KCI_FNCODE_AQENQ  90                                     /* KCIAQEnq */
#define KCI_FNCODE_AQDEQ  91                                     /* KCIAQDeq */
#define KCI_FNCODE_RESET  92                                     /* KCIReset */
#define KCI_FNCODE_SVCCTXTOLDA  93                         /* KCISvcCtxToLda */
#define KCI_FNCODE_LOBLOCATORASSIGN 94                /* KCILobLocatorAssign */

#define KCI_FNCODE_UBINDBYNAME 95

#define KCI_FNCODE_AQLISTEN  96                               /* KCIAQListen */

#define KCI_FNCODE_SVC2HST 97                                    /* reserved */
#define KCI_FNCODE_SVCRH   98                                    /* reserved */
	/* 97 and 98 are reserved for Oracle internal use */

#define KCI_FNCODE_TRANSMULTIPREPARE   99            /* KCITransMultiPrepare */

#define KCI_FNCODE_CPOOLCREATE  100               /* KCIConnectionPoolCreate */
#define KCI_FNCODE_CPOOLDESTROY 101              /* KCIConnectionPoolDestroy */
#define KCI_FNCODE_LOGON2 102                                   /* KCILogon2 */
#define KCI_FNCODE_ROWIDTOCHAR  103                        /* KCIRowidToChar */

#define KCI_FNCODE_SPOOLCREATE  104                  /* KCISessionPoolCreate */
#define KCI_FNCODE_SPOOLDESTROY 105                 /* KCISessionPoolDestroy */
#define KCI_FNCODE_SESSIONGET   106                         /* KCISessionGet */
#define KCI_FNCODE_SESSIONRELEASE 107                   /* KCISessionRelease */
#define KCI_FNCODE_STMTPREPARE2 108                       /* KCIStmtPrepare2 */
#define KCI_FNCODE_STMTRELEASE 109                         /* KCIStmtRelease */
#define KCI_FNCODE_AQENQARRAY  110                          /* KCIAQEnqArray */
#define KCI_FNCODE_AQDEQARRAY  111                          /* KCIAQDeqArray */
#define KCI_FNCODE_LOBCOPY2    112                            /* KCILobCopy2 */
#define KCI_FNCODE_LOBERASE2   113                           /* KCILobErase2 */
#define KCI_FNCODE_LOBLENGTH2  114                       /* KCILobGetLength2 */
#define KCI_FNCODE_LOBLOADFROMFILE2  115              /* KCILobLoadFromFile2 */
#define KCI_FNCODE_LOBREAD2    116                            /* KCILobRead2 */
#define KCI_FNCODE_LOBTRIM2    117                            /* KCILobTrim2 */
#define KCI_FNCODE_LOBWRITE2   118                           /* KCILobWrite2 */
#define KCI_FNCODE_LOBGETSTORAGELIMIT 119           /* KCILobGetStorageLimit */
#define KCI_FNCODE_DBSTARTUP 120                             /* KCIDBStartup */
#define KCI_FNCODE_DBSHUTDOWN 121                           /* KCIDBShutdown */
#define KCI_FNCODE_LOBARRAYREAD       122                 /* KCILobArrayRead */
#define KCI_FNCODE_LOBARRAYWRITE      123                /* KCILobArrayWrite */
#define KCI_FNCODE_AQENQSTREAM        124               /* KCIAQEnqStreaming */
#define KCI_FNCODE_AQGETREPLAY        125              /* KCIAQGetReplayInfo */
#define KCI_FNCODE_AQRESETREPLAY      126            /* KCIAQResetReplayInfo */
#define KCI_FNCODE_ARRAYDESCRIPTORALLOC 127        /*KCIArrayDescriptorAlloc */
#define KCI_FNCODE_ARRAYDESCRIPTORFREE  128       /* KCIArrayDescriptorFree  */
#define KCI_FNCODE_LOBGETOPT        129                /* KCILobGetCptions */
#define KCI_FNCODE_LOBSETOPT        130                /* KCILobSetCptions */
#define KCI_FNCODE_LOBFRAGINS       131           /* KCILobFragementInsert */
#define KCI_FNCODE_LOBFRAGDEL       132           /* KCILobFragementDelete */
#define KCI_FNCODE_LOBFRAGMOV       133             /* KCILobFragementMove */
#define KCI_FNCODE_LOBFRAGREP       134          /* KCILobFragementReplace */
#define KCI_FNCODE_LOBGETDEDUPLICATEREGIONS 135/* KCILobGetDeduplicateRegions */
#define KCI_FNCODE_APPCTXSET        136        /* KCIAppCtxSet */
#define KCI_FNCODE_APPCTXCLEARALL   137         /* KCIAppCtxClearAll */

#define KCI_FNCODE_LOBGETCONTENTTYPE 138             /* KCILobGetContentType */
#define KCI_FNCODE_LOBSETCONTENTTYPE 139             /* KCILobSetContentType */
#define KCI_FNCODE_DEFINEBYPOS2      140                  /* KCIDefineByPos2 */
#define KCI_FNCODE_BINDBYPOS2        141                    /* KCIBindByPos2 */
#define KCI_FNCODE_BINDBYNAME2       142                   /* KCIBindByName2 */
#define KCI_FNCODE_STMTGETNEXTRESULT 143             /* KCIStmtGetNextResult */
#define KCI_FNCODE_AQENQ2            144                        /* KCIAQEnq2 */
#define KCI_FNCODE_AQDEQ2            145                        /* KCIAQDeq2 */
#define KCI_FNCODE_TYPEBYNAME          146                  /* KCITypeByName */
#define KCI_FNCODE_TYPEBYFULLNAME      147              /* KCITypeByFullName */
#define KCI_FNCODE_TYPEBYREF           148                   /* KCITypeByRef */
#define KCI_FNCODE_TYPEARRAYBYNAME     149             /* KCITypeArrayByName */
#define KCI_FNCODE_TYPEARRAYBYFULLNAME 150         /* KCITypeArrayByFullName */
#define KCI_FNCODE_TYPEARRAYBYREF      151              /* KCITypeArrayByRef */
#define KCI_FNCODE_MAXFCN              151      /* maximum KCI function code */

	/*---------------SQLSTMT Cache callback modes-----------------------------*/
#define KCI_CBK_STMTCACHE_STMTPURGE  0x01

	/*---------------------------------------------------------------------------*/

	/*-----------------------Handle Definitions----------------------------------*/
	typedef struct KCIEnv           KCIEnv;            /* KCI environment handle */
	typedef struct KCIError         KCIError;                /* KCI error handle */
	typedef struct KCISvcCtx        KCISvcCtx;             /* KCI service handle */
	typedef struct KCIStmt          KCIStmt;             /* KCI statement handle */
	typedef struct KCIBind          KCIBind;                  /* KCI bind handle */
	typedef struct KCIDefine        KCIDefine;              /* KCI Define handle */
	typedef struct KCIDescribe      KCIDescribe;          /* KCI Describe handle */
	typedef struct KCIServer        KCIServer;              /* KCI Server handle */
	typedef struct KCISession       KCISession;     /* KCI Authentication handle */
	typedef struct KCIComplexObject KCIComplexObject;          /* KCI COR handle */
	typedef struct KCITrans         KCITrans;          /* KCI Transaction handle */
	typedef struct KCISecurity      KCISecurity;          /* KCI Security handle */
	typedef struct KCISubscription  KCISubscription;      /* subscription handle */

	typedef struct KCICPool         KCICPool;          /* connection pool handle */
	typedef struct KCISPool         KCISPool;             /* session pool handle */
	typedef struct KCIAuthInfo      KCIAuthInfo;                  /* auth handle */
	typedef struct KCIAdmin         KCIAdmin;                    /* admin handle */
	typedef struct KCIEvent         KCIEvent;                 /* HA event handle */

															  /*-----------------------Descriptor Definitions------------------------------*/
	typedef struct KCISnapshot      KCISnapshot;      /* KCI snapshot descriptor */
	typedef struct KCIResult        KCIResult;      /* KCI Result Set Descriptor */
	typedef struct KCILobLocator    KCILobLocator; /* KCI Lob Locator descriptor */
	typedef struct KCILobRegion     KCILobRegion;  /* KCI Lob Regions descriptor */
	typedef struct KCIParam         KCIParam;        /* KCI PARameter descriptor */
	typedef struct KCIComplexObjectComp KCIComplexObjectComp;
	/* KCI COR descriptor */
	typedef struct KCIRowid KCIRowid;                    /* KCI ROWID descriptor */

	typedef struct KCIDateTime KCIDateTime;           /* KCI DateTime descriptor */
	typedef struct KCIInterval KCIInterval;           /* KCI Interval descriptor */

	typedef struct KCIUcb           KCIUcb;      /* KCI User Callback descriptor */
	typedef struct KCIServerDNs     KCIServerDNs;    /* KCI server DN descriptor */

													 /*-------------------------- AQ Descriptors ---------------------------------*/
	typedef struct KCIAQEnqOptions    KCIAQEnqOptions; /* AQ Enqueue Options hdl */
	typedef struct KCIAQDeqOptions    KCIAQDeqOptions; /* AQ Dequeue Options hdl */
	typedef struct KCIAQMsgProperties KCIAQMsgProperties;  /* AQ Mesg Properties */
	typedef struct KCIAQAgent         KCIAQAgent;         /* AQ Agent descriptor */
	typedef struct KCIAQNfyDescriptor KCIAQNfyDescriptor;   /* AQ Nfy descriptor */
	typedef struct KCIAQSignature     KCIAQSignature;            /* AQ Siganture */
	typedef struct KCIAQListenOpts    KCIAQListenOpts;      /* AQ listen options */
	typedef struct KCIAQLisMsgProps   KCIAQLisMsgProps;   /* AQ listen msg props */
	typedef struct KCIAQJmsgProperties KCIAQJmsgProperties; /* AQ JMS Properties */

															/*---------------------------------------------------------------------------*/

															/* Lob typedefs for Pro*C */
	typedef struct KCILobLocator KCIClobLocator;    /* KCI Character LOB Locator */
	typedef struct KCILobLocator KCIBlobLocator;       /* KCI Binary LOB Locator */
	typedef struct KCILobLocator KCIBFileLocator; /* KCI Binary LOB File Locator */
												  /*---------------------------------------------------------------------------*/

												  /* Undefined value for tz in interval types*/
#define KCI_INTHR_UNK 24

												  /* These defined adjustment values */
#define KCI_ADJUST_UNK            10
#define KCI_ORACLE_DATE           0
#define KCI_ANSI_DATE             1

												  /*------------------------ Lob-specific Definitions -------------------------*/

												  /*
												  * ociloff - KCI Lob OFFset
												  *
												  * The offset in the lob data.  The offset is specified in terms of bytes for
												  * BLOBs and BFILes.  Character offsets are used for CLOBs, NCLOBs.
												  * The maximum size of internal lob data is 4 gigabytes.  FILE LOB
												  * size is limited by the operating system.
												  */
	typedef ub4 KCILobOffset;

	/*
	* ocillen - KCI Lob LENgth (of lob data)
	*
	* Specifies the length of lob data in bytes for BLOBs and BFILes and in
	* characters for CLOBs, NCLOBs.  The maximum length of internal lob
	* data is 4 gigabytes.  The length of FILE LOBs is limited only by the
	* operating system.
	*/
	typedef ub4 KCILobLength;
	/*
	* ocilmo - KCI Lob open MOdes
	*
	* The mode specifies the planned operations that will be performed on the
	* FILE lob data.  The FILE lob can be opened in read-only mode only.
	*
	* In the future, we may include read/write, append and truncate modes.  Append
	* is equivalent to read/write mode except that the FILE is positioned for
	* writing to the end.  Truncate is equivalent to read/write mode except that
	* the FILE LOB data is first truncated to a length of 0 before use.
	*/
	enum KCILobMode
	{
		KCI_LOBMODE_READONLY = 1,                                     /* read-only */
		KCI_LOBMODE_READWRITE = 2             /* read_write for internal lobs only */
	};
	typedef enum KCILobMode KCILobMode;

	/*---------------------------------------------------------------------------*/


	/*----------------------------Piece Definitions------------------------------*/

	/* if ocidef.h is being included in the app, ocidef.h should precede oci.h */

	/*
	* since clients may  use oci.h, ocidef.h and ocidfn.h the following defines
	* need to be guarded, usually internal clients
	*/

#ifndef KCI_FLAGS
#define KCI_FLAGS
#define KCI_ONE_PIECE 0                                         /* one piece */
#define KCI_FIRST_PIECE 1                                 /* the first piece */
#define KCI_NEXT_PIECE 2                          /* the next of many pieces */
#define KCI_LAST_PIECE 3                                   /* the last piece */
#endif
	/*---------------------------------------------------------------------------*/

	/*--------------------------- FILE open modes -------------------------------*/
#define KCI_FILE_READONLY 1             /* readonly mode open for FILE types */
	/*---------------------------------------------------------------------------*/
	/*--------------------------- LOB open modes --------------------------------*/
#define KCI_LOB_READONLY 1              /* readonly mode open for ILOB types */
#define KCI_LOB_READWRITE 2                /* read write mode open for ILOBs */
#define KCI_LOB_WRITEONLY     3         /* Writeonly mode open for ILOB types*/
#define KCI_LOB_APPENDONLY    4       /* Appendonly mode open for ILOB types */
#define KCI_LOB_FULLOVERWRITE 5                 /* Completely overwrite ILOB */
#define KCI_LOB_FULLREAD      6                 /* Doing a Full Read of ILOB */

	/*----------------------- LOB Buffering Flush Flags -------------------------*/
#define KCI_LOB_BUFFER_FREE   1 
#define KCI_LOB_BUFFER_NOFREE 2
	/*---------------------------------------------------------------------------*/

	/*---------------------------LOB Option Types -------------------------------*/
#define KCI_LOB_OPT_COMPRESS     1                    /* SECUREFILE Compress */
#define KCI_LOB_OPT_ENCRYPT      2                     /* SECUREFILE Encrypt */
#define KCI_LOB_OPT_DEDUPLICATE  4                 /* SECUREFILE Deduplicate */
#define KCI_LOB_OPT_ALLOCSIZE    8             /* SECUREFILE Allocation Size */
#define KCI_LOB_OPT_CONTENTTYPE 16                /* SECUREFILE Content Type */
#define KCI_LOB_OPT_MODTIME     32           /* SECUREFILE Modification Time */

	/*------------------------   LOB Option Values ------------------------------*/
	/* Compression */
#define KCI_LOB_COMPRESS_OFF  0                           /* Compression off */
#define KCI_LOB_COMPRESS_ON   1                            /* Compression on */
	/* Encryption */
#define KCI_LOB_ENCRYPT_OFF   0                            /* Encryption Off */
#define KCI_LOB_ENCRYPT_ON    2                             /* Encryption On */
	/* Deduplciate */
#define KCI_LOB_DEDUPLICATE_OFF 0                         /* Deduplicate Off */
#define KCI_LOB_DEDUPLICATE_ON  4                        /* Deduplicate Lobs */

	/*--------------------------- KCI SQLSTMT Types ---------------------------*/

#define  KCI_STMT_UNKNOWN 0                             /* Unknown statement */
#define  KCI_STMT_SELECT  1                              /* select statement */
#define  KCI_STMT_UPDATE  2                              /* update statement */
#define  KCI_STMT_DELETE  3                              /* delete statement */
#define  KCI_STMT_INSERT  4                              /* Insert SQLSTMT */
#define  KCI_STMT_CREATE  5                              /* create statement */
#define  KCI_STMT_DROP    6                                /* drop statement */
#define  KCI_STMT_ALTER   7                               /* alter statement */
#define  KCI_STMT_BEGIN   8                   /* begin ... (pl/sql statement)*/
#define  KCI_STMT_DECLARE 9                /* declare .. (pl/sql statement ) */
#define  KCI_STMT_CALL    10                      /* corresponds to kpu call */
	/*---------------------------------------------------------------------------*/

	/*--------------------------- KCI Parameter Types ---------------------------*/
#define KCI_PTYPE_UNK                 0                         /* unknown   */
#define KCI_PTYPE_TABLE               1                         /* table     */
#define KCI_PTYPE_VIEW                2                         /* view      */
#define KCI_PTYPE_PROC                3                         /* procedure */
#define KCI_PTYPE_FUNC                4                         /* function  */
#define KCI_PTYPE_PKG                 5                         /* package   */
#define KCI_PTYPE_TYPE                6                 /* user-defined type */
#define KCI_PTYPE_SYN                 7                         /* synonym   */
#define KCI_PTYPE_SEQ                 8                         /* sequence  */
#define KCI_PTYPE_COL                 9                         /* column    */
#define KCI_PTYPE_ARG                 10                        /* argument  */
#define KCI_PTYPE_LIST                11                        /* list      */
#define KCI_PTYPE_TYPE_ATTR           12    /* user-defined type's attribute */
#define KCI_PTYPE_TYPE_COLL           13        /* collection type's element */
#define KCI_PTYPE_TYPE_METHOD         14       /* user-defined type's method */
#define KCI_PTYPE_TYPE_ARG            15   /* user-defined type method's arg */
#define KCI_PTYPE_TYPE_RESULT         16/* user-defined type method's result */
#define KCI_PTYPE_SCHEMA              17                           /* schema */
#define KCI_PTYPE_DATABASE            18                         /* database */
#define KCI_PTYPE_RULE                19                             /* rule */
#define KCI_PTYPE_RULE_SET            20                         /* rule set */
#define KCI_PTYPE_EVALUATION_CONTEXT  21               /* evaluation context */
#define KCI_PTYPE_TABLE_ALIAS         22                      /* table alias */
#define KCI_PTYPE_VARIABLE_TYPE       23                    /* variable type */
#define KCI_PTYPE_NAME_VALUE          24                  /* name value pair */

	/*---------------------------------------------------------------------------*/

	/*----------------------------- KCI List Types ------------------------------*/
#define KCI_LTYPE_UNK           0                               /* unknown   */
#define KCI_LTYPE_COLUMN        1                             /* column list */
#define KCI_LTYPE_ARG_PROC      2                 /* procedure argument list */
#define KCI_LTYPE_ARG_FUNC      3                  /* function argument list */
#define KCI_LTYPE_SUBPRG        4                         /* subprogram list */
#define KCI_LTYPE_TYPE_ATTR     5                          /* type attribute */
#define KCI_LTYPE_TYPE_METHOD   6                             /* type method */
#define KCI_LTYPE_TYPE_ARG_PROC 7    /* type method w/o result argument list */
#define KCI_LTYPE_TYPE_ARG_FUNC 8      /* type method w/result argument list */
#define KCI_LTYPE_SCH_OBJ       9                      /* schema object list */
#define KCI_LTYPE_DB_SCH        10                   /* database schema list */
#define KCI_LTYPE_TYPE_SUBTYPE  11                           /* subtype list */
#define KCI_LTYPE_TABLE_ALIAS   12                       /* table alias list */
#define KCI_LTYPE_VARIABLE_TYPE 13                     /* variable type list */
#define KCI_LTYPE_NAME_VALUE    14                        /* name value list */
#define KCI_LTYPE_PACKAGE_TYPE  15                      /* package type list */

	/*---------------------------------------------------------------------------*/

	/*-------------------------- Memory Cartridge Services ---------------------*/
#define KCI_MEMORY_CLEARED  1

	/*-------------------------- Pickler Cartridge Services ---------------------*/
	typedef struct KCIPicklerTdsCtx KCIPicklerTdsCtx;
	typedef struct KCIPicklerTds KCIPicklerTds;
	typedef struct KCIPicklerImage KCIPicklerImage;
	typedef struct KCIPicklerFdo KCIPicklerFdo;
	typedef ub4 KCIPicklerTdsElement;

	typedef struct KCIAnyData KCIAnyData;

	typedef struct KCIAnyDataSet KCIAnyDataSet;
	typedef struct KCIAnyDataCtx KCIAnyDataCtx;

	/*---------------------------------------------------------------------------*/

	/*--------------------------- User Callback Constants -----------------------*/
#define KCI_UCBTYPE_ENTRY       1                          /* entry callback */
#define KCI_UCBTYPE_EXIT        2                           /* exit callback */
#define KCI_UCBTYPE_REPLACE     3                    /* replacement callback */

	/*---------------------------------------------------------------------------*/

	/*--------------------- NLS service type and constance ----------------------*/
#define KCI_NLS_DAYNAME1      1                    /* Native name for Monday */
#define KCI_NLS_DAYNAME2      2                   /* Native name for Tuesday */
#define KCI_NLS_DAYNAME3      3                 /* Native name for Wednesday */
#define KCI_NLS_DAYNAME4      4                  /* Native name for Thursday */
#define KCI_NLS_DAYNAME5      5                    /* Native name for Friday */
#define KCI_NLS_DAYNAME6      6              /* Native name for for Saturday */
#define KCI_NLS_DAYNAME7      7                /* Native name for for Sunday */
#define KCI_NLS_ABDAYNAME1    8        /* Native abbreviated name for Monday */
#define KCI_NLS_ABDAYNAME2    9       /* Native abbreviated name for Tuesday */
#define KCI_NLS_ABDAYNAME3    10    /* Native abbreviated name for Wednesday */
#define KCI_NLS_ABDAYNAME4    11     /* Native abbreviated name for Thursday */
#define KCI_NLS_ABDAYNAME5    12       /* Native abbreviated name for Friday */
#define KCI_NLS_ABDAYNAME6    13 /* Native abbreviated name for for Saturday */
#define KCI_NLS_ABDAYNAME7    14   /* Native abbreviated name for for Sunday */
#define KCI_NLS_MONTHNAME1    15                  /* Native name for January */
#define KCI_NLS_MONTHNAME2    16                 /* Native name for February */
#define KCI_NLS_MONTHNAME3    17                    /* Native name for March */
#define KCI_NLS_MONTHNAME4    18                    /* Native name for April */
#define KCI_NLS_MONTHNAME5    19                      /* Native name for May */
#define KCI_NLS_MONTHNAME6    20                     /* Native name for June */
#define KCI_NLS_MONTHNAME7    21                     /* Native name for July */
#define KCI_NLS_MONTHNAME8    22                   /* Native name for August */
#define KCI_NLS_MONTHNAME9    23                /* Native name for September */
#define KCI_NLS_MONTHNAME10   24                  /* Native name for October */
#define KCI_NLS_MONTHNAME11   25                 /* Native name for November */
#define KCI_NLS_MONTHNAME12   26                 /* Native name for December */
#define KCI_NLS_ABMONTHNAME1  27      /* Native abbreviated name for January */
#define KCI_NLS_ABMONTHNAME2  28     /* Native abbreviated name for February */
#define KCI_NLS_ABMONTHNAME3  29        /* Native abbreviated name for March */
#define KCI_NLS_ABMONTHNAME4  30        /* Native abbreviated name for April */
#define KCI_NLS_ABMONTHNAME5  31          /* Native abbreviated name for May */
#define KCI_NLS_ABMONTHNAME6  32         /* Native abbreviated name for June */
#define KCI_NLS_ABMONTHNAME7  33         /* Native abbreviated name for July */
#define KCI_NLS_ABMONTHNAME8  34       /* Native abbreviated name for August */
#define KCI_NLS_ABMONTHNAME9  35    /* Native abbreviated name for September */
#define KCI_NLS_ABMONTHNAME10 36      /* Native abbreviated name for October */
#define KCI_NLS_ABMONTHNAME11 37     /* Native abbreviated name for November */
#define KCI_NLS_ABMONTHNAME12 38     /* Native abbreviated name for December */
#define KCI_NLS_YES           39   /* Native string for affirmative response */
#define KCI_NLS_NO            40                 /* Native negative response */
#define KCI_NLS_AM            41           /* Native equivalent string of AM */
#define KCI_NLS_PM            42           /* Native equivalent string of PM */
#define KCI_NLS_AD            43           /* Native equivalent string of AD */
#define KCI_NLS_BC            44           /* Native equivalent string of BC */
#define KCI_NLS_DECIMAL       45                        /* decimal character */
#define KCI_NLS_GROUP         46                          /* group separator */
#define KCI_NLS_DEBIT         47                   /* Native symbol of debit */
#define KCI_NLS_CREDIT        48                  /* Native sumbol of credit */
#define KCI_NLS_DATEFORMAT    49                       /* Oracle date format */
#define KCI_NLS_INT_CURRENCY  50            /* International currency symbol */
#define KCI_NLS_LOC_CURRENCY  51                   /* Locale currency symbol */
#define KCI_NLS_LANGUAGE      52                            /* Language name */
#define KCI_NLS_ABLANGUAGE    53           /* Abbreviation for language name */
#define KCI_NLS_TERRITORY     54                           /* Territory name */
#define KCI_NLS_CHARACTER_SET 55                       /* Character set name */
#define KCI_NLS_LINGUISTIC_NAME    56                     /* Linguistic name */
#define KCI_NLS_CALENDAR      57                            /* Calendar name */
#define KCI_NLS_DUAL_CURRENCY 78                     /* Dual currency symbol */
#define KCI_NLS_WRITINGDIR    79               /* Language writing direction */
#define KCI_NLS_ABTERRITORY   80                   /* Territory Abbreviation */
#define KCI_NLS_DDATEFORMAT   81               /* Oracle default date format */
#define KCI_NLS_DTIMEFORMAT   82               /* Oracle default time format */
#define KCI_NLS_SFDATEFORMAT  83       /* Local string formatted date format */
#define KCI_NLS_SFTIMEFORMAT  84       /* Local string formatted time format */
#define KCI_NLS_NUMGROUPING   85                   /* Number grouping fields */
#define KCI_NLS_LISTSEP       86                           /* List separator */
#define KCI_NLS_MONDECIMAL    87               /* Monetary decimal character */
#define KCI_NLS_MONGROUP      88                 /* Monetary group separator */
#define KCI_NLS_MONGROUPING   89                 /* Monetary grouping fields */
#define KCI_NLS_INT_CURRENCYSEP 90       /* International currency separator */
#define KCI_NLS_CHARSET_MAXBYTESZ 91     /* Maximum character byte size      */
#define KCI_NLS_CHARSET_FIXEDWIDTH 92    /* Fixed-width charset byte size    */
#define KCI_NLS_CHARSET_ID    93                         /* Character set id */
#define KCI_NLS_NCHARSET_ID   94                        /* NCharacter set id */

#define KCI_NLS_MAXBUFSZ   100 /* Max buffer size may need for KCINlsGetInfo */

#define KCI_NLS_BINARY            0x1           /* for the binary comparison */
#define KCI_NLS_LINGUISTIC        0x2           /* for linguistic comparison */
#define KCI_NLS_CASE_INSENSITIVE  0x10    /* for case-insensitive comparison */

#define KCI_NLS_UPPERCASE         0x20               /* convert to uppercase */
#define KCI_NLS_LOWERCASE         0x40               /* convert to lowercase */

#define KCI_NLS_CS_IANA_TO_ORA   0   /* Map charset name from IANA to Oracle */
#define KCI_NLS_CS_ORA_TO_IANA   1   /* Map charset name from Oracle to IANA */
#define KCI_NLS_LANG_ISO_TO_ORA  2   /* Map language name from ISO to Oracle */
#define KCI_NLS_LANG_ORA_TO_ISO  3   /* Map language name from Oracle to ISO */
#define KCI_NLS_TERR_ISO_TO_ORA  4   /* Map territory name from ISO to Oracle*/
#define KCI_NLS_TERR_ORA_TO_ISO  5   /* Map territory name from Oracle to ISO*/
#define KCI_NLS_TERR_ISO3_TO_ORA 6   /* Map territory name from 3-letter ISO */
	/* abbreviation to Oracle               */
#define KCI_NLS_TERR_ORA_TO_ISO3 7   /* Map territory name from Oracle to    */
	/* 3-letter ISO abbreviation            */
#define KCI_NLS_LOCALE_A2_ISO_TO_ORA 8
	/*Map locale name from A2 ISO to oracle*/
#define KCI_NLS_LOCALE_A2_ORA_TO_ISO 9
	/*Map locale name from oracle to A2 ISO*/

	typedef struct KCIMsg  KCIMsg;
	typedef ub4            KCIWchar;

#define KCI_XMLTYPE_CREATE_KCISTRING 1
#define KCI_XMLTYPE_CREATE_CLOB      2
#define KCI_XMLTYPE_CREATE_BLOB      3

	/*------------------------- Kerber Authentication Modes ---------------------*/
#define KCI_KERBCRED_PROXY               1 /* Apply Kerberos Creds for Proxy */
#define KCI_KERBCRED_CLIENT_IDENTIFIER   2/*Apply Creds for Secure Client ID */

	/*------------------------- Database Startup Flags --------------------------*/
#define KCI_DBSTARTUPFLAG_FORCE 0x00000001  /* Abort running instance, start */
#define KCI_DBSTARTUPFLAG_RESTRICT 0x00000002      /* Restrict access to DBA */

	/*------------------------- Database Shutdown Modes -------------------------*/
#define KCI_DBSHUTDOWN_TRANSACTIONAL      1 /* Wait for all the transactions */
#define KCI_DBSHUTDOWN_TRANSACTIONAL_LOCAL 2  /* Wait for local transactions */
#define KCI_DBSHUTDOWN_IMMEDIATE           3      /* Terminate and roll back */
#define KCI_DBSHUTDOWN_ABORT              4 /* Terminate and don't roll back */
#define KCI_DBSHUTDOWN_FINAL              5              /* Orderly shutdown */

	/*------------------------- Version information -----------------------------*/
#define KCI_MAJOR_VERSION  12                       /* Major release version */
#define KCI_MINOR_VERSION  1                        /* Minor release version */

	/*---------------------- KCIIOV structure definitions -----------------------*/
	struct KCIIOV
	{
		void     *bfp;                            /* The Pointer to the data buffer */
		ub4       bfl;                                 /* Length of the Data Buffer */
	};
	typedef struct KCIIOV KCIIOV;

	/*---------------------------------------------------------------------------
	PRIVATE TYPES AND CONSTANTS
	---------------------------------------------------------------------------*/

	/* None */

	/*---------------------------------------------------------------------------
	PUBLIC FUNCTIONS
	---------------------------------------------------------------------------*/

	/* see kci_api.h or ocikp.h */

	/*---------------------------------------------------------------------------
	PRIVATE FUNCTIONS
	---------------------------------------------------------------------------*/

	/* None */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif                                              /* __KCI_h__ */