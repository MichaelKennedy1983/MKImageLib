#include "MKIHistogram.h"

#include "MKIImageConstants.h"

#include <fstream>
#include <filesystem>
#include <cmath>

namespace MKImage {
	using Path = std::filesystem::path;
	namespace FS = std::filesystem;

	MKIHistogram::MKIHistogram() :
		m_Data{}, m_EQData{}, m_Avg{ -1.0 }, m_Var{ -1.0 } {
	}

	MKIHistogram::MKIHistogram(const Image& image) :
		m_Data{}, m_EQData{}, m_Avg{-1.0}, m_Var{-1.0} {
		make(image);
		calcAvg();
		calcVar();
	}

	void MKIHistogram::make(const Image& image) {
		m_Data.resize(image.depth() + 1);

		for (const auto& i : image.data()) {
			for (const auto& j : i) {
				++m_Data.at(j);
			}
		}

		for (auto& i : m_Data) {
			i = i / (image.rows() * image.columns());
		}
	}

	void MKIHistogram::calcAvg() {
		m_Avg = 0;

		for (size_t i = 0; i < m_Data.size(); ++i) {
			m_Avg += (m_Data.at(i) * i);
		}
	}

	void MKIHistogram::calcVar() {
		if (m_Avg < 0) {
			calcAvg();
		}

		m_Var = 0;

		for (size_t i = 0; i < m_Data.size(); ++i) {
			m_Var += (std::pow(i - m_Avg, 2) * m_Data.at(i));
		}
	}

	void MKIHistogram::save(const std::string& file) {
		Path fileOut{ FS::current_path() };
		fileOut /= Consts::INPUT_FOLDER;
		fileOut /= Consts::OUTPUT_FOLDER;
		
		if (!FS::exists(fileOut)) {
			FS::create_directory(fileOut);
		}

		fileOut /= file;

		std::ofstream out(fileOut);

		if (out.is_open()) {
			out << "GS, Pixel Percent" << std::endl;

			for (size_t i = 0; i < m_Data.size(); ++i) {
				out << i << ", " << m_Data.at(i);

				if (i != m_Data.size() - 1) {
					out << std::endl;
				}
			}
		}
	}

	void MKIHistogram::saveEqualized(const std::string & file) {
		Path fileOut{ FS::current_path() };
		fileOut /= Consts::INPUT_FOLDER;
		fileOut /= Consts::OUTPUT_FOLDER;

		if (!FS::exists(fileOut)) {
			FS::create_directory(fileOut);
		}

		fileOut /= file;

		std::ofstream out(fileOut);

		if (out.is_open()) {
			out << "GS,  Equalized Pixel Modifier" << std::endl;

			for (size_t i = 0; i < m_EQData.size(); ++i) {
				out << i << ", " << m_EQData.at(i);

				if (i != m_EQData.size() - 1) {
					out << std::endl;
				}
			}
		}
	}

	void MKIHistogram::makeEqualized() {
		if (m_EQData.size() != m_Data.size()) {
			m_EQData.resize(m_Data.size());
		}

		double sum = 0;

		for (size_t i = 0; i < m_Data.size(); ++i) {
			sum += m_Data.at(i);
			m_EQData.at(i) = sum;
		}
	}
}
 