#include "MKIImage.h"

#include "MKIImageConstants.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

namespace MKImage {
	
	Image::Image() 
		: m_File{}, m_FileType{}, m_Rows{ -1 }, m_Columns{ -1 },
		m_Depth{ -1 }, m_MinLevel{ 255 }, m_MaxLevel{ 0 }, m_Body(),
		m_MinMaxMutex{}, m_BadImage{ true } {
	}

	Image::Image(const std::string& file) :
		Image{} {

		load(file);
	}

	Image::Image(const Image& other) 
		: m_File{ other.m_File }, m_FileType{ other.m_FileType },
		m_Rows{ other.m_Rows }, m_Columns{ other.m_Columns }, m_Depth{ other.m_Depth }, m_MinLevel{ other.m_MinLevel },
		m_MaxLevel{ other.m_MaxLevel }, m_Body(other.m_Body), m_MinMaxMutex{}, m_BadImage{ other.m_BadImage } {
	}

	Image::Image(const Image&& other) 
		: m_File{ std::move(other.m_File) }, m_FileType{ std::move(other.m_FileType) },
		m_Rows{ other.m_Rows }, m_Columns{ other.m_Columns }, m_Depth{ other.m_Depth }, m_MinLevel{ other.m_MinLevel },
		m_MaxLevel{ other.m_MaxLevel }, m_Body(std::move(other.m_Body)), m_MinMaxMutex{}, m_BadImage{ other.m_BadImage } {

	}

	void Image::protectRange(short& val) {
		if (val < 0)
			val = 0;
		if (val > m_Depth)
			val = m_Depth;
	}

	void Image::updateMinMax(short val) {
		m_MinMaxMutex.lock();
		if (val < m_MinLevel)
			m_MinLevel = val;
		if (val > m_MaxLevel)
			m_MaxLevel = val;
		m_MinMaxMutex.unlock();
	}

	void Image::load(const std::string& file) {
		m_File.assign(FS::current_path());
		m_File /= file;
		if (!FS::exists(m_File)) {
			m_File.assign(FS::current_path());
			m_File /= Consts::INPUT_FOLDER;
			m_File /= file;
		}
		if (!FS::exists(m_File)) {
			std::cout << "Image failed to load";
			m_BadImage = true;
			return;
		}
		
		readHeader(m_File.generic_string());

		if (m_FileType == "P5") {
			loadBin(m_File);
		}
		else {
			loadText(m_File);
		}
		m_BadImage = false;
		std::cout<< '\n' << file << " opened successful.\n";
	}

	void Image::save(const std::string& file, const std::string& comment) {
		Path outFile{ m_File.parent_path() };
		outFile /= Consts::OUTPUT_FOLDER;
		
		if (!FS::exists(outFile)) {
			FS::create_directory(outFile);
		}

		outFile /= file;

		if (m_FileType == FileType::P4 || m_FileType == FileType::P5 || m_FileType == FileType::P6) {
			saveBin(outFile, comment);
		}
		else {
			saveText(outFile, comment);
		}

		std::cout << '\n' << file << " saved successfully.\n";
	}

	void Image::saveCopy(const std::string& comment) {
		std::string outName{ m_File.filename().generic_string() };
		size_t pos = outName.find_last_of('.');

		if (pos != std::string::npos) {
			outName.insert(pos, "_COPY");
		}
		else {
			outName.append("_COPY.pgm");
		}

		save(outName, comment);
	}

	void Image::maskProcessing(const Mask& mask) {
		auto funcStart = std::chrono::high_resolution_clock::now();
		std::cout << "\nMasking started.\n";

		ImageData::iterator mid = m_Body.begin() + (std::distance(m_Body.begin(), m_Body.end()) / 2);
		ImageData::iterator oneQuarter = m_Body.begin() + (std::distance(m_Body.begin(), mid) / 2);
		ImageData::iterator threeQuarter = mid + (std::distance(mid, m_Body.end()) / 2);

		ImageData temp(m_Body.size(), std::vector<short>(m_Body.at(0).size()));

		Image::MaskProcessFunct mpFirst(*this, temp, m_Body.begin(), oneQuarter);
		Image::MaskProcessFunct mpSecond(*this, temp, oneQuarter, mid);
		Image::MaskProcessFunct mpThird(*this, temp, mid, threeQuarter);
		Image::MaskProcessFunct mpFourth(*this, temp, threeQuarter, m_Body.end());

		std::thread mpThreadOne(mpFirst, mask);
		std::thread mpThreadTwo(mpSecond, mask);
		std::thread mpThreadThree(mpThird, mask);
		std::thread mpThreadFour(mpFourth, mask);

		mpThreadOne.join();
		mpThreadTwo.join();
		mpThreadThree.join();
		mpThreadFour.join();

		m_Body = std::move(temp);
		
		auto funcEnd = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> funcRuntime = funcEnd - funcStart;
		std::cout << "Masking finished in " << funcRuntime.count() << " seconds.\n";
	}

	void Image::scalingProcessing(size_t newWidth, size_t newHeight, ScalingOps operation) {
		auto funcStart = std::chrono::high_resolution_clock::now();
		std::cout << "\nInterpolation started.\n";

		ImageData temp(newHeight, std::vector<short>(newWidth));

		ImageData::iterator mid = temp.begin() + (std::distance(temp.begin(), temp.end()) / 2);
		ImageData::iterator oneQuarter = temp.begin() + (std::distance(temp.begin(), mid) / 2);
		ImageData::iterator threeQuarter = mid + (std::distance(mid, temp.end()) / 2);

		Image::ScalingProcessFunct ipFirst(*this, temp, temp.begin(), oneQuarter, operation);
		Image::ScalingProcessFunct ipSecond(*this, temp, oneQuarter, mid, operation);
		Image::ScalingProcessFunct ipThird(*this, temp, mid, threeQuarter, operation);
		Image::ScalingProcessFunct ipFourth(*this, temp, threeQuarter, temp.end(), operation);

		double widthRatio = columns() / static_cast<double>(newWidth);
		double heightRatio = rows() / static_cast<double>(newHeight);

		std::thread ipThreadOne(ipFirst, widthRatio, heightRatio);
		std::thread ipThreadTwo(ipSecond, widthRatio, heightRatio);
		std::thread ipThreadThree(ipThird, widthRatio, heightRatio);
		std::thread ipThreadFour(ipFourth, widthRatio, heightRatio);

		ipThreadOne.join();
		ipThreadTwo.join();
		ipThreadThree.join();
		ipThreadFour.join();

		m_Body = std::move(temp);

		m_Columns = newWidth;
		m_Rows = newHeight;

		auto funcEnd = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> funcRuntime = funcEnd - funcStart;
		std::cout << "Interpolation finished in " << funcRuntime.count() << " seconds.\n";
	}

	void Image::frameProcessing(Image& otherImage, FrameOps op) {
		auto funcStart = std::chrono::high_resolution_clock::now();
		std::cout << "\nFrame processing started.\n";

		ImageData::iterator mid = m_Body.begin() + (std::distance(m_Body.begin(), m_Body.end()) / 2);
		ImageData::iterator oneQuarter = m_Body.begin() + (std::distance(m_Body.begin(), mid) / 2);
		ImageData::iterator threeQuarter = mid + (std::distance(mid, m_Body.end()) / 2);

		ImageData temp(m_Body.size(), std::vector<short>(m_Body.at(0).size()));

		Image::FrameProcessFunct fpFirst(*this, otherImage, temp, m_Body.begin(), oneQuarter, op);
		Image::FrameProcessFunct fpSecond(*this, otherImage, temp, oneQuarter, mid, op);
		Image::FrameProcessFunct fpThird(*this, otherImage, temp, mid, threeQuarter, op);
		Image::FrameProcessFunct fpFourth(*this, otherImage, temp, threeQuarter, m_Body.end(), op);

		std::thread fpThreadOne(fpFirst);
		std::thread fpThreadTwo(fpSecond);
		std::thread fpThreadThree(fpThird);
		std::thread fpThreadFour(fpFourth);

		fpThreadOne.join();
		fpThreadTwo.join();
		fpThreadThree.join();
		fpThreadFour.join();

		m_Body = std::move(temp);

		auto funcEnd = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> funcRuntime = funcEnd - funcStart;
		std::cout << "Frame processing finished in " << funcRuntime.count() << " seconds.\n";
	}

	/* #################### Private methods #################### */

	void Image::readHeader(const Path& file) {
		std::ifstream in(file);
		if (in.is_open()) {
			in >> m_FileType;
			removeComment(in);
			in >> m_Columns;
			in >> m_Rows;
			in >> m_Depth;
		}
	}

