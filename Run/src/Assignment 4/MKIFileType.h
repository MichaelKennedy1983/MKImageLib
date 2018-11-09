#pragma once

#include <map>
#include <string>
#include <iostream>

namespace MKImage {

	class FileType {
	public:
		enum FType { Unknown = 0, P1, P2, P3, P4, P5, P6 };

	private:
		static const std::map<FType, std::string> m_EnumToString;
		static const std::map<std::string, FType> m_StringToEnum;

		FType m_FType;

	public:
		FileType();
		FileType(const FType fileType);
		explicit FileType(const std::string& type);

		FType& operator =(const std::string& rhs);
		
		bool operator ==(const std::string& rhs) const;
		bool operator ==(const FType rhs) const;

		friend std::ostream& operator <<(std::ostream& out, const FileType& ft);
		friend std::istream& operator >>(std::istream& in, FileType& ft);

		std::string toString() const;

	};
}

