/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: zdenek
 *
 * Created on 3. září 2019, 16:59
 */

#include <cstdlib>
#include <list>

using namespace std;


#include "gtest/gtest.h"


#include "../src/AMWindow.h"

#include "../../AMVariableNames.h"


class AMPropertyPoolTest: public AMPropertyPool {

public:
    void plusDepthTraceItem(AMPropertyBase* item) {AMPropertyPool::plusDepthTraceItem(item);}
    void minusDepth() {AMPropertyPool::minusDepth();}
    void addToPool(AMPropertyBase* _property) {AMPropertyPool::addToPool(_property);}
    void removeFromPool(AMPropertyBase* _property) {AMPropertyPool::removeFromPool(_property);}
    void* TEST_getTraceitem(int index) {return m_trace_array[index];}
};


TEST(SmartProperties, Exception) {

    AMPropertyPoolTest pool;
    AMProperty<double> p1(pool, 1.1);
    AMProperty<double> p2(pool, 1.2);
    AMProperty<double> p3(pool, []{ throw std::out_of_range("AA"); return 5.5;});
    try {
        throw AMPropertyDeletedException(pool);
        FAIL() << "Looped AMproperty should throw exception on call";
    }
    catch(AMPropertyDeletedException& e) {
        EXPECT_STREQ(e.what(), "AMProperty: Detected deleted property. Member propertyXXX() show which property causes an exception. Run trace() to explain.");
        EXPECT_EQ(e.propertyEntered(), (AMPropertyBase*)0);
        EXPECT_EQ(e.propertyCaused(), (AMPropertyBase*)0);
    }
    catch(...) {
          FAIL() << "Looped AMproperty throws undefined exception";
    }
    pool.plusDepthTraceItem(&p1);
    pool.plusDepthTraceItem(&p2);
    try {
        throw AMPropertyLoopException(pool);
        FAIL() << "Looped AMproperty should throw exception on call";
    }
    catch(AMPropertyLoopException& e) {
        EXPECT_STREQ(e.what(), "AMProperty: Detected loop. Member propertyXXX() show which property causes an exception. Run trace() to explain.");
        EXPECT_EQ(e.propertyEntered(), &p1);
        EXPECT_EQ(e.propertyCaused(), (AMPropertyBase*)0);
    }
    catch(...) {
        FAIL() << "Looped AMroperty throws undefined exception";
    }
    pool.runLoop();
    try {
        throw AMPropertyNestingException(pool);
        FAIL() << "Looped AMproperty should throw exception on call";
    }
    catch(AMPropertyNestingException& e) {
        EXPECT_STREQ(e.what(), "AMProperty: Detected too depth nesting. Member propertyXXX() show which property causes an exception. Run trace() to explain.");
        EXPECT_EQ(e.propertyEntered(), &p1);
        EXPECT_EQ(e.propertyCaused(), &p2);
    }
    catch(...) {
        FAIL() << "Looped AMroperty throws undefined exception";
    }

    try {
        double d = (double)p3;
        FAIL() << "Looped AMproperty should throw exception on call";
    }
    catch(std::exception& e) {
        EXPECT_STREQ(e.what(), "AA");
    }
    catch(...) {
        FAIL() << "Looped AMroperty throws undefined exception";
    }
}

TEST(SmartProperties, PropertyPool_addRemove) {
    AMPropertyPoolTest pool;
    AMProperty<double> p1(pool, 1.1);
    AMProperty<double> p2(pool, 1.2);
    AMProperty<double> p3(pool, 1.3);
    AMProperty<double> p4(AMPropertyPool::defaultPool(), 1.4);
    char strExpected[16384];
    sprintf(strExpected, "D  0 -> [%p, st=NOCOMPUTED]\nD  1 -> [%p, st=NOCOMPUTED]\nD  2 -> [%p, st=NOCOMPUTED]\n", &p3, &p2, &p1);
    char strResult[16384];
    pool.dump(strResult, 16384);
    EXPECT_STREQ(strResult ,strExpected);
    pool.removeFromPool(&p2);
    pool.addToPool(&p4);
    sprintf(strExpected, "D  0 -> [%p, st=NOCOMPUTED]\nD  1 -> [%p, st=NOCOMPUTED]\nD  2 -> [%p, st=NOCOMPUTED]\n", &p4, &p3, &p1);
    pool.dump(strResult, 16384);
    EXPECT_STREQ(strResult ,strExpected);
    pool.removeFromPool(&p4);
    pool.removeFromPool(&p1);
    sprintf(strExpected, "D  0 -> [%p, st=NOCOMPUTED]\n", &p3);
    pool.dump(strResult, 16384);
    EXPECT_STREQ(strResult ,strExpected);
    pool.destroy();
    strExpected[0] = '\0';
    pool.dump(strResult, 16384);
    EXPECT_STREQ(strResult ,strExpected);
}

