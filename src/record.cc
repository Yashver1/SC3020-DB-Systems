#include "record.h"

RecordView::RecordView(unsigned offset) : offset(offset) {
  throw NotImplemented();
}

Record& RecordView::operator[](std::size_t index) { throw NotImplemented(); }

const Record& RecordView::operator[](std::size_t index) const {
  throw NotImplemented();
}

const size_t RECORD_SIZE = sizeof(Record);

void RecordView::updateBlock(unsigned offset) { throw NotImplemented(); }
