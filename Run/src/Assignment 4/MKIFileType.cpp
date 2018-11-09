#include "MKIFileType.h"

namespace MKImage {
	std::map<FileType::FType, std::string> const FileType::m_EnumToString{
		{FileType::Unknown, "Unknown"},
		{FileType::P1, "P1"},
		{FileType::P2, "P2"},
		{FileType::P3, "P3"},
		{FileType::P4, "P4"},
		{FileType::P5, "P5"},
		{FileType::P6, "P6"}
	};

	std::map<std::string, FileType::FType> const FileType::m_StringToEnum{
		{"Unknown", FileType::Unknown},
		{"P1", FileType::P1},
		{"P2", FileType::P2},
		{"P3", FileType::P3},
		{"P4", FileType::P4},
		{"P5", FileType::P5},
		{"P6", FileType::P6},
	};

	FileType::FileType() 
		: m_FType(FileType::Unknown) {
	}

	FileType::FileType(const FType fileType)
		: m_FType(fileType) {
	}

	FileType::FileType(const std::string& type) 
		: m_FType(m_StringToEnum.at(type)) {
	}

	FileType::FType& FileType::operator=(const std::string& rhs) {
		m_FType = m_StringToEnum.at(rhs);
		return m_FType;
	}

	bool FileType::operator==(const std::string& rhs) const {
		return m_FType == m_StringToEnum.at(rhs);
	}

	bool FileType::operator==(const FType rhs) const {
		return m_FType == rhs;
	}

	std::string FileType::toString() const {
		return m_EnumToString.at(m_FType);
	}

	std::ostream& operator<<(std::ostream& out, const FileType& ft) {
		out << ft.toString();
		return out;
	}

	std::istream& operator>>(std::istream& in, FileType& ft) {
		std::string temp;

		in >> temp;
		ft = temp;
		return in;
	}
}
