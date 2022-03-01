#!/bin/sh

if [ -z $(which clang-format) ] ; then
  echo "clang-format not found."
  echo "Install Homebrew, and run:"
  echo "brew install clang-format"
  echo ""
  echo "Aborting."
  exit
fi

pushd $(git rev-parse --show-toplevel);
find source/ -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i -style=Google;
pwd;