TEST(SmartProperties, PropertyPool_trace) {
    AMPropertyPoolTest pool;

    try {
        for(int i=1; i<AMPropertyPool::TRACE_SIZE; i++) {
            pool.plusDepthTraceItem((AMPropertyBase*)0x12345678);
        }
    }
    catch(...) {
        FAIL() << "Exception thrown too early";
    }
    try {
        pool.plusDepthTraceItem((AMPropertyBase*)0x12345678);
        FAIL() << "Exception not thrown";

    }
    catch(AMPropertyNestingException e) {

    }
    catch(...) {
        FAIL() << "Exception thrown unknown";
    }

    pool.reset();
    try {
        for(int i=1; i<AMPropertyPool::TRACE_SIZE; i++) {
            pool.plusDepthTraceItem((AMPropertyBase*)0x12345678);
        }
        for(int i=1; i<AMPropertyPool::TRACE_SIZE; i++) {
            pool.minusDepth();
        }
    }
    catch(...) {
        FAIL() << "Exception thrown too early";
    }
    try {
        pool.minusDepth();
        FAIL() << "Exception not thrown";
    }
    catch(std::underflow_error e) {
        EXPECT_STREQ(e.what(), "AMPropertyPool: Out of range");
    }
    catch(...) {
        FAIL() << "Exception thrown unknown";
    }
    return;
    pool.reset();
    AMProperty<double> p1(pool, 1.1);
    AMProperty<double> p2(pool, 1.2);
    AMProperty<double> p3(pool, 1.3);
    AMProperty<double> p4(pool, 1.4);

    EXPECT_FALSE(pool.isDestroyed());
    EXPECT_FALSE(pool.isInProgress());
    pool.plusDepthTraceItem(&p1);
    pool.plusDepthTraceItem(&p2);
    pool.plusDepthTraceItem(&p3);
    pool.runLoop();
    EXPECT_EQ((void *) &p1, pool.TEST_getTraceitem(0));
    EXPECT_EQ((void *) &p2, pool.TEST_getTraceitem(1));
    EXPECT_EQ((void *) &p3, pool.TEST_getTraceitem(2));
    std::string tracesreturned = pool.trace();
    std::stringstream traceexpected;
    traceexpected << hex << (void*)&p1 << endl << hex << (void*)&p2 << endl << hex << (void*)&p3 << endl;
    traceexpected.flush();
    EXPECT_STREQ(tracesreturned.c_str(),  traceexpected.str().c_str());
    pool.minusDepth();
    pool.minusDepth();
    pool.minusDepth();
    std::string tracesreturned2 = pool.trace();
    EXPECT_STREQ(tracesreturned2.c_str(), std::string("").c_str());
}


TEST(SmartProperties, PropertyPool_states) {
    AMPropertyPool pool;
    EXPECT_EQ(pool.getPoolState(), AMPropertyState::NORMAL);
    pool.setModified();
    EXPECT_EQ(pool.getPoolState(), AMPropertyState::MODIFIED);
    pool.destroy();
    EXPECT_EQ(pool.getPoolState(), AMPropertyState::DESTROYED_NORMAL);
    pool.setModified();
    EXPECT_EQ(pool.getPoolState(), AMPropertyState::DESTROYED_MODIFIED);

    EXPECT_EQ(AMPropertyPool::defaultPool().getPoolState(), AMPropertyState::DESTROYED_NORMAL);
}

TEST(SmartProperties, ModifyResultTest) {

    AMPropertyPool pp;
    AMProperty<AMRect, AMRectSubproperties> rect_1(pp);
    AMRect res = rect_1.getResult();
    EXPECT_EQ(res, (AMRect{{0.0,0.0},{0.0,0.0}}));

    AMRect rrr = {{1.1,1.2},{1.3,1.4}};

    AMProperty<AMRect, AMRectSubproperties> rect_2(pp, rrr);
    EXPECT_EQ(rect_2.getResult(), (AMRect({{1.1,1.2},{1.3,1.4}})));

    AMProperty<AMRect, AMRectSubproperties> rect_3(pp, rrr);
    rect_3->origin = AMPoint{2.1,2.2};
    rect_3->sz = AMSize{2.3,2.4} ;
    EXPECT_EQ(rect_3.getResult(), (AMRect({{2.1,2.2},{2.3,2.4}})));

    AMProperty<AMRect, AMRectSubproperties> rect_4(pp, rrr);
    rect_4->origin->x = 3.1;
    rect_4->origin->y = 3.2;
    rect_4->sz->width = 3.3;
    rect_4->sz->height = 3.4;
    EXPECT_EQ(rect_4.getResult(), (AMRect({{3.1,3.2},{3.3,3.4}})));

}

TEST(SmartProperties, FunctionsTest) {
    AMRect rrr = {{1.1,1.2},{1.3,1.4}};
    AMPropertyPool pp;
    AMProperty<AMRect, AMRectSubproperties> rect_1(pp, rrr);
    AMProperty<AMRect, AMRectSubproperties> rect_2(pp, rrr);
    rect_2->origin->x = [&](){return rect_1->origin->x+10.0+rect_1->sz->width;};
    rect_2->origin->y = [&](){return rect_1->origin->y * 2.0;};
    rect_2->sz->width = [](){return 90.0;};
    rect_2->sz->height = [&rect_2](){return rect_2->sz->width / 2.0 ;};
    EXPECT_EQ(rect_2.getResult(), (AMRect({{1.1,1.2},{1.3,1.4}})));
    EXPECT_EQ(rect_2.getResult(), (AMRect({{1.1,1.2},{1.3,1.4}})));

    //property operator +
    AMProperty<AMRect, AMRectSubproperties> rect_3(pp, rrr);
    rect_2->origin->x = [&](){return rect_1->origin->x + rect_1->origin->x;};
    rect_2->origin->y = [&]()->double{return rect_1->origin->y;};
    rect_2->sz->width = [](){return 90.0;};
    rect_2->sz->height = [&rect_2](){return rect_2->sz->width / 2.0 ;};
    EXPECT_EQ(rect_2.getResult(), (AMRect({{1.1,1.2},{1.3,1.4}})));
    EXPECT_EQ(rect_2.getResult(), (AMRect({{1.1,1.2},{1.3,1.4}})));
}


