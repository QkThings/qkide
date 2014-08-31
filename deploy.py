#!/usr/bin/python

from os import getcwd, chdir, path
from distutils.dir_util import copy_tree, remove_tree
from subprocess import call
import argparse

QMAKE_EXE = "/opt/Qt5.1.1/5.1.1/gcc/bin/qmake"

def cp(root_src,root_dest,rel_path):
	print "Copying %s from %s to %s" % (rel_path, root_src, root_dest)
	copy_tree(path.join(root_src, rel_path), path.join(root_dest, rel_path))	

def deploy():

	parser = argparse.ArgumentParser()
	parser.add_argument("--emb", action="store_true", help='deploy embedded')
	parser.add_argument("--toolchain", action="store_true", help='copy toolchain')
	parser.add_argument("--clean", action="store_true", help='clean')
	args = parser.parse_args()

	rootdir = getcwd()
	
	deploy_embedded = args.emb
	copy_toolchain = args.toolchain

	call([QMAKE_EXE])

	if args.clean:
		print "! Cleaning"
		if path.exists("release"):
			remove_tree("release")
			call(["make","clean"])
		print "! Building QkIDE"
		call(["make"])

	SOFTWARE_DIR = path.join(rootdir, "../")
	EMB_DIR  = path.join(rootdir, "../../embedded")
	EMB_SHARED_DIR = path.join(EMB_DIR, "shared")

	RC_DIR = path.join(rootdir, "resources")
	TOOLS_DIR = path.join(rootdir, "../tools/linux")

	RELEASE_DIR = path.join(rootdir, "release")
	RELEASE_RC_DIR = path.join(RELEASE_DIR, "resources")
	RELEASE_TOOLS_DIR = path.join(RELEASE_RC_DIR, "tools")
	RELEASE_EMB_DIR  = path.join(RELEASE_RC_DIR, "embedded")

	libs = ["qkcore", "qkwidget"]
	for lib in libs:
		print "! Build %s" % (lib)
		chdir(path.join(SOFTWARE_DIR, lib))
		call(QMAKE_EXE)
#		if args.clean:
#			call(["make","clean"])
		call("make")
		chdir(rootdir)

	if copy_toolchain:
		print "! Copy toolchain"
		cp(EMB_SHARED_DIR, RELEASE_EMB_DIR, "toolchain/common")
		cp(EMB_SHARED_DIR, RELEASE_EMB_DIR, "toolchain/linux")

	if deploy_embedded:
		print "! Deploy embedded"
		chdir(EMB_DIR)
		call(["python", "deploy.py"])
		chdir(rootdir)
		print "! Copy embedded"
		cp(EMB_DIR,  RELEASE_EMB_DIR,  "target")
		emb_libs = ["qkprogram", "qkperipheral", "qkdsp"]
		for emb_lib in emb_libs:
			cp(path.join(EMB_DIR, emb_lib), path.join(RELEASE_EMB_DIR, emb_lib), "doc")
			cp(path.join(EMB_DIR, emb_lib), path.join(RELEASE_EMB_DIR, emb_lib), "include")
			cp(path.join(EMB_DIR, emb_lib), path.join(RELEASE_EMB_DIR, emb_lib), "build/lib")


	print "! Copy other resources"
	cp(".", RELEASE_DIR, "examples")
	cp(RC_DIR, RELEASE_RC_DIR, "html")
	cp(RC_DIR, RELEASE_RC_DIR, "theme")
	cp(TOOLS_DIR, RELEASE_TOOLS_DIR, "ctags")



if __name__ == "__main__":
	deploy()

