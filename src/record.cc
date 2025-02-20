#include "record.h"

RecordView::RecordView(unsigned offset, BlockView& block)
    : offset(offset), block(block) {
  throw NotImplemented();
}

Record& RecordView::operator[](std::size_t index) { throw NotImplemented(); }

const Record& RecordView::operator[](std::size_t index) const {
  throw NotImplemented();
}

void RecordView::updateBlock(unsigned offset) { throw NotImplemented(); }
