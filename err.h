#pragma once
#include <iostream>

namespace glExt {
	namespace err {
		class BaseException : private std::exception {
			int _errcode;
		public:
			inline BaseException():_errcode(-1),std::exception() {};
			inline BaseException(int err) :_errcode(err), std::exception() {};
			inline BaseException(const char* msg) : _errcode(-1), std::exception(msg) {};
			inline BaseException(int err, const char* msg) : _errcode(err), std::exception(msg) {};

			inline const char* what() const {
				return std::exception::what();
			};
			inline const int errcode() const {
				return this->_errcode;
			};
		};

		template<int _errercode>
		class glException : public BaseException {
		public:
			glException() :BaseException(_errercode) {};
			glException(const char* msg) : BaseException(_errercode, msg) {};
		};

		using glInitFailed = glException<0xC001>;
		using glWindowCreateFailed = glException<0xC002>;
		using glShaderCompileFailed = glException<0xC003>;
		using glLinkFailed = glException<0xC004>;
		using glImageLoadFailed = glException<0xC005>;
		using glOutofRange = glException<0xC006>;
		using glNotRegerister = glException<0xC007>;
		using glFontLoadFailed = glException<0xC008>;
	};
}
