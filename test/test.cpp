/*
 * test.cpp
 *
 *  Created on: Apr 7, 2023
 *      Author: rafal
 */

#include "test.h"
#include <cassert>
#include <iostream>
#include <string>


using namespace std;
using namespace CppUnit;
using namespace std;


class TestCbuffer : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCbuffer);
    CPPUNIT_TEST(test_cbuff_init);
    CPPUNIT_TEST(test_cbuff_put);
    CPPUNIT_TEST(test_cbuff_put_more_than_size);
    CPPUNIT_TEST(test_cbuff_get_more_than_size);
    CPPUNIT_TEST(test_fill_buffer_with_data_fit_size);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void) {
    	mTestObj = new CircBuffer(size);
    }
    void tearDown(void){
    	delete mTestObj;
    }

protected:
    void test_cbuff_init(void) {
    	uint32_t size = 55;
    	CPPUNIT_ASSERT(CircBuffer(size).size == size);
    }

    void test_cbuff_put(void) {
    	char c = 'a';
    	mTestObj->put(c);
    	CPPUNIT_ASSERT(mTestObj->get() == c);
    }

    void test_cbuff_put_more_than_size(void) {
    	bool result = fill_up_buffer();
    	CPPUNIT_ASSERT(result == false);
    	CPPUNIT_ASSERT(mTestObj->free_space() == 0);
    	CPPUNIT_ASSERT(mTestObj->available() == size);
    }

    void test_cbuff_get_more_than_size(void) {
    	char c = 'x';
    	fill_up_buffer(c);
    	for(int i=0; i<size; ++i){
    		CPPUNIT_ASSERT(mTestObj->get() == c);
    	}
    	CPPUNIT_ASSERT(mTestObj->get() == false);
    	CPPUNIT_ASSERT(mTestObj->free_space() == size);
    	CPPUNIT_ASSERT(mTestObj->available() == 0);
    }

    void test_fill_buffer_with_data_fit_size(void) {
    	string test_string {"12345"};
    	for(auto& c: test_string){
    		mTestObj->put(c);
    	}

    	int i=0;
    	char c=mTestObj->get();
    	while(c){
    		CPPUNIT_ASSERT(c == test_string[i++]);
    		c=mTestObj->get();
    	}
    }

    bool fill_up_buffer(char c = 'a'){
    	bool result = true;
    	for(int i=0; i<=size+5; ++i){
    		result = mTestObj->put(c);
    	}
    	return result;
    }

private:
    uint32_t size = 5;
    CircBuffer *mTestObj;
};


CPPUNIT_TEST_SUITE_REGISTRATION( TestCbuffer );

int main(int argc, char* argv[])
{
    // informs test-listener about testresults
    CPPUNIT_NS::TestResult testresult;

    // register listener for collecting the test-results
    CPPUNIT_NS::TestResultCollector collectedresults;
    testresult.addListener (&collectedresults);

    // register listener for per-test progress output
    CPPUNIT_NS::BriefTestProgressListener progress;
    testresult.addListener (&progress);

    // insert test-suite at test-runner by registry
    CPPUNIT_NS::TestRunner testrunner;
    testrunner.addTest (CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest ());
    testrunner.run(testresult);

    // output results in compiler-format
    CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
    compileroutputter.write ();

    // Output XML for Jenkins CPPunit plugin
    ofstream xmlFileOut("cppTestBasicMathResults.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    // return 0 if tests were successful
    return collectedresults.wasSuccessful() ? 0 : 1;
}

