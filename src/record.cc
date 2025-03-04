#include "record.h"

RecordView::RecordView(std::fstream &inputFile,
                       unsigned blkOffset)
    : sizeOfRecord(RECORD_SIZE),
      block(inputFile, blkOffset) {
        numOfRecords = this->block.blkSize / this->sizeOfRecord;
      }


void RecordView::updateBlkOffset(unsigned blkOffset) {
  this->block.loadAt(blkOffset);
}

const size_t RECORD_SIZE = sizeof(Record);
