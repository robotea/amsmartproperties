/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   AMUICommon.h
 * Author: zdenek
 *
 * Created on 3. září 2019, 17:25
 */

#ifndef AMUICOMMON_H
#define AMUICOMMON_H


#include <stdio.h>

/*
#ifdef AMProperty_log
#ifndef DEBUG
#define AMProperty_log(...) printf(__VA_ARGS__)
#else
#define AMProperty_log(...)
#endif
#endif


*/

//#include <functional>
#include <vector>

//template<typename T>
//using AMPropertyFunctionType = std::function<T()>;

template<typename T>
using AMPropertyContainerType = std::vector<T>;

#endif
