#include "record.h"

RecordView::RecordView(unsigned offset, std::fstream& inputFile,
                       unsigned blkOffset)
    : inBlkOffset(offset),
      sizeOfRecord(RECORD_SIZE),
      block(inputFile, blkOffset) {}

Record& RecordView::operator[](std::size_t index) {
  unsigned maxNumOfRecords = this->block.blkSize / this->sizeOfRecord;
  if (index >= maxNumOfRecords) {
    throw std::out_of_range("access exceeds block range");
  }

  throw NotImplemented();
}

const Record& RecordView::operator[](std::size_t index) const {
  unsigned maxNumOfRecords = this->block.blkSize / this->sizeOfRecord;
  if (index >= maxNumOfRecords) {
    throw std::out_of_range("access exceeds block range");
  }

  throw NotImplemented();
}

void RecordView::updateBlkOffset(unsigned blkOffset) {
  this->block.loadAt(blkOffset);
}

const size_t RECORD_SIZE = sizeof(Record);
