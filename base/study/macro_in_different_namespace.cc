/* Copyright 2016 kunming.xie
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * 宏在预处理被替换，namespace在编译器发生作用
 * 因此t0, t1里面的MAX宏定义，只有最后一个宏定义起作用
 */

#include <iostream>

/////////////

namespace t0 {
#define MAX(m, n) (m)
};

 
namespace t1 {
#define MAX(m, n) (n)   //warning: "MAX" redefined
};

void test()
{
    // 
    // t1::(0);  // #define MAX(m, n) (n)
    //
    t1::MAX(1, 0);
}

using namespace t0;
int main(int argc, char **argv)
{
    // last def of MAX
    // #define MAX(m, n) (n)
    MAX(1, 0);
    return 0;
}
