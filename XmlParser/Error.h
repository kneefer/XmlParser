#pragma once

namespace Xml {

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
		void ThrowError(ErrorCode errorCode);
	private:
		ErrorCode err;
		const char* errMsg;
	};
}