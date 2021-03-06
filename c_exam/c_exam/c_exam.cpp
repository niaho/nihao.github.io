
/*************************************************************************************
本文件含3个考题，宏定义USE_EXAM1,USE_EXAM2,USE_EXAM2分别用于开启相应的测试代码	
   
   exam1: 实现hash表及双向链表的基础操作函数
   exam2: 扫描SQL字符串，找出所有参数信息
   exam3: 对一组对象按对象名进行排序，并实现二分法查找

*************************************************************************************/

#include "stdafx.h"
#include <windows.h>

#define USE_EXAM1
#define USE_EXAM2
#define USE_EXAM3

#ifdef USE_EXAM1
/*************************************************************************************
测试目标：实现hash表及双向链表操作函数
*************************************************************************************/

/*##以下定义排它锁(mutex),使用windows的临界区作为mutex##*/
typedef     CRITICAL_SECTION	mutex;
#define     mutex_init(h)		InitializeCriticalSection(&(h))
#define     mutex_lock(h)		EnterCriticalSection(&(h))
#define     mutex_unlock(h)		LeaveCriticalSection(&(h))
#define     mutex_close(h)		DeleteCriticalSection(&(h))

/*以下定义双向链表结构，本处定义的双向链表含链头和链元素，链头和链元素都使用XList结构，
  链头为一个独立的ＸList结构，初始化为prev,next成员皆指向自身，链元素不独立存在，而是
  作为其它对象的内嵌成员的方式存在，即对象通过内嵌的XList成员将自己加入到目标双向链表，
  一个对象可以内嵌多个XList成员分别对应不同的队列，从而实现将对象同时加入多个双向链表*/
typedef struct XList {
	struct XList *volatile prev;
	struct XList *volatile next;
}XList;

#define InitList(_l_) (_l_).prev=(_l_).next=&(_l_)
#define ZeroList(_l_) (_l_).prev=(_l_).next=NULL 
/*根据结构或类的成员地址计算结构或类的入口地址*/
#define EntryPtr(ptr,cla,nam) ((cla*)(((char*)ptr)-(size_t)(&(((cla*)0)->nam)))) 

//！！！下面代码不完整或有错误，需要修改 
/*将XList元素加入到链尾*/ 
#define ListAdd(_h_,_l_) do{\
	(_l_)->next = &(_h_);\
	(_l_)->prev = (_h_).prev;\
	(_h_).prev->next=(_l_);\
	(_h_).prev = (_l_);}while(0)

//！！！下面代码不完整或有错误，需要修改 
/*将XList元素插入到指定元素前面*/
#define ListInsB(_l0_,_l_)   do{\
	(_l_)->prev = (_l0_)->prev;\
	(_l_)->next = (_l0_);\
	(_l0_)->prev->next = (_l_);\
	(_l0_)->prev = (_l_);}while(0)

//！！！下面代码不完整或有错误，需要修改 
/*将XList元素插入到指定元素后面*/
#define ListInsA(_l0_,_l_)   do{\
	(_l_)->next = (_l0_)->next;\
	(_l_)->prev = (_l0_);\
	(_l0_)->next->prev=(_l_);\
	(_l0_)->next = (_l_);}while(0)

//！！！下面代码不完整或有错误，需要修改 
/*将XList元素从链表中移除*/
#define ListRmv(_l_) \
	do{ if((_l_)->next){\
	(_l_)->prev->next = (_l_)->next;\
	(_l_)->next->prev = (_l_)->prev;\
	(_l_)->prev=(_l_)->next = NULL;}}while(0)

/*XList链表遍历宏*/
#define Foreach(_l_,_h_)\
	for(XList *_l_=(_h_.next); _l_!=&(_h_); _l_=_l_->next)

/*测试双向链表是否为空链（即只有链头，没有成员）*/
#define LEmpty(_h_) ((_h_).next==&(_h_)&&(_h_).next!=NULL) 
/*测试链元素是否在双向链中*/
#define InList(_l_) ((_l_)->next!=NULL)

//！！！以下定义hash表及总双向链表
#define  HASH_SIZE  1023
mutex    oper_lock;   //操作锁，用于支持并发操作
XList    obj_dque;    //对象双向链表，使用对象的dque_link成员将其链入该链
XList    hash_tab[HASH_SIZE]; //对象hash表，各slot为双向链表的链头，使用对象的hash_link成员加入相应的slot