class myview: public AMWindow
{
public:
    AMView menubar;
    AMView fileBtn;
    AMView editBtn;
    AMView optionsBtn;
    AMView exitBtn;
    AMView content;
    AMView text1;
    AMView text2;
    AMView image;
    AMView buttonContainer;
    AMView cancelBtn;
    AMView OKBtn;

public:
    myview();
};

myview::myview() :
    AMWindow(),
    menubar(this,"menubar"),
    fileBtn(&menubar,"fileBtn"),
    editBtn(&menubar,"editBtn"),
    optionsBtn(&menubar,"optionsBtn"),
    exitBtn(&menubar,"exitBtn"),
    content(this,"content"),
    text1(&content,"text1"),
    text2(&content,"text2"),
    image(&content,"image"),
    buttonContainer(this,"buttonContainer"),
    cancelBtn(&buttonContainer,"cancelBtn"),
    OKBtn(&buttonContainer,"OKBtn")
{
    rect = AMRect{AMPoint{10.0,20.0},AMSize{800.0,600.0}};

    menubar.rect->origin = [&]{
        return (AMPoint)this->rect->origin;
    };

    auto mrsw = [&] {
        return (double)this->rect->sz->width;
    };
    menubar.rect->sz->width = mrsw;

    menubar.rect->sz->height = 35.0;

    fileBtn.rect->origin = [&]{return(AMPoint) menubar.rect->origin;};
    fileBtn.rect->sz->width = 75.0;
    fileBtn.rect->sz->height = [&]{return (double)menubar.rect->sz->height;};

    editBtn.rect->origin = [&]{return fileBtn.rect->origin + AMPoint{100.0, 0.0};};
    editBtn.rect->sz->width = 75.0;
    editBtn.rect->sz->height = [&]{return (double)menubar.rect->sz->height;};

    optionsBtn.rect->origin = [&]{return editBtn.rect->origin + AMPoint{100.0, 0.0};};
    optionsBtn.rect->sz->width = 75.0;
    optionsBtn.rect->sz->height = [&]{return (double)menubar.rect->sz->height;};

    exitBtn.rect->origin = [&]{return optionsBtn.rect->origin + AMPoint{100.0, 0.0};};
    exitBtn.rect->sz->width = 75.0;
    exitBtn.rect->sz->height = [&]{return (double)menubar.rect->sz->height;};

    content.rect->origin->x = [&]{return (double)this->rect->origin->x;};
    content.rect->sz->width = [&]{return (double)this->rect->sz->width;};
    content.rect->sz->height = [&]{return this->rect->sz->height - 35.0 - 40.0;};
    content.rect->origin->y = [&]{return this->rect->origin->y + 35.0;};

    text1.rect->origin = [&]{return (AMPoint)content.rect->origin;};
    text1.rect->sz->width = [&]{return (double)content.rect->sz->width;};
    text1.rect->sz->height = [&]{return content.rect->sz->height / 3.0;};

    text2.rect->origin->x = [&]{return (double)content.rect->origin->x;};
    text2.rect->origin->y = [&]{return content.rect->origin->y + content.rect->sz->height / 3.0;};
    text2.rect->sz->width = [&]{return (double)content.rect->sz->width;};
    text2.rect->sz->height = [&]{return content.rect->sz->height / 3.0;};

    image.rect->origin->x = [&]{return (double)content.rect->origin->x;};
    image.rect->origin->y = [&]{return content.rect->origin->y + content.rect->sz->height / 3.0 * 2.0;};
    image.rect->sz->width = [&]{return (double)content.rect->sz->width;};
    image.rect->sz->height = [&]{return content.rect->sz->height / 3.0;};


    buttonContainer.rect->origin->x = [&]{return (double)this->rect->origin->x;};
    buttonContainer.rect->sz->width = [&]{return (double)this->rect->sz->width;};
    buttonContainer.rect->sz->height = 40.0;
    buttonContainer.rect->origin->y = [&]{return this->rect->origin->y + this->rect->sz->height - 40.0;};

    cancelBtn.rect->origin = [&]{return (AMPoint)buttonContainer.rect->origin;};
    cancelBtn.rect->sz->width = 75.0;
    cancelBtn.rect->sz->height = [&]{return (double)buttonContainer.rect->sz->height;};

    OKBtn.rect->origin->x = [&]{return buttonContainer.rect->origin->x + buttonContainer.rect->sz->width - 75.0;};
    OKBtn.rect->origin->y = [&]{return (double)buttonContainer.rect->origin->y;};
    OKBtn.rect->sz->width = 75.0;
    OKBtn.rect->sz->height = [&]{return (double)buttonContainer.rect->sz->height;};

}



