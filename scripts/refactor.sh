#! /bin/sh

cpp="s/ui\/$1.hpp/ui\/$1\/$1.hpp/" 

echo $cpp

sed -i "${cpp}" **/*.cpp
sed -i "${cpp}" **/*.hpp
