#!/bin/bash


QKPROGRAM_DIR=/home/mribeiro/bitbucket/qkthings/embedded/qkprogram
QKPROGRAM_DOC_DIR=$QKPROGRAM_DIR/doc

RESOURCES_DIR=release/resources
EMBEDDED_DIR=$RESOURCES_DIR/embedded

rm -rf $EMBEDDED_DIR/qkprogram/doc
mkdir -p $EMBEDDED_DIR/qkprogram/doc
cp -r $QKPROGRAM_DOC_DIR $EMBEDDED_DIR/qkprogram/

rm -rf release/resources/html
mkdir -p release/resources/html
cp -r resources/html release/resources

rm -rf release/examples
mkdir -p release/examples
cp -r examples/ release/
