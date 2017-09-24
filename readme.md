# for study

# ubuntu build
sudo apt-get install cmake libgtk2.0-dev libssl-dev libgconf2-dev
./build.sh

# windows build
build.bat

# centos build
yum groupinstall "Development Tools"
yum install gtk+-devel nss-util-devel GConf2-devel

wget https://cmake.org/files/v3.8/cmake-3.8.2-Linux-x86_64.tar.gz
tar -xf cmake-3.8.2-Linux-x86_64.tar.gz
cp cmake-3.8.2-Linux-x86_64/* /usr -a
