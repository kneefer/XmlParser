#include "Error.h"

namespace Xml {

	Error::Error() :
		err(ErrorCode::None),
		errMsg(nullptr) { }

	void Error::ThrowError(ErrorCode errorCode)
	{
		err = errorCode;
		if (errorCode != ErrorCode::None) {
			switch (errorCode) {
			case ErrorCode::StreamError:
				errMsg = "Stream error has occured";
				break;
			case ErrorCode::FileExistError:
				errMsg = "File does not exist";
				break;
			case ErrorCode::ElementCloseError:
				errMsg = "No elements to close";
				break;
			case ErrorCode::ElementTagsNotEmpty:
				errMsg = "Not all element tags are closed. Closing remaining tags";
				break;
			}
			throw errMsg;
		}
		errMsg = nullptr;
	}
}