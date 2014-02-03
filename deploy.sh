#!/bin/bash

rm -rf release/resources/html
mkdir -p release/resources/html
cp -r resources/html release/resources

rm -rf release/examples
mkdir -p release/examples
cp -r examples/ release/
