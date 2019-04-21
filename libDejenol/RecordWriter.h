#pragma once

#include<fstream>
#include<cassert>

template <int LEN> class RecordWriter {
	uint8_t buffer[LEN];
	std::ofstream file;
	size_t cursor;
public:
	RecordWriter(const std::string& filename) :cursor(0) {
		file.open(filename, std::ios::binary);
		memset(buffer, 0, LEN);
	}
	
	~RecordWriter() {
		if (cursor > 0) {
			writeRecord();
		}
		file.close();
	}

	void writeRecord() {
		file.write((char*)buffer, LEN);
		memset(buffer, 0, LEN);
		cursor = 0;
	}

	template<typename T>
	void set(T var) {
		assert(cursor + sizeof(var) < LEN);
		uint8_t * ptr = reinterpret_cast<uint8_t*>(&var);
		for (size_t i = 0; i < sizeof(var); i++) {
			buffer[cursor++] = ptr[i];
		}
	}

	void set(const char* str) {
		set(std::string(str));
	}


	void set(const std::string& str, size_t len = 0) {
		if (len == 0) {
			set(uint16_t(str.length()));
			for (auto c : str)
				set(c);
		}
		else {
			for (size_t i = 0; i < std::min(str.length(), len); i++) {
				set(str[i]);
			}
			for (size_t i = str.length(); i < len; i++) {
				set(' ');
			}
		}
	}

	void setIntCurrency(int64_t val) {
		set<int64_t>(val * 10000);
	}

};