	void Image::removeComment(std::ifstream& in) const {
		char next;
		in.get(next);

		while (next == '\n' || next == '#') {
			if (next == '#')
				in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			in.get(next);
		}
		in.unget();
	}

	void Image::skipHeader(std::ifstream& in) const {
		in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		removeComment(in);

		std::string buffer;
		std::getline(in, buffer);

		std::stringstream ss(buffer);

		int count = 0;
		while (!ss.eof()) {
			int i;
			ss >> i;
			++count;
		}

		if (count < 3) {
			std::getline(in, buffer);
		}
	}

	void Image::loadBin(const Path& file) {
		std::ifstream in;

		in.open(file, std::ios::binary);

		if (in.is_open()) {
			skipHeader(in);

			m_Body = ImageData(m_Rows, std::vector<short>(m_Columns));

			//short tempPixValue = 0;
			for (int i = 0; i < m_Rows; ++i) {
				for (int j = 0; j < m_Columns; ++j) {
					in.read(reinterpret_cast<char*>(&m_Body.at(i).at(j)), sizeof(int8_t));
					//m_Body.at(i).at(j) = tempPixValue;
					updateMinMax(m_Body.at(i).at(j));
				}
			}
		}
	}

	void Image::loadText(const Path& file) {
		std::ifstream in;

		in.open(file);

		if (in.is_open()) {
			skipHeader(in);

			m_Body = ImageData(m_Rows, std::vector<short>(m_Columns));

			//short tempPixValue = 0;
			for (int i = 0; i < m_Rows; ++i) {
				for (int j = 0; j < m_Columns; ++j) {
					in >> m_Body.at(i).at(j);
					//m_Body.at(i).at(j) = tempPixValue;
					updateMinMax(m_Body.at(i).at(j));
				}
			}
		}
	}

 	void Image::saveBin(const Path& file, const std::string& comment) {
		std::ofstream out;

		out.open(file, std::ios::binary);

		if (out.is_open()) {
			out << m_FileType << '\n';
			if (comment.length() > 0) {
				out << comment << '\n';
			}
			out << m_Columns << ' ' << m_Rows << '\n';
			out << m_Depth << '\n';

			for (auto& i : m_Body) {
				for (auto& j : i) {
					out.write(reinterpret_cast<char*>(&j), sizeof(uint8_t));
				}
			}
		}
	}

	void Image::saveText(const Path& file, const std::string& comment) {
		std::ofstream out;

		out.open(file);

		if (out.is_open()) {
			out << m_FileType << '\n';
			if (comment.length() > 0) {
				out << comment << '\n';
			}
			out << m_Columns << ' ' << m_Rows << '\n';
			out << m_Depth << '\n';

			for (auto& i : m_Body) {
				for (auto& j : i) {
					out << j << ' ';
				}
				out << '\n';
			}
		}
	}

	/* #################### Functors #################### */

	Image::PointProcessFunct::PointProcessFunct(Image & image, ImageData & out,
														  ImageData::iterator begin, ImageData::iterator end)
		: m_Image(image), m_Out(out), m_Begin(begin), m_End(end) {
	}

	Image::MaskProcessFunct::MaskProcessFunct(Image & image, ImageData & out, ImageData::iterator begin,
														ImageData::iterator end)
		: m_Image(image), m_Out(out), m_Begin(begin), m_End(end) {
	}

	void Image::MaskProcessFunct::operator()(const Mask & mask) {
		int min = m_Image.depth();
		int max = 0;

		size_t countI = m_Begin - m_Image.data().begin();
		size_t countJ;
		for (m_Begin; m_Begin != m_End; ++m_Begin) {
			countJ = 0;
			for (auto j : *m_Begin) {
				short val = mask(m_Image.data(), countI, countJ);

				m_Image.protectRange(val);

				m_Out.at(countI).at(countJ) = val;

				if (val < min)
					min = val;
				if (val > max)
					max = val;

				++countJ;
			}
			++countI;
		}

		m_Image.updateMinMax(min);
		m_Image.updateMinMax(max);
	}

	Image::FrameProcessFunct::FrameProcessFunct(Image& image, Image& otherImage,
														  ImageData& out, ImageData::iterator begin,
														  ImageData::iterator end, Image::FrameProcessFunct::Operations operation)
		: m_Image(image), m_OtherImage(otherImage), m_Out(out), m_Begin(begin), m_End(end), m_Operation(operation) {
	}

