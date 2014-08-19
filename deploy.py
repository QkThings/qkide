#!/usr/bin/python

from os import getcwd, chdir, path
from distutils.dir_util import copy_tree, remove_tree
from subprocess import call
import argparse

def cp(root_src,root_dest,rel_path):
	print "Copying %s from %s to %s" % (rel_path, root_src, root_dest)
	copy_tree(path.join(root_src, rel_path), path.join(root_dest, rel_path))
	

def deploy():

	parser = argparse.ArgumentParser()

	parser.add_argument("-A", "--all", action="store_true", help='deploy everything from scratch')
	args = parser.parse_args()

	rootdir = getcwd()
	
	deploy_embedded = False
	cp_toolchain = False

	# Paths
	QMAKE_EXE = "/opt/Qt5.1.1/5.1.1/gcc/bin/qmake"

	if args.all:
		if path.exists("release"):
			remove_tree("release")
		deploy_embedded = True
		cp_toolchain = False

		print " === Build qkide"
		call([QMAKE_EXE])
		call(["make"])



	SOFTWARE_DIR = path.join(rootdir, "../")
	SOFTWARE_SHARED_DIR = path.join(SOFTWARE_DIR, "shared")
	EMBEDDED_DIR  = path.join(rootdir, "../../embedded")
	QKPROGRAM_DIR = path.join(EMBEDDED_DIR, "qkprogram")
	QKPERIPHERAL_DIR = path.join(EMBEDDED_DIR, "qkperipheral")
	QKDSP_DIR = path.join(EMBEDDED_DIR, "qkdsp")
	EMBEDDED_SHARED_DIR = path.join(EMBEDDED_DIR, "shared")

	TOOLS_DIR = path.join(SOFTWARE_SHARED_DIR, "linux/tools")
	CTAGS_DIR = path.join(TOOLS_DIR, "ctags")

	RELEASE_DIR = path.join(rootdir, "release")
	RELEASE_RESOURCES_DIR = path.join(RELEASE_DIR, "resources")
	RELEASE_TOOLS_DIR = path.join(RELEASE_RESOURCES_DIR, "tools")
	RELEASE_EMBEDDED_DIR  = path.join(RELEASE_RESOURCES_DIR, "embedded")
	RELEASE_QKPROGRAM_DIR = path.join(RELEASE_EMBEDDED_DIR, "qkprogram")
	RELEASE_QKPERIPHERAL_DIR = path.join(RELEASE_EMBEDDED_DIR, "qkperipheral") 
	RELEASE_QKDSP_DIR = path.join(RELEASE_EMBEDDED_DIR, "qkdsp") 

	# Build dependencies
	libs = ["qkcore", "qkwidget"]
	for lib in libs:
		print " === Build %s" % (lib)
		chdir(path.join(SOFTWARE_DIR, lib))
		call(QMAKE_EXE)
		call("make")
		chdir(rootdir)

	if cp_toolchain:
		print " === Copy embedded toolchain"
		cp(EMBEDDED_SHARED_DIR, RELEASE_EMBEDDED_DIR, "toolchain/common")
		cp(EMBEDDED_SHARED_DIR, RELEASE_EMBEDDED_DIR, "toolchain/linux")

	if deploy_embedded:
		chdir(EMBEDDED_DIR)
		call(["python", "deploy.py"])
		chdir(rootdir)

	# Copy files
	print " === Copy embedded"
	cp(EMBEDDED_DIR,  RELEASE_EMBEDDED_DIR,  "target")


	cp(QKPROGRAM_DIR, RELEASE_QKPROGRAM_DIR, "doc")
	cp(QKPROGRAM_DIR, RELEASE_QKPROGRAM_DIR, "include")
	cp(QKPROGRAM_DIR, RELEASE_QKPROGRAM_DIR, "build/lib")

	cp(QKPERIPHERAL_DIR, RELEASE_QKPERIPHERAL_DIR, "doc")
	cp(QKPERIPHERAL_DIR, RELEASE_QKPERIPHERAL_DIR, "include")
	cp(QKPERIPHERAL_DIR, RELEASE_QKPERIPHERAL_DIR, "build/lib")

	cp(QKDSP_DIR, RELEASE_QKDSP_DIR, "doc")
	cp(QKDSP_DIR, RELEASE_QKDSP_DIR, "include")
	cp(QKDSP_DIR, RELEASE_QKDSP_DIR, "build/lib")

	print " === Copy other resources"
	cp(TOOLS_DIR, RELEASE_TOOLS_DIR, "ctags")
	cp("resources", RELEASE_RESOURCES_DIR, "html")
	cp("resources", RELEASE_RESOURCES_DIR, "theme")
	cp("", RELEASE_DIR, "examples")
#	cp(SOFTWARE_SHARED_DIR, RELEASE_RESOURCES_DIR, "info")

	print "Done"


if __name__ == "__main__":
	deploy()

