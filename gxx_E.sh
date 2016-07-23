g++ -E $1 -I ./ext/gtest/googletest/googletest/include/ -I . > /tmp/gxx_E.cc
vim /tmp/gxx_E.cc