	void Image::FrameProcessFunct::operator()() {
		int min = m_Image.depth();
		int max = 0;

		auto ops = operation();

		size_t countI = m_Begin - m_Image.data().begin();
		size_t countJ;
		for (m_Begin; m_Begin != m_End; ++m_Begin) {
			countJ = 0;
			for (auto j : *m_Begin) {
				short val = ops(j, m_OtherImage.data().at(countI).at(countJ));

				m_Image.protectRange(val);

				m_Out.at(countI).at(countJ) = val;

				if (val < min)
					min = val;
				if (val > max)
					max = val;

				++countJ;
			}
			++countI;
		}

		m_Image.updateMinMax(min);
		m_Image.updateMinMax(max);
	}

	std::function<short(short, short)> Image::FrameProcessFunct::operation() {
		switch (m_Operation) {
		case Operations::add:
			return [](short imageVal, short otherImageVal) -> short {
				return imageVal + otherImageVal;
			};
		case Operations::sub:
			return [](short imageVal, short otherImageVal) -> short {
				return imageVal - otherImageVal;
			};
		case Operations::mult:
			return [](short imageVal, short otherImageVal) -> short {
				return imageVal - otherImageVal;
			};
		case Operations::unknown:
			return [](short imageVal, short) -> short {
				return imageVal;
			};
		}
	}

	Image::ScalingProcessFunct::ScalingProcessFunct(Image& image, ImageData& out,
																ImageData::iterator begin, ImageData::iterator end,
																Operations operation) 
		: m_Image(image), m_Out(out), m_Begin(begin), m_End(end), m_Operation(operation) {
	}

	void Image::ScalingProcessFunct::operator()(double widthRatio, double heightRatio) {
		int min = m_Image.depth();
		int max = 0;

		auto ops = operation();

		size_t rowCount = m_Begin - m_Out.begin();
		size_t colCount;

		for (m_Begin; m_Begin != m_End; ++m_Begin) {
			for (colCount = 0; colCount < m_Out.at(rowCount).size(); ++colCount) {
				short val = ops(colCount, rowCount, widthRatio, heightRatio);

				m_Image.protectRange(val);

				m_Out.at(rowCount).at(colCount) = val;

				if (val < min) {
					min = val;
				}
				if (val > max) {
					max = val;
				}
			}
			++rowCount;
		}
		m_Image.updateMinMax(min);
		m_Image.updateMinMax(max);
	}

	std::function<short(size_t, size_t, double, double)> Image::ScalingProcessFunct::operation() {
		switch (m_Operation) {
		case Operations::nearestNeighbor:
			return [&m_Image = m_Image, &m_Out = m_Out](size_t column, size_t row, double ratioWidth, double ratioHeight) -> short {
				double columnIndex = column * ratioWidth;
				double rowIndex = row * ratioHeight;
				short val;
				val = m_Image.data().at(std::floor(rowIndex)).at(std::floor(columnIndex));
				return val;
			};
		case Operations::bilinear:
			return [this](size_t column, size_t row, double ratioWidth, double ratioHeight) -> short {
				size_t columnIndex = static_cast<size_t>(column * ratioWidth);
				size_t rowIndex = static_cast<size_t>(row * ratioHeight);
				double columnDiff = (column * ratioWidth) - columnIndex;
				double rowDiff = (row * ratioHeight) - rowIndex;

				short a = rangeCheckedPixel(columnIndex, rowIndex);
				short b = rangeCheckedPixel(columnIndex + 1, rowIndex);
				short c = rangeCheckedPixel(columnIndex, rowIndex + 1);
				short d = rangeCheckedPixel(columnIndex + 1, rowIndex + 1);

				short val = static_cast<short>(
					a * (1 - columnDiff) * (1 - rowDiff) +
					b * (columnDiff) * (1 - rowDiff) +
					c * (1 -columnDiff) * (rowDiff) +
					d * (columnDiff) * (rowDiff)
				);
				return val;
			};
		case Operations::unkown:
			return [](size_t, size_t, double, double) -> short {
				return short();
			};
		}
	}

	short Image::ScalingProcessFunct::rangeCheckedPixel(size_t column, size_t row) {
		rangeCheck(column, size_t(0), m_Image.data().at(0).size() - 1);
		rangeCheck(row, size_t(0), m_Image.data().size() - 1);

		return m_Image.data().at(row).at(column);
	}
}