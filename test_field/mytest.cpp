#include "/usr/local/include/cuckoofilter/cuckoofilter.h"
#include <typeinfo>
#include <vector>
#include <iostream>

using namespace std;
using cuckoofilter::CuckooFilter;

int main(int argc, char **argv){

    size_t total_items = 10;
    CuckooFilter<size_t, 12> filter1(total_items);
    CuckooFilter<size_t, 12> filter2(total_items);
    
    filter1.Add(1);
    filter1.Add(2);
    filter1.Add(3);
    
    filter2.Add(1);
    filter2.Add(2);
    filter2.Add(3);

    cout<< filter1.Contain(1) << endl;
    cout<< filter2.Contain(1) << endl;
    cout<< filter2.Contain(4) << endl;
    
    cout<< "------use vector------" << endl;
    // 这里要保存指针，而不是对象！
    //    vector在pushback时，会调用constructor和desctructor
    //    但是这里出错，估计作者没写好
    // 这里保存指针，效率也更高
    vector< CuckooFilter<size_t, 12>* > filters;

    filters.push_back(&filter1);
    filters.push_back(&filter2);
    
    cout<< filters[0]->Contain(4) << endl;

    cout<< "------use loop------" << endl;
    // 这里如果循环里添加，需要new一个对象，返回其指针
    //    在c++里，new的对象分配在heap，不会自动删除
    //    如果用完，需要手动delete，不然会内存泄漏
    for(int i = 0; i < 3; i++){
        CuckooFilter<size_t,12>* tmp = new CuckooFilter<size_t, 12>(total_items);
        filters.push_back(tmp);
    }
    cout << filters.size() << endl;

    filters.resize(9);

    cout << filters.size() << endl;

    cout<< filters[4]->Contain(4) << endl;
    cout<< filters[4]->Add(4) << endl;
    cout<< filters[4]->Contain(4) << endl;

    // 这里是能释放new出来的，不能释放其他的
    // 其他的会自动回收
    for(int i = 2; i < 5; i++){
        delete filters[i];
    }

    cout << "------memory free------" << endl;
    cout << filters.size() << endl;
}
