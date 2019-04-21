#include "MSaver.h"
#include "RecordWriter.h"

void saveAutomap(const Automap& a, std::string filename) {
	RecordWriter<10> buff(filename);
	buff.set("1.1");
	buff.writeRecord();
	buff.set(a.deepestLevel);
	buff.writeRecord();
	for (auto floor : a.flags) {
		for (auto flag : floor) {
			buff.set(flag);
			buff.writeRecord();
		}
	}
}