/*以下定义hash表管理对象的基类，所有hash表管理对象在链入hash槽的同时也要链入对象链表*/
enum ClaT {
	T_Cla1 = 1,
	T_Cla2,
};

#define IsA(_obj,_cla) _obj->class_type==T_##_cla

class  HsEle {
public:
	ClaT class_type;
protected:
	char *obj_name;   //对象名称（以此计算对象的hash值)
public:
	XList hash_link;  //用于将对象链接到hash表的slot链
	XList dque_link;  //用于对对象链接到对象双向链表
	
	unsigned int hasVal() //对象hash值计算函数
	{
		int	 h = 0;
		char *key = obj_name;
		while (*key)
		{
			h = (h * 37) ^ (*key++);
		}
		return (unsigned int )h;
	}
};

/*以下定义两个子类，后面程序中将使用这两个类的对象进行hash表操作*/
class  Cla1 : public HsEle
{
private:
	double  r8_val;
public:
	Cla1(const char *name, double val)
	{
		class_type = T_Cla1;
		obj_name = _strdup(name);
		r8_val = val;
	}
	double value()
	{
		return r8_val;
	}
};

class  Cla2 : public HsEle
{
private:
	__int64  i8_val;
public:
	Cla2(const char *name, __int64 val)
	{
		class_type = T_Cla2;
		obj_name = _strdup(name);
		i8_val = val;
	}
	__int64 value()
	{
		return i8_val;
	}
};

/*初始化hash表、对象双向链表及操作锁*/
void  hash_init()
{
	mutex_init(oper_lock);
	InitList(obj_dque);
	for (int i = 0; i < HASH_SIZE; i++)
	{
		InitList(hash_tab[i]);
	}
}

//!!!请将下面函数填写完整或按您的想法重写该函数
/*下面函数用于将对象加入hash表及对象双向链表*/
void  add_obj(HsEle *p_obj)
{
	unsigned int hs_val = p_obj->hasVal();
	unsigned int slot_no = hs_val % HASH_SIZE;
	
	mutex_lock(oper_lock);
	//todo
	ListAdd(obj_dque, &hash_tab[slot_no]);


	mutex_unlock(oper_lock);
}

//!!!请将下面函数填写完整或按您的想法重写该函数
/*下面函数用于将对象从hash表及对象双向链表中移出*/
void  remove_obj(HsEle *p_obj)
{
	mutex_lock(oper_lock);
	//todo



	mutex_unlock(oper_lock);
}


//!!!请将下面函数填写完整或按您的想法重写该函数
/*下面函数用于按名查找对象(必须使用hash值)*/
HsEle  *find_obj(const char *name)
{
	HsEle *p_obj = NULL;

	mutex_lock(oper_lock);
	//todo


	mutex_unlock(oper_lock);
	return p_obj;
}


//!!!请将下面函数填写完整或按您的想法重写该函数
/*下面函数用于累加所有对象的数值（r8_val或i8_val）*/
double  sum_all_val()
{
	HsEle   *p_ele;
	double  sum_val = 0.0;

	mutex_lock(oper_lock);
	Foreach(l, obj_dque)
	{
		p_ele = EntryPtr(l, HsEle, dque_link);
		if (IsA(p_ele, Cla1))
		{
			//todo

		}
		else
		{
			//todo

		}
	}
	mutex_unlock(oper_lock);
	return sum_val;
}

//!!!请将下面函数填写完整或按您的想法重写该函数
/*下面函数用于删除所有加入到hash表中的对象*/
void delete_all()
{
	mutex_lock(oper_lock);
	while (!LEmpty(obj_dque))
	{
		XList *p_link = obj_dque.next;
		HsEle *p_ele = EntryPtr(p_link, HsEle, dque_link);
		
		//todo





	}
	mutex_unlock(oper_lock);
}

