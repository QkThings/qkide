#!/usr/bin/python

from os import getcwd, chdir, path
from distutils.dir_util import copy_tree
from subprocess import call

def cp(root_src,root_dest,rel_path):
	copy_tree(path.join(root_src, rel_path), path.join(root_dest, rel_path))
	

def deploy():
	rootdir = getcwd()
	
	deploy_embedded = False
	cp_toolchain = False

	# Paths
	QMAKE_EXE = "/opt/Qt5.1.1/5.1.1/gcc/bin/qmake"

	SOFTWARE_DIR = path.join(rootdir, "../")
	EMBEDDED_DIR  = path.join(rootdir, "../../embedded")
	QKPROGRAM_DIR = path.join(EMBEDDED_DIR, "qkprogram")
	EMBEDDED_SHARED_DIR = path.join(EMBEDDED_DIR, "shared")

	TOOLS_DIR = path.join(rootdir, "../tools/linux")
	CTAGS_DIR = path.join(TOOLS_DIR, "ctags")

	RELEASE_DIR = path.join(rootdir, "release")
	RELEASE_RESOURCES_DIR = path.join(RELEASE_DIR, "resources")
	RELEASE_TOOLS_DIR = path.join(RELEASE_RESOURCES_DIR, "tools")
	RELEASE_EMBEDDED_DIR  = path.join(RELEASE_RESOURCES_DIR, "embedded")
	RELEASE_QKPROGRAM_DIR = path.join(RELEASE_EMBEDDED_DIR, "qkprogram")

	# Build dependencies
	libs = ["qkcore", "qkconnect", "qkwidget"]
	for lib in libs:
		chdir(path.join(SOFTWARE_DIR, lib))
		print "Building", lib
		call(QMAKE_EXE)
		call("make")
		chdir(rootdir)

	# Deploy embedded libraries

	if deploy_embedded:
		chdir(QKPROGRAM_DIR)
		print "Deploying embedded"
		call(["python", path.join(QKPROGRAM_DIR, "deploy.py")])
		chdir(rootdir)

	# Copy files
	print "Copying files"

	cp(QKPROGRAM_DIR, RELEASE_QKPROGRAM_DIR, "doc")
	cp(QKPROGRAM_DIR, RELEASE_QKPROGRAM_DIR, "include")
	cp(QKPROGRAM_DIR, RELEASE_QKPROGRAM_DIR, "build/lib")
	cp(QKPROGRAM_DIR, RELEASE_QKPROGRAM_DIR, "build/target")

	cp(TOOLS_DIR, RELEASE_TOOLS_DIR, "ctags")
	cp("resources", RELEASE_RESOURCES_DIR, "html")
	cp("", RELEASE_DIR, "examples")

	if cp_toolchain:
		print "Copying toolchain"
		cp(EMBEDDED_SHARED_DIR, RELEASE_EMBEDDED_DIR, "toolchain/common")
		cp(EMBEDDED_SHARED_DIR, RELEASE_EMBEDDED_DIR, "toolchain/linux")

	print "Done"


if __name__ == "__main__":
	deploy();
