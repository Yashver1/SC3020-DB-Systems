#ifndef RECORD_H
#define RECORD_H
#include <array>
#include <iostream>
#include <string>
#include <cstring>
#include "utility.h"
#include "block.h"

struct RecordHeader {
  // unsigned long timestamp;
  unsigned offset;
  // Byte schemaKey;
  RecordHeader(unsigned offset) : offset(offset){};
};

struct Record {
  RecordHeader Header;               // 4
  unsigned TEAM_ID_HOME;             // 4
  float FG_PCT_HOME;                 // 4
  float FT_PCT_HOME;                 // 4
  float FG3_PCT_HOME;                // 4
  std::array<Byte, 3> GAME_DATE_EST; // 3
  Byte AST_HOME;                     // 1
  Byte REB_HOME;                     // 1
  Byte PTS_HOME;                     // 1
  Byte HOME_TEAM_WINS;               // 1
  // 1 byte padding

  Record()
      : Header{RecordHeader{0}}, TEAM_ID_HOME{0}, FG_PCT_HOME{0.0f},
        FT_PCT_HOME{0.0f}, FG3_PCT_HOME{0.0f},
        GAME_DATE_EST{std::array<Byte, 3>{}}, AST_HOME{0}, REB_HOME{0},
        PTS_HOME{0}, HOME_TEAM_WINS{false} {}

  Record(unsigned offset, std::string gameDateEst, std::string teamIdHome,
         std::string ptsHome, std::string fgPctHome, std::string ftPctHome,
         std::string fg3PctHome, std::string astHome, std::string rebHome,
         std::string homeTeamWins)
      : Header(RecordHeader{offset}) {
    try {
      std::vector<std::string> splitDate = split(gameDateEst, "/");

      Byte day = 0 | (std::stoi(splitDate[0]));
      Byte month = 0 | (std::stoi(splitDate[1]));
      Byte year = 0 | (std::stoi(splitDate[2]) - 2000);

      this->GAME_DATE_EST = {day, month, year};
      this->TEAM_ID_HOME = std::stoi(teamIdHome);
      this->FG_PCT_HOME = std::stof(fgPctHome);
      this->FT_PCT_HOME = std::stof(ftPctHome);
      this->FG3_PCT_HOME = std::stof(fg3PctHome);
      this->AST_HOME = 0 | (std::stoi(astHome));
      this->REB_HOME = 0 | (std::stoi(rebHome));
      this->PTS_HOME = 0 | (std::stoi(ptsHome));
      this->HOME_TEAM_WINS = 0 | (std::stoi(homeTeamWins));
    } catch (std::exception &e) {
      std::cerr << e.what() << std::endl;
      throw e;
    }
  }

  Record(unsigned offset, std::array<Byte, 3> gameDateEst, unsigned teamIdHome,
         Byte ptsHome, float fgPctHome, float ftPctHome, float fg3PctHome,
         Byte astHome, Byte rebHome, Byte homeTeamWins)
      : Header(RecordHeader{offset}), TEAM_ID_HOME(teamIdHome),
        FG_PCT_HOME(fgPctHome), FT_PCT_HOME(ftPctHome),
        FG3_PCT_HOME(fg3PctHome), GAME_DATE_EST(gameDateEst), AST_HOME(astHome),
        REB_HOME(rebHome), PTS_HOME(ptsHome), HOME_TEAM_WINS(homeTeamWins) {}

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
       << "HOME_TEAM_WINS: " << static_cast<int>(record.HOME_TEAM_WINS) << "\n";

    return os;
  }

  bool operator==(const Record &other) const {
    return TEAM_ID_HOME == other.TEAM_ID_HOME &&
           FG_PCT_HOME == other.FG_PCT_HOME &&
           FT_PCT_HOME == other.FT_PCT_HOME &&
           FG3_PCT_HOME == other.FG3_PCT_HOME &&
           GAME_DATE_EST == other.GAME_DATE_EST && AST_HOME == other.AST_HOME &&
           REB_HOME == other.REB_HOME && PTS_HOME == other.PTS_HOME &&
           HOME_TEAM_WINS == other.HOME_TEAM_WINS;
  }
};




class RecordView {
 public:
  class ProxyRecord {
    public:
    RecordView &parent;
    size_t index;
    ProxyRecord(RecordView &p, size_t index) : parent(p), index(index){};

    operator Record() const {
      std::vector<Byte> temp;
      temp.resize(parent.sizeOfRecord);
      for (unsigned i = 0; i < parent.sizeOfRecord; ++i) {
        temp[i] = parent.block[i + index * parent.sizeOfRecord];
        
      }
      Record newRecord{};
      std::memcpy(&newRecord,temp.data(),sizeof(Record));
      return newRecord;
    }

    //! current allocations are not very efficient
    ProxyRecord &operator=(Record rec) {
      std::vector<Byte> temp;
      temp.resize(parent.sizeOfRecord);
      std::memcpy(temp.data(), &rec, sizeof(Record));
      for(unsigned i = 0; i < parent.sizeOfRecord; ++i){
        parent.block[i+ index * parent.sizeOfRecord] = temp[i];
        //! this will cause an allocation on each due to parent overload
      }
      return *this;
    }
  };

  unsigned sizeOfRecord;
  unsigned numOfRecords;
  Record data;
  BlockView block;


  RecordView(std::fstream &inputFile,
             unsigned blkOffset = 0);

  ProxyRecord operator[](std::size_t index){
    unsigned maxNumOfRecords = this->block.blkSize / this->sizeOfRecord;
    if (index >= maxNumOfRecords) {
      throw std::out_of_range("access exceeds block range");
    }
  
    return ProxyRecord(*this,index);

  };
  void updateBlkOffset(unsigned blkOffset);
};

extern const size_t RECORD_SIZE;

#endif // !RECORD_H

// i * BLOCK_SIZE + j * RECORD_SIZE

// 12316 / BLOCKSIZE = block no.
// 12316 % BLOCKSIZE / RECORD_SIZE = record no.
