#pragma once

enum class ErrorCode
{
	None,
	StreamError,
	FileExistError,
	ElementCloseError,
	ElementTagsNotEmpty
};

class Error 
{
public:
	Error();
	void ThrowError2(ErrorCode errorCode);
private:
	ErrorCode err;
	const char* errMsg;
};