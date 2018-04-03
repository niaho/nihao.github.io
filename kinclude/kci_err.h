#ifndef __KCI_ERR_H__
#define __KCI_ERR_H__
class IOEx{
public:
	int err_code;
	IOEx(int code)
	{
		err_code = code;
	}
};
#endif