TEST(SmartProperties, Functions) {
	int res;

    AMFunction<int(int,int)> amfn1([](int x, int y){return x+y;});
    res = amfn1(8, 57);
    EXPECT_EQ(res, 65);

    AMFunction<int(int,int)> amfn2;
    EXPECT_FALSE(amfn2);
    EXPECT_FALSE(amfn2 != nullptr);
    EXPECT_FALSE(nullptr != amfn2);
    EXPECT_TRUE(amfn2 == nullptr);
    EXPECT_TRUE(nullptr == amfn2);
    amfn2 = amfn1;
    res = amfn2(88, -5);
    EXPECT_EQ(res, 83);
    EXPECT_TRUE(amfn2);
    EXPECT_TRUE(amfn2 != nullptr);
    EXPECT_TRUE(nullptr != amfn2);
    EXPECT_FALSE(amfn2 == nullptr);
    EXPECT_FALSE(nullptr == amfn2);

    AMFunction<int(int,int)> amfn3 = std::move(amfn2);
    res = amfn3(81, 9);
    EXPECT_EQ(res, 90);
    EXPECT_TRUE(amfn3);
    amfn3 = nullptr;
    EXPECT_FALSE(amfn3);

    AMFunction<int(int,int, int)> amfn5;
    amfn5 = [](int a, int b, int c){return a+b+c;};
    res = amfn5(81, 9, 12);
    EXPECT_EQ(res, 102);
    amfn5 = [&](int, int, int){return res + 61;};
    res = amfn5(1,2,3);
    EXPECT_EQ(res, 163);

    int externint = 24;
    AMFunction<int(int,int,int)> amfn6([&](int x, int y, int z) {
        return x+y+z+externint;
    });
    AMFunction<int(int,int,int)> amfn7([&](int x, int y, int z) {
        return x+y+z-externint;
    });

    res = amfn6( 5, 15, 25);
    EXPECT_EQ(res, 69);
    res = amfn7( 5, 15, 25);
    EXPECT_EQ(res, 21);
    amfn6 = amfn7;
    res = amfn6( 5, 15, 25);
    EXPECT_EQ(res, 21);
    res = amfn7( 5, 15, 25);
    EXPECT_EQ(res, 21);
}



