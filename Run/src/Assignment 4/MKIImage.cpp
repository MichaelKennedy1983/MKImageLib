#include "MKIImage.h"

#include "MKIImageConstants.h"

#include <fstream>
#include <sstream>

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

	void Image::parallelMasking(const Mask& mask) {
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
	}

	void Image::frameProcessing(Image& otherImage, FrameOps op) {
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
			int i = 0;
			ss >> i;
		}

		if (count < 3) {
			std::getline(in, buffer);
		}
	}

	void Image::loadBin(const Path& file) {
		std::ifstream in;

		in.open(file, std::ios::binary);

		if (in.is_open()) {
			std::cout << "File opened successful.\n";

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
			m_Body.shrink_to_fit();
		}
	}

	void Image::loadText(const Path& file) {
		std::ifstream in;

		in.open(file);

		if (in.is_open()) {
			std::cout << "File opened successful.\n";

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
			m_Body.shrink_to_fit();
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

			std::cout << "Saved successfully.\n";
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

			std::cout << "Saved successfully.\n";
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
}