#!/bin/sh

cd plustache

out=include/plustache.hxx

cat include/plustache/plustache_types.hpp | sed 's/^#include <plustache.*>//g' > $out
cat include/plustache/context.hpp | sed 's/^#include <plustache.*>//g' >> $out
cat include/plustache/template.hpp | sed 's/^#include <plustache.*>//g' >> $out
cat src/context.cpp | sed 's/^#include <plustache.*>//g' >> $out
cat src/template.cpp | sed 's/^#include <plustache.*>//g' >> $out

cd ..