TEST(SmartProperties, BasicTest) {

    myview mv;

    mv.getPool().reset();
    AMRect r = mv.menubar.rect;
    EXPECT_EQ(r, (AMRect{AMPoint{10.000000,20.000000},AMSize{800.000000,35.000000}}) );
    EXPECT_EQ(r.origin.x, 10.0);
    EXPECT_EQ(r.origin.y, 20.0);
    EXPECT_EQ(r.sz.width, 800.0);
    EXPECT_EQ(r.sz.height, 35.0);
    EXPECT_EQ((AMRect)mv.fileBtn.rect, (AMRect{AMPoint{10.000000,20.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.editBtn.rect, (AMRect{AMPoint{110.000000,20.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.optionsBtn.rect, (AMRect{AMPoint{210.000000,20.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.exitBtn.rect, (AMRect{AMPoint{310.000000,20.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.content.rect, (AMRect{AMPoint{10.000000,55.000000},AMSize{800.000000,525.000000}}) );
    EXPECT_EQ((AMRect)mv.text1.rect, (AMRect{AMPoint{10.000000,55.000000},AMSize{800.000000,175.000000}}) );
    EXPECT_EQ((AMRect)mv.text2.rect, (AMRect{AMPoint{10.000000,230.000000},AMSize{800.000000,175.000000}}) );
    EXPECT_EQ((AMRect)mv.image.rect, (AMRect{AMPoint{10.000000,405.000000},AMSize{800.000000,175.000000}}) );
    EXPECT_EQ((AMRect)mv.buttonContainer.rect, (AMRect{AMPoint{10.000000,580.000000},AMSize{800.000000,40.000000}}) );
    EXPECT_EQ((AMRect)mv.cancelBtn.rect, (AMRect{AMPoint{10.000000,580.000000},AMSize{75.000000,40.000000}}) );
    EXPECT_EQ((AMRect)mv.OKBtn.rect, (AMRect{AMPoint{735.000000,580.000000},AMSize{75.000000,40.000000}}) );

    mv.rect = AMRect{AMPoint{100.0,200.0},AMSize{1000.0,399.0}};
    mv.getPool().reset();
    EXPECT_EQ((AMRect)mv.menubar.rect, (AMRect{AMPoint{100.000000,200.000000},AMSize{1000.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.fileBtn.rect, (AMRect{AMPoint{100.000000,200.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.editBtn.rect, (AMRect{AMPoint{200.000000,200.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.optionsBtn.rect, (AMRect{AMPoint{300.000000,200.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.exitBtn.rect, (AMRect{AMPoint{400.000000,200.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.content.rect, (AMRect{AMPoint{100.000000,235.000000},AMSize{1000.000000,324.000000}}) );
    EXPECT_EQ((AMRect)mv.text1.rect, (AMRect{AMPoint{100.000000,235.000000},AMSize{1000.000000,108.0}}) );
    EXPECT_EQ((AMRect)mv.text2.rect, (AMRect{AMPoint{100.000000,343.0},AMSize{1000.000000,108.0}}) );
    EXPECT_EQ((AMRect)mv.image.rect, (AMRect{AMPoint{100.000000,451.0},AMSize{1000.000000,108.0}}) );
    EXPECT_EQ((AMRect)mv.buttonContainer.rect, (AMRect{AMPoint{100.000000,559.000000},AMSize{1000.000000,40.000000}}) );
    EXPECT_EQ((AMRect)mv.cancelBtn.rect, (AMRect{AMPoint{100.000000,559.000000},AMSize{75.000000,40.000000}}) );
    EXPECT_EQ((AMRect)mv.OKBtn.rect, (AMRect{AMPoint{1025.000000,559.000000},AMSize{75.000000,40.000000}}) );
}


TEST(SmartProperties, BasicTest_noreset_nocompute) {

    myview mv;

    mv.getPool().reset();
    EXPECT_EQ((AMRect)mv.menubar.rect, (AMRect{AMPoint{10.000000,20.000000},AMSize{800.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.fileBtn.rect, (AMRect{AMPoint{10.000000,20.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.editBtn.rect, (AMRect{AMPoint{110.000000,20.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.optionsBtn.rect, (AMRect{AMPoint{210.000000,20.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.exitBtn.rect, (AMRect{AMPoint{310.000000,20.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.content.rect, (AMRect{AMPoint{10.000000,55.000000},AMSize{800.000000,525.000000}}) );
    EXPECT_EQ((AMRect)mv.text1.rect, (AMRect{AMPoint{10.000000,55.000000},AMSize{800.000000,175.000000}}) );
    EXPECT_EQ((AMRect)mv.text2.rect, (AMRect{AMPoint{10.000000,230.000000},AMSize{800.000000,175.000000}}) );
    EXPECT_EQ((AMRect)mv.image.rect, (AMRect{AMPoint{10.000000,405.000000},AMSize{800.000000,175.000000}}) );
    EXPECT_EQ((AMRect)mv.buttonContainer.rect, (AMRect{AMPoint{10.000000,580.000000},AMSize{800.000000,40.000000}}) );
    EXPECT_EQ((AMRect)mv.cancelBtn.rect, (AMRect{AMPoint{10.000000,580.000000},AMSize{75.000000,40.000000}}) );
    EXPECT_EQ((AMRect)mv.OKBtn.rect, (AMRect{AMPoint{735.000000,580.000000},AMSize{75.000000,40.000000}}) );

    mv.rect = AMRect{AMPoint{100.0,200.0},AMSize{1000.0,399.0}};
    EXPECT_EQ((AMRect)mv.menubar.rect, (AMRect{AMPoint{100.000000,200.000000},AMSize{1000.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.fileBtn.rect, (AMRect{AMPoint{100.000000,200.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.editBtn.rect, (AMRect{AMPoint{200.000000,200.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.optionsBtn.rect, (AMRect{AMPoint{300.000000,200.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.exitBtn.rect, (AMRect{AMPoint{400.000000,200.000000},AMSize{75.000000,35.000000}}) );
    EXPECT_EQ((AMRect)mv.content.rect, (AMRect{AMPoint{100.000000,235.000000},AMSize{1000.000000,324.000000}}) );
    EXPECT_EQ((AMRect)mv.text1.rect, (AMRect{AMPoint{100.000000,235.000000},AMSize{1000.000000,108.0}}) );
    EXPECT_EQ((AMRect)mv.text2.rect, (AMRect{AMPoint{100.000000,343.0},AMSize{1000.000000,108.0}}) );
    EXPECT_EQ((AMRect)mv.image.rect, (AMRect{AMPoint{100.000000,451.0},AMSize{1000.000000,108.0}}) );
    EXPECT_EQ((AMRect)mv.buttonContainer.rect, (AMRect{AMPoint{100.000000,559.000000},AMSize{1000.000000,40.000000}}) );
    EXPECT_EQ((AMRect)mv.cancelBtn.rect, (AMRect{AMPoint{100.000000,559.000000},AMSize{75.000000,40.000000}}) );
    EXPECT_EQ((AMRect)mv.OKBtn.rect, (AMRect{AMPoint{1025.000000,559.000000},AMSize{75.000000,40.000000}}) );
}

class myview2: public AMWindow
{
public:
    AMView menubar;
    AMView fileBtn;
    AMView editBtn;
    AMView optionsBtn;
    AMView exitBtn;
    AMView content;
    AMView text1;
    AMView text2;
    AMView image;
    AMView buttonContainer;
    AMView cancelBtn;
    AMView OKBtn;

public:
    myview2();
};

myview2::myview2() :
    AMWindow(),
    menubar(this,"menubar"),
    fileBtn(&menubar,"fileBtn"),
    editBtn(&menubar,"editBtn"),
    optionsBtn(&menubar,"optionsBtn"),
    exitBtn(&menubar,"exitBtn"),
    content(this,"content"),
    text1(&content,"text1"),
    text2(&content,"text2"),
    image(&content,"image"),
    buttonContainer(this,"buttonContainer"),
    cancelBtn(&buttonContainer,"cancelBtn"),
    OKBtn(&buttonContainer,"OKBtn")
{
    rect = AMRect{AMPoint{10.0,20.0},AMSize{800.0,600.0}};

    menubar.rect->origin = [&]{
        return this->rect->origin();
        };
    menubar.rect->sz->width = [&]{return exitBtn.rect->sz->width();};
    menubar.rect->sz->height = 35.0;

    fileBtn.rect->origin = [&]{return menubar.rect->origin();};
    fileBtn.rect->sz->width = [&]{return menubar.rect->sz->width();};
    fileBtn.rect->sz->height = [&]{return menubar.rect->sz->height();};

    editBtn.rect->origin = [&]{return fileBtn.rect->origin() + AMPoint{100.0, 0.0};};
    editBtn.rect->sz->width = [&]{return fileBtn.rect->sz->width();};
    editBtn.rect->sz->height = [&]{return menubar.rect->sz->height();};

    optionsBtn.rect->origin = [&]{return editBtn.rect->origin() + AMPoint{100.0, 0.0};};
    optionsBtn.rect->sz->width = [&]{return editBtn.rect->sz->width();};
    optionsBtn.rect->sz->height = [&]{return menubar.rect->sz->height();};

    exitBtn.rect->origin = [&]{return optionsBtn.rect->origin() + AMPoint{100.0, 0.0};};
    exitBtn.rect->sz->width = [&]{return optionsBtn.rect->sz->width();};
    exitBtn.rect->sz->height = [&]{return menubar.rect->sz->height();};

    content.rect->origin->x = [&]{return this->rect->origin->x();};
    content.rect->sz->width = [&]{return this->rect->sz->width();};
    content.rect->sz->height = [&]{return this->rect->sz->height() - 35.0 - 40.0;};
    content.rect->origin->y = [&]{return this->rect->origin->y() + 35.0;};

    text1.rect->origin = [&]{return content.rect->origin();};
    text1.rect->sz->width = [&]{return content.rect->sz->width();};
    text1.rect->sz->height = [&]{return content.rect->sz->height() / 3.0;};

    text2.rect->origin->x = [&]{return content.rect->origin->x();};
    text2.rect->origin->y = [&]{return content.rect->origin->y() + content.rect->sz->height() / 3.0;};
    text2.rect->sz->width = [&]{return content.rect->sz->width();};
    text2.rect->sz->height = [&]{return content.rect->sz->height() / 3.0;};

    image.rect->origin->x = [&]{return content.rect->origin->x();};
    image.rect->origin->y = [&]{return content.rect->origin->y() + content.rect->sz->height() / 3.0 * 2.0;};
    image.rect->sz->width = [&]{return content.rect->sz->width();};
    image.rect->sz->height = [&]{return content.rect->sz->height() / 3.0;};


    buttonContainer.rect->origin->x = [&]{return this->rect->origin->x();};
    buttonContainer.rect->sz->width = [&]{return this->rect->sz->width();};
    buttonContainer.rect->sz->height = 40.0;
    buttonContainer.rect->origin->y = [&]{return this->rect->origin->y() + this->rect->sz->height() - 40.0;};

    cancelBtn.rect->origin = [&]{return buttonContainer.rect->origin();};
    cancelBtn.rect->sz->width = 75.0;
    cancelBtn.rect->sz->height = [&]{return buttonContainer.rect->sz->height();};

    OKBtn.rect->origin->x = [&]{return buttonContainer.rect->origin->x() + buttonContainer.rect->sz->width() - 75.0;};
    OKBtn.rect->origin->y = [&]{return buttonContainer.rect->origin->y();};
    OKBtn.rect->sz->width = 75.0;
    OKBtn.rect->sz->height = [&]{return buttonContainer.rect->sz->height();};
}

TEST(SmartProperties, BasicTest_loop) {

AMVariableNames::setEnabled(true);
    myview2 mv;

    mv.getPool().reset();

    AMRect rect;

    try {
        rect = (AMRect)mv.menubar.rect;
        FAIL() << "Looped AMroperty should throw exception on call";
    }
    catch(AMPropertyLoopException& e) {
        //printf("------------------------------------------------------------------------------------\n%s (%s --> %s)\n\n%s\n----------------------------------------------------------------------------------------------------------------\n",
        //e.what(), AMVariableNames::getVariableName(e.propertyEntered()).c_str(), AMVariableNames::getVariableName(e.propertyCaused()).c_str(),e.trace().c_str());

        std::string tracesreturned = e.trace();
        std::stringstream traceexpected;
        traceexpected<<AMVariableNames::getVariableName(&mv.menubar.rect)<<endl;
        traceexpected<<AMVariableNames::getVariableName(&mv.menubar.rect->sz)<<endl;
        traceexpected<<AMVariableNames::getVariableName(&mv.menubar.rect->sz->width)<<endl;
        traceexpected<<AMVariableNames::getVariableName(&mv.exitBtn.rect->sz->width)<<endl;
        traceexpected<<AMVariableNames::getVariableName(&mv.optionsBtn.rect->sz->width)<<endl;
        traceexpected<<AMVariableNames::getVariableName(&mv.editBtn.rect->sz->width)<<endl;
        traceexpected<<AMVariableNames::getVariableName(&mv.fileBtn.rect->sz->width)<<endl;
        traceexpected<<AMVariableNames::getVariableName(&mv.menubar.rect->sz->width)<<endl;
        traceexpected.flush();
        EXPECT_STREQ(tracesreturned.c_str(),  traceexpected.str().c_str());
    }
    catch(...) {
          FAIL() << "Looped AMroperty throws undefined exception";
    }
    try {
        rect = (AMRect)mv.fileBtn.rect;
        FAIL() << "Looped AMroperty should throw exception on call";
    }
    catch(AMPropertyLoopException& e) {
		//printf("------------------------------------------------------------------------------------\n%s (%s --> %s)\n\n%s\n----------------------------------------------------------------------------------------------------------------\n",
		//e.what(), AMVariableNames::getVariableName(e.propertyEntered()).c_str(), AMVariableNames::getVariableName(e.propertyCaused()).c_str(),e.trace().c_str());
    }
    catch(...) {
          FAIL() << "Looped AMroperty throws undefined exception";
    }
    try {
        rect = (AMRect)mv.editBtn.rect;
        FAIL() << "Looped AMroperty should throw exception on call";
    }
    catch(AMPropertyLoopException& e) {
		//printf("------------------------------------------------------------------------------------\n%s (%s --> %s)\n\n%s\n----------------------------------------------------------------------------------------------------------------\n",
		//e.what(), AMVariableNames::getVariableName(e.propertyEntered()).c_str(), AMVariableNames::getVariableName(e.propertyCaused()).c_str(),e.trace().c_str());
    }
    catch(...) {
          FAIL() << "Looped AMroperty throws undefined exception";
    }
    try {
        rect = (AMRect)mv.optionsBtn.rect;
        FAIL() << "Looped AMroperty should throw exception on call";
    }
    catch(AMPropertyLoopException& e) {
		//printf("------------------------------------------------------------------------------------\n%s (%s --> %s)\n\n%s\n----------------------------------------------------------------------------------------------------------------\n",
		//e.what(), AMVariableNames::getVariableName(e.propertyEntered()).c_str(), AMVariableNames::getVariableName(e.propertyCaused()).c_str(),e.trace().c_str());
    }
    catch(...) {
          FAIL() << "Looped AMroperty throws undefined exception";
    }
    try {
        rect = (AMRect)mv.exitBtn.rect;
        FAIL() << "Looped AMroperty should throw exception on call";
    }
    catch(AMPropertyLoopException& e) {
		//printf("------------------------------------------------------------------------------------\n%s (%s --> %s)\n\n%s\n----------------------------------------------------------------------------------------------------------------\n",
		//e.what(), AMVariableNames::getVariableName(e.propertyEntered()).c_str(), AMVariableNames::getVariableName(e.propertyCaused()).c_str(),e.trace().c_str());
    }
    catch(...) {
          FAIL() << "Looped AMroperty throws undefined exception";
    }

    EXPECT_EQ((AMRect)mv.content.rect, (AMRect{AMPoint{10.000000,55.000000},AMSize{800.000000,525.000000}}) );
    EXPECT_EQ((AMRect)mv.text1.rect, (AMRect{AMPoint{10.000000,55.000000},AMSize{800.000000,175.000000}}) );
    EXPECT_EQ((AMRect)mv.text2.rect, (AMRect{AMPoint{10.000000,230.000000},AMSize{800.000000,175.000000}}) );
    EXPECT_EQ((AMRect)mv.image.rect, (AMRect{AMPoint{10.000000,405.000000},AMSize{800.000000,175.000000}}) );
    EXPECT_EQ((AMRect)mv.buttonContainer.rect, (AMRect{AMPoint{10.000000,580.000000},AMSize{800.000000,40.000000}}) );
    EXPECT_EQ((AMRect)mv.cancelBtn.rect, (AMRect{AMPoint{10.000000,580.000000},AMSize{75.000000,40.000000}}) );
    EXPECT_EQ((AMRect)mv.OKBtn.rect, (AMRect{AMPoint{735.000000,580.000000},AMSize{75.000000,40.000000}}) );

}

class myview3: public AMWindow
{
public:
    AMView menubar;
	std::list<AMView*> subviews;
public:
    myview3();
};

myview3::myview3() :
    AMWindow(),
    menubar(this,"menubar")
{
    rect = AMRect{AMPoint{10.0,20.0},AMSize{800.0,600.0}};

	AMView* fileBtn = new AMView(&menubar,"fileBtn");
	subviews.push_back(fileBtn);
	AMView* editBtn = new AMView(&menubar,"editBtn");
	subviews.push_back(editBtn);
    AMView* optionsBtn = new AMView(&menubar,"optionsBtn");
	subviews.push_back(optionsBtn);
    AMView* exitBtn = new AMView(&menubar,"exitBtn");
	subviews.push_back(exitBtn);

    menubar.rect->origin = [&]{
        return rect->origin();
        };

    menubar.rect->sz->width = [=]{return exitBtn->rect->sz->width();};
    menubar.rect->sz->height = 35.0;

    fileBtn->rect->origin = [&]{return menubar.rect->origin();};
    fileBtn->rect->sz->width = [&]{return menubar.rect->sz->width();};
    fileBtn->rect->sz->height = [&]{return menubar.rect->sz->height();};

    editBtn->rect->origin = [=]{return fileBtn->rect->origin() + AMPoint{100.0, 0.0};};
    editBtn->rect->sz->width = [=]{return fileBtn->rect->sz->width();};
    editBtn->rect->sz->height = [&]{return menubar.rect->sz->height();};

    optionsBtn->rect->origin = [=]{return editBtn->rect->origin() + AMPoint{100.0, 0.0};};
    optionsBtn->rect->sz->width = [=]{return editBtn->rect->sz->width();};
    optionsBtn->rect->sz->height = [&]{return menubar.rect->sz->height();};

    exitBtn->rect->origin = [=]{return optionsBtn->rect->origin() + AMPoint{100.0, 0.0};};
    exitBtn->rect->sz->width = [=]{return optionsBtn->rect->sz->width();};
    exitBtn->rect->sz->height = [&]{return menubar.rect->sz->height();};


}





TEST(SmartProperties, BasicTest_delete) {

	AMVariableNames::setEnabled(true);

    myview3 mv;

    mv.getPool().reset();

    AMRect rect;

	auto it = mv.subviews.begin();
	std::advance(it,1);
	(*it)->rect.setPool(AMPropertyPool::defaultPool());

    try {
        rect = (AMRect)mv.menubar.rect;

        FAIL() << "Looped AMroperty should throw exception on call";
    }
    catch(AMPropertyDeletedException& e) {
        //printf("------------------------------------------------------------------------------------\n%s (%s --> %s)\n\n%s\n----------------------------------------------------------------------------------------------------------------\n",
		//e.what(), AMVariableNames::getVariableName(e.propertyEntered()).c_str(), AMVariableNames::getVariableName(e.propertyCaused()).c_str(),e.trace().c_str());

		std::string tracesreturned = e.trace();
		std::stringstream traceexpected;
		auto it = mv.subviews.begin();
		std::advance(it,1);
		traceexpected<<AMVariableNames::getVariableName(&(*it)->rect->sz->width)<<endl;
		traceexpected.flush();
		EXPECT_STREQ(tracesreturned.c_str(),  traceexpected.str().c_str());
    }
    catch(...) {
          FAIL() << "Looped AMroperty throws undefined exception";
    }

}


TEST(SmartProperties, EX1) {
	float k = 1.11;
	float l = 2.22;
	float m = 3.33;

	AMPropertyPool pool;
	AMProperty<float> prop1(pool, [&]{return k+l+m;});

	EXPECT_FLOAT_EQ((float)prop1, 6.66f);
	//cout << "k+l+m=" << prop1 << endl;

	l = 4.44;

	float fprop1 = prop1;

	EXPECT_FLOAT_EQ(fprop1, 6.66f);
	//cout << "k+l+m=" << prop1 << endl;
}


TEST(SmartProperties, EX2) {
	AMPropertyPool pool;
    AMProperty<float> propk(pool, 1.11);
	AMProperty<float> propl(pool, 2.22);
	AMProperty<float> propm(pool, 3.33);


	AMProperty<float> prop1(pool, [&]{return propk+propl+propm;});

	EXPECT_FLOAT_EQ((float)prop1, 6.66f);
	//cout << "k+l+m=" << prop1 << endl;

	propl = 4.44;

	EXPECT_FLOAT_EQ((float)prop1, 8.88f);
	//cout << "k+l+m=" << prop1 << endl;
}

class testVector {
public:
    AMProperty<float> propk;
    AMProperty<float> propl;
    AMProperty<float> propm;

    testVector(AMPropertyPool& pool):
        propk(pool, 1.11), propl(pool, 2.22), propm(pool, 3.33) {}
};

TEST(SmartProperties, EX2b) {
    AMPropertyPool pool;
    testVector v(pool);

    AMProperty<float> prop1(pool, [&]{return v.propk()+v.propl()+v.propm();});

    EXPECT_FLOAT_EQ((float)prop1, 6.66f);
    //cout << "k+l+m=" << prop1 << endl;

    v.propl = 4.0f;

    EXPECT_FLOAT_EQ((float)prop1, 8.44);
    //cout << "k+l+m=" << prop1 << endl;
}

TEST(SmartProperties, EX3) {
    AMPropertyPool pool;
    std::list<AMProperty<int> > myList;

    for(int i=0; i < 10; i++) {
        AMProperty<int> value(pool, i+1);
        myList.push_back(value);
    }

    AMProperty<int> sum(pool, [&]	{
        int sum = 0;
        for(std::list<AMProperty<int> >::iterator it = myList.begin(); it != myList.end(); it++) {
            sum += (int)*it;
        }
        return sum;
    });

    EXPECT_EQ(sum(), 55);
    //cout << "sum1 =" << sum() << endl;

    std::list<AMProperty<int> >::iterator it = myList.begin();
    std::advance(it, 4);
    *it = 25;

    EXPECT_EQ(sum(), 75);
    //cout << "sum2 =" << sum() << endl;
}

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
};
