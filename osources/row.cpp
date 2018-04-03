#include "odbc.h"


int rowSize(FieldInfo *p_f_info,int n_col)
{
	int i;
	/*计算状态字节个数(每个字段4位,分别为:空标志 ,是否地址,字节数)*/
	int bytes_n = ROW_HEAD_SIZE+(n_col*2+7)/8;
	
	for(i=0;i<n_col;i++)
		bytes_n += getTypeLen(p_f_info[i].type_id);
	return bytes_n;
}

/*创建一个记录行*/
RhRow *createRow(int row_size)
{
	RhRow *p_row = malloc(row_size);
	memset(p_row,0x0,row_size);
	return p_row;
}
	
/*清除指针指向的数据*/
void freeRow(RhRow *p_row,FieldInfo *p_f_info,int field_num)
{
	int  i;
	for(i=0;i<field_num;i++)
	{
		if(isPtr(p_row,i))
		{
			void *p_dat  = getData(p_row,p_f_info,i,NULL);
			safe_free(p_dat);
		}
	}
	free(p_row);
}

/*第i个字段为空?*/
bool isNull(RhRow *p_row,int n)
{
	uchar ch = p_row->row_data[n/4];
	switch(n%4)
	{
	case 0:
		return (ch&0x1)!=0;
	case 1:
		return (ch&0x4)!=0;
	case 2:
		return (ch&0x10)!=0;
	case 3:
		return (ch&0x40)!=0;
	}
	return false;
}
	
void setNull(RhRow *p_row,int n)
{
	uchar *p_ch = p_row->row_data + n/4;
	switch(n%4)
	{
	case 0:
		*p_ch |= (uchar)0x1;
		break;
	case 1:
		*p_ch |= (uchar)0x4;
		break;
	case 2:
		*p_ch |= (uchar)0x10;
		break;
	case 3:
		*p_ch |= (uchar)0x40;
		break;
	}
}

void clrNull(RhRow *p_row,int n)
{
	uchar *p_ch = p_row->row_data+n/4;
	switch(n%4)
	{
	case 0:
		*p_ch &= (~(uchar)0x1);
		break;
	case 1:
		*p_ch &= (~(uchar)0x4);
		break;
	case 2:
		*p_ch &= (~(uchar)0x10);
		break;
	case 3:
		*p_ch &= (~(uchar)0x40);
		break;
	}
}

/*第i个字段为指针?*/
bool	 isPtr(RhRow *p_row,int n)
{
	uchar ch = p_row->row_data[n/4];
	switch(n%4)
	{
	case 0:
		return (ch&0x2)!=0;
	case 1:
		return (ch&0x8)!=0;
	case 2:
		return (ch&0x20)!=0;
	case 3:
		return (ch &0x80)!=0;
	}
	return false;
}
	
void	setPtr(RhRow *p_row,int n)
{
	uchar *p_ch = p_row->row_data+n/4;
	switch(n%4)
	{
	case 0:
		*p_ch |= (uchar)0x2;
		break;
	case 1:
		*p_ch |= (uchar)0x8;
		break;
	case 2:
		*p_ch |= (uchar)0x20;
		break;
	case 3:
		*p_ch |= (uchar)0x80;
		break;
	}
}

void clrPtr(RhRow *p_row,int n)
{
	uchar *p_ch = p_row->row_data+n/4;
	switch(n%4)
	{
	case 0:
		*p_ch &= (~(uchar)0x2);
		break;
	case 1:
		*p_ch &= (~(uchar)0x8);
		break;
	case 2:
		*p_ch &= (~(uchar)0x20);
		break;
	case 3:
		*p_ch &= (~(uchar)0x80);
		break;
	}
}

/*取字段数据*/
void  *getData(RhRow *p_row,FieldInfo *p_f_info,int n, int *p_len)
{
	if(isNull(p_row,n))
		return NULL;

	if(isPtr(p_row,n))
	{
		PtrField *p_ptr = (PtrField*)(p_row->row_data+p_f_info[n].off);
		if(p_len!=NULL)
			*p_len = p_ptr->d_len;
		return p_ptr->ptr;
	}
	else
	{
		if(p_len!=NULL)
			*p_len = getTypeLen(p_f_info->type_id);
		return  (void*)(p_row->row_data+p_f_info[n].off);
	}
}

/*取字段地址(无论是否指针型字段数据,皆返回其在记录中的位置指针)*/
void	 *getFieldPtr(RhRow *p_row,FieldInfo *p_f_info,int n)
{
	return  (void*)(p_row->row_data+p_f_info[n].off);
}

/*取字段的长度*/
int  getFieldLen(RhRow *p_row,FieldInfo *p_f_info,int n)
{
	if(isNull(p_row,n))
		return 0;

	if(isPtr(p_row,n))
	{
		PtrField *p_ptr = (PtrField*)(p_row->row_data+p_f_info[n].off);
			return p_ptr->d_len;
	}
	else
		return getTypeLen(p_f_info->type_id);
}

void	 setDataBool(RhRow *p_row,FieldInfo *p_f_info,int n,bool b)
{
	*(bool*)(p_row->row_data+p_f_info[n].off) = b;
	clrNull(p_row,n);
}

void	 setDataI1(RhRow *p_row,FieldInfo *p_f_info,int n,int8 i)
{
	*(int8*)(p_row->row_data+p_f_info[n].off) = i;
	clrNull(p_row,n);
}

void	 setDataI2(RhRow *p_row,FieldInfo *p_f_info,int n,int16 i)
{
	*(int16*)(p_row->row_data+p_f_info[n].off) = i;
	clrNull(p_row,n);
}

void	 setDataI4(RhRow *p_row,FieldInfo *p_f_info,int n,int32 i)
{
	*(int32*)(p_row->row_data+p_f_info[n].off) = i;
	clrNull(p_row,n);

}

void	 setDataI8(RhRow *p_row,FieldInfo *p_f_info,int n,int64 i)
{
	*(int64*)(p_row->row_data+p_f_info[n].off) = i;
	clrNull(p_row,n);
}

void	 setDataR4(RhRow *p_row,FieldInfo *p_f_info,int n,float f)
{
	*(float*)(p_row->row_data+p_f_info[n].off) = f;
	clrNull(p_row,n);
}

void	 setDataR8(RhRow *p_row,FieldInfo *p_f_info,int n,double f)
{
	*(double*)(p_row->row_data+p_f_info[n].off) = f;
	clrNull(p_row,n);

}

void	 setDataStr(RhRow *p_row,FieldInfo *p_f_info,int n,char *str)
{
	PtrField *p_ptr;
	void *p_dat  = getData(p_row,p_f_info,n,NULL);
	
	safe_free(p_dat);
	p_ptr = (PtrField*)(p_row->row_data+p_f_info[n].off);
	p_ptr->ptr = str;
	p_ptr->d_len = strlen(str)+1;
	clrNull(p_row,n);
	setPtr(p_row,n);
}

void	 setDataBin(RhRow *p_row,FieldInfo *p_f_info,int n,char *data,int32 len)
{
	PtrField *p_ptr;
	void *p_dat  = getData(p_row,p_f_info,n,NULL);
	
	safe_free(p_dat);
	p_ptr = (PtrField*)(p_row->row_data+p_f_info[n].off);
	p_ptr->ptr = data;
	p_ptr->d_len = len;
	clrNull(p_row,n);
	setPtr(p_row,n);
}
