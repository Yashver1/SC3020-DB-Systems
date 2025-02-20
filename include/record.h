#ifndef RECORD_H
#define RECORD_H
#include <array>
#include <iostream>

#include "block.h"
#include "utility.h"

struct RecordHeader {
  unsigned long timestamp;
  unsigned offset;
  byte schemaKey;
  RecordHeader() : timestamp(0), offset(0), schemaKey(1) {};
};

struct Record {
  RecordHeader Header;
  unsigned TEAM_ID_HOME;
  float FG_PCT_HOME;
  float FT_PCT_HOME;
  float FG3_PCT_HOME;
  std::array<byte, 3> GAME_DATE_EST;
  byte AST_HOME;
  byte REB_HOME;
  byte PTS_HOME;
  bool HOME_TEAM_WINS;

  Record()
      : Header{RecordHeader{}},
        TEAM_ID_HOME{0},
        FG_PCT_HOME{0.0f},
        FT_PCT_HOME{0.0f},
        FG3_PCT_HOME{0.0f},
        GAME_DATE_EST{std::array<byte, 3>{}},
        AST_HOME{0},
        REB_HOME{0},
        PTS_HOME{0},
        HOME_TEAM_WINS{false} {}

  inline friend std::ostream &operator<<(std::ostream &os,
                                         const Record &record) {
    os << "TEAM_ID_HOME: " << record.TEAM_ID_HOME << "\n"
       << "FG_PCT_HOME: " << record.FG_PCT_HOME << "\n"
       << "FT_PCT_HOME: " << record.FT_PCT_HOME << "\n"
       << "FG3_PCT_HOME: " << record.FG3_PCT_HOME << "\n"
       << "DATE: " << static_cast<int>(record.GAME_DATE_EST[0]) << "/"
       << static_cast<int>(record.GAME_DATE_EST[1]) << "/"
       << static_cast<int>(record.GAME_DATE_EST[2]) << "\n"
       << "AST_HOME: " << static_cast<int>(record.AST_HOME) << "\n"
       << "REB_HOME: " << static_cast<int>(record.REB_HOME) << "\n"
       << "PTS_HOME: " << static_cast<int>(record.PTS_HOME) << "\n"
       << "HOME_TEAM_WINS: " << record.HOME_TEAM_WINS << "\n";

    return os;
  }
};

class RecordView {
  Record data;
  BlockView &block;
  unsigned sizeOfRecord;
  unsigned offset;

 public:
  RecordView(unsigned offset, BlockView &block);

  Record &operator[](std::size_t index);
  const Record &operator[](std::size_t index) const;
  void updateBlock(unsigned offset);
};

#endif  // !RECORD_H