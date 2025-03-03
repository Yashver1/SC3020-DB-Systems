#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "record.h"
#include "utility.h"
#include <vector>
#include <string>
#include <bitset>
#include <iostream>
#include <fstream>
#include <diskmanager.h>
#include <array>
#include <cstring>
using namespace std;

TEST_CASE("Block Size Test") { REQUIRE(BLOCK_SIZE != 0); }

TEST_CASE("Record Struct Test") {
  // !Assumption dates will be in 2000s but for memory saving we keep minus 2000 to fit in one byte
  string date = "24/12/2014";
  Record rec1{0, date, "1234", "100", "0.5", "0.75", "0.33", "10", "20", "1"};
  Record rec2{0, {24, 12, 14}, 1234, 100, 0.5, 0.75, 0.33, 10, 20, 1};
  
  REQUIRE(rec1.Header.offset == rec2.Header.offset);
  REQUIRE(rec1.TEAM_ID_HOME == rec2.TEAM_ID_HOME);
  REQUIRE(rec1.FG_PCT_HOME == rec2.FG_PCT_HOME);
  REQUIRE(rec1.FT_PCT_HOME == rec2.FT_PCT_HOME);
  REQUIRE(rec1.FG3_PCT_HOME == rec2.FG3_PCT_HOME);
  REQUIRE(rec1.GAME_DATE_EST[0] == rec2.GAME_DATE_EST[0]);
  REQUIRE(rec1.GAME_DATE_EST[1] == rec2.GAME_DATE_EST[1]);
  REQUIRE(rec1.GAME_DATE_EST[2] == rec2.GAME_DATE_EST[2]);

  REQUIRE(sizeof(Record) == 28 );

  std::vector<Record> buffer{};
  buffer.push_back(rec1);
  buffer.push_back(rec2);

  REQUIRE(sizeof(buffer[0]) * buffer.size() == 28*2);


}

TEST_CASE("Utility Tests"){
  vector<string> splitDate{"24", "12" , "14"};
  string date{"24/12/14"};
  REQUIRE(splitDate == split(date,"/"));

  string line{"22/12/2022	1610612740	126	0.484	0.926	0.382	25	46	1"};
  vector<string> splitLine{"22/12/2022", "1610612740", "126", "0.484", "0.926", "0.382", "25", "46", "1"};
  REQUIRE( splitLine == split(line,"\t"));

}

TEST_CASE("Disk Manager Tests"){

  fstream file{"games.txt",
    file.in
  };

  if(!file.is_open()){
    std::cerr << "File not Open";
  }

  DiskManager disk{};
  disk.txtToBinary(file,true);


}


TEST_CASE("Memory Tests"){

  //Testing allocations as bytes is okay
  string date = "24/12/2014";
  Record rec1{0, date, "1234", "100", "0.5", "0.75", "0.33", "10", "20", "1"};
  std::array<Record,1> buffer{};
  std::memcpy(&buffer, &rec1, sizeof(rec1));
  REQUIRE(sizeof(buffer) == RECORD_SIZE);
  Record rec2{};
  std::memcpy(&rec2,&buffer,sizeof(Record));
  REQUIRE(rec1 == rec2);




}

TEST_CASE("Test Binary File"){
  fstream inputFile{"data.bin", inputFile.binary | inputFile.in};
  fstream logFile{"log.txt", logFile.out | logFile.trunc};
  std::array<Record,1> buffer{};
  while(inputFile.peek() != EOF){
    inputFile.read(reinterpret_cast<char*>(&buffer), sizeof(Record));
    logFile << buffer[0];

  };


}
