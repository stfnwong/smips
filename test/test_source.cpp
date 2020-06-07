/* 
   TEST_SOURCE
   Unit tests for objects/functions that handle source 
   intermeidate representations.
 *
 * Stefan Wong 2020
 */

#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include <iostream> 
#include <iomanip>
#include <vector>
#include <string>

// unit(s) under test
#include "Address.hpp"
#include "Source.hpp"

TEST_CASE("init_text_info", "[classic]")
{
    TextInfo test_info;

    // just check the init is correct
    REQUIRE("\0" == test_info.label);
    REQUIRE("\0" == test_info.symbol);
    REQUIRE("\0" == test_info.errstr);

    REQUIRE(0 == test_info.line_num);
    REQUIRE(0 == test_info.addr);
    REQUIRE(false == test_info.error);
    REQUIRE(false == test_info.is_label);
    REQUIRE(false == test_info.is_directive);
    REQUIRE(false == test_info.is_imm);
    REQUIRE(false == test_info.is_symbol);
    REQUIRE(false == test_info.upper);
    REQUIRE(false == test_info.lower);

    TokenType blank_token_type;
    Opcode    blank_opcode;
    for(int i = 0; i < 3; ++i)
    {
        REQUIRE(0 == test_info.val[i]);
        REQUIRE(blank_token_type == test_info.type[i]);
    }
    REQUIRE(blank_opcode == test_info.opcode);
}


// ==== Creating a blank DataInfo
TEST_CASE("init_data_info", "[classic]")
{
    DataInfo test_info;

    REQUIRE(SYM_DIR_NONE == test_info.directive);
    REQUIRE("\0" == test_info.errstr);
    REQUIRE("\0" == test_info.label);

    REQUIRE(0 == test_info.data.size());
    REQUIRE(0 == test_info.line_num);
    REQUIRE(0 == test_info.addr);
    REQUIRE(0 == test_info.space);      // originally this was mean to be how many items are 

    REQUIRE(false == test_info.error);
    REQUIRE(false == test_info.is_label);
}


// ==== Creating a blank SourceInfo
TEST_CASE("init_source_info", "[classic]")
{
    SourceInfo test_info;

    // Could change the name to TextSegment and DataSegment or something...
    REQUIRE(0 == test_info.getTextInfoSize());
    REQUIRE(0 == test_info.getDataInfoSize());
    REQUIRE(0 == test_info.getNumErr());
    REQUIRE(0 == test_info.getNumLines());
}

TEST_CASE("source_info_get_data_by_addr", "[classic]")
{
    SourceInfo test_info;
    DataInfo data;

    // create some data
    uint8_t test_byte = 0xEF;
    data.addByte(test_byte);
    data.line_num = 2;
    data.addr = 0xBADA55;
    
    REQUIRE(1 == data.size());

    test_info.addData(data);
    REQUIRE(1 == test_info.getDataInfoSize());
    REQUIRE(0 == test_info.getTextInfoSize());

    // Now try to get the value at address 0xBADA55
    REQUIRE(0 == test_info.getDataByAddr(0));       // nothing here 
    REQUIRE(0 == test_info.getDataByAddr(0xDEADBEEF));  // nothing here
    REQUIRE(0xEF == test_info.getDataByAddr(0xBADA55));
}