//!!!测试题主入口1
void exma1()
{
	printf("123\n");
	Cla1 *p_obj1;
	Cla2 *p_obj2;

	hash_init();
	p_obj1 = new Cla1("cla1-obj1", 211.34);
	add_obj(p_obj1);
	p_obj1 = new Cla1("cla1-obj2", 121.22);
	add_obj(p_obj1);
	p_obj1 = new Cla1("cla1-obj3", 333.45);
	add_obj(p_obj1);
	p_obj2 = new Cla2("cla2-obj1", 112);
	add_obj(p_obj2);
	p_obj2 = new Cla2("cla2-obj2", 212);
	add_obj(p_obj2);
	p_obj2 = new Cla2("cla1-obj3", 311);
	add_obj(p_obj2);
	printf("step1: all value is %f\r\n", sum_all_val());

	p_obj1 = (Cla1*)find_obj("cla1-obj2");
	printf("step2: find cla1-obj2 value is %f\r\n", p_obj1->value());
	p_obj2 = (Cla2*)find_obj("cla2-obj2");
	printf("step2: find cla1-obj2 value is %f\r\n", p_obj2->value());
	printf("step2: all value is %f\r\n", sum_all_val());

	delete_all();
}
#endif




#ifdef USE_EXAM2
/***************************************************************************************************
  测试目标：扫描sql字符串，从中提取出参数占位符信息
  基本概念解释：
		sql字符串:  select * from  tab1 where id = :p1 and name = :p2 and info like '%:myif';
		参数占位符: 由':'号 + 参数名组成，用于指代SQL语句中的值，在执行过程中将被实际的参数值所取代
				    上例SQL串中 :p1, :p2 就是参数占位符，但 :myif不是参数占位符，因为它在常量字符串
					表达式之中,在扫描SQL串的过程中需要防止常量字符串值的影响
        常量字串值: 以'号开始并以'号结束的用于表示字符串值的常量表达式，本处假定字符串值内部不再含的'号
****************************************************************************************************/

/*参数占位符结构描述*/
typedef struct ParaInfo
{
	struct ParaInfo *next;  //用于形成参数信息链
	char   paraName[16];    //参数名称
	int    paraPos;         //参数 
}ParaInfo;


//!!!请将下面函数填写完整或按您的想法重写该函数
/**扫描SQL字符串，从中找出多个参数占位符的信息
参数：
    SQL字符串，内含有若干占位符，同时可能含有常量字符串值，假定其不含有注释，但允许含有制表符及回车换行符
返回值：
	参数占位符结构形成的链表
**/

ParaInfo *pick_sql_paras(const char *sql_str)
{
	int      status = 0;     //当前分析状态   0:未处于'号之内   1:处于'号之内   2:已扫描到占位符的标志,但对应占位符还没有处理完毕
	int      offset = 0;     //当前字符相对于SQL串的起始位置的偏移量
	ParaInfo *paras = NULL;

	//todo



	return paras;
}

//!!!测试题主入口1
void exam2()
{
	ParaInfo *paras = pick_sql_paras("select from tab1 where id=:_id and name=:_name and info like '::kk';");
	
	for (ParaInfo *p = paras; p != NULL; p = p->next)
	{
		printf("para_name = %s  position= %d \r\n", p->paraName, p->paraPos);
	}
}
#endif






#ifdef USE_EXAM3
/**********************************************************************************
测试目标：对一组对象按对象名进行排序，并实现二分法查找
***********************************************************************************/

/*下面结构定义关键词*/
typedef struct KeyWord
{
	const char *key;   //关键词的名称
	int   KEY_ID;      //关键词的取值      
}KeyWord;


KeyWord   key_words[] = {
	{"SELECT",1},
	{"FROM",2},
	{"TABLE",3},
	{"WHERE",4},
	{"UPDATE",5},
	{"INSERT",6},
	{"DELETE",7},
	{"INTEGER",8},
	{"NUMBER",9},
	{"CHAR",10}
};

//!!!请将下面函数填写完整或按您的想法重写该函数
/*下面函数用于对key_words表中的成员按关键词的名称进行排序*/
void sort_key_tab()
{
	//todo
}


//!!!请将下面函数填写完整(要求使用二分法查找)
/*下面函数用于按关键词的名从关键词表中查找关键词，返回关键词的取值*/
int  get_key_id(const char *key_name)
{
	int key_id = -1;
	//todo

	return key_id;
}

//!!!测试题主入口1
void exam3()
{
	int  key_id;
	
	sort_key_tab();
	key_id = get_key_id("UPDATE");
	printf("key update id is %d\r\n", key_id);
}
#endif





int main()
{
#ifdef USE_EXAM1
	exma1();
#endif

#ifdef USE_EXAM2
	exam2();
#endif

#ifdef USE_EXAM3
	exam3();
#endif

    return 0;
}

