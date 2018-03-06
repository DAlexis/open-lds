#include "gtest/gtest.h"

#include "MysqlWrapper.hpp"

TEST(InsertGeneratorTests, SimpleNumericInsert)
{
    MySQLInsertGenerator insert("table1");
    insert.set("num", -25);
    ASSERT_EQ(std::string("INSERT INTO table1(num) VALUES(-25)"), insert.generate());
}

TEST(InsertGeneratorTests, InsertCString)
{
    MySQLInsertGenerator insert("table1");
    char str[] = "Hello";
    insert.set("msg", str, true);
    ASSERT_EQ(std::string("INSERT INTO table1(msg) VALUES(\'Hello\')"), insert.generate());
}

TEST(InsertGeneratorTests, InsertStdString)
{
    MySQLInsertGenerator insert("table1");
    std::string str = "Hello";
    insert.set("msg", str, true);
    ASSERT_EQ(std::string("INSERT INTO table1(msg) VALUES(\'Hello\')"), insert.generate());
}

TEST(InsertGeneratorTests, ComplexInsert)
{
    MySQLInsertGenerator insert("table1");
    std::string str = "Hello";
    insert.set("msg", str, true);
    insert.set("num", -248);
    insert.set("flt", 3.1415);
    // This test is BAD, because map keys are unordered!! 
    ASSERT_EQ(std::string("INSERT INTO table1(flt, msg, num) VALUES(3.1415, \'Hello\', -248)"), insert.generate()) << "This test may crash if compiler version changed. Rewrite it, if you want";
}

