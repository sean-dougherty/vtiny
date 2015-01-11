#!/usr/bin/python
# vim: set fileencoding=utf-8

# taken from https://github.com/sztomi/code-generator/blob/master/src/dump_ast.py

import clang.cindex
from clang.cindex import CursorKind
import sys

"""
if len(sys.argv) != 2:
	print("Usage: parse.py [header file name]")
	sys.exit()
"""
path = "test.cpp"

clang.cindex.Config.set_library_file('/usr/lib/llvm-3.4/lib/libclang.so.1')
index = clang.cindex.Index.create()
translation_unit = index.parse(path, ['-x', 'c++', '-std=c++11', '-Dclunion=struct __attribute__((annotate("clunion")))'])

def has_child(node, predicate):
	for c in node.get_children():
		if predicate(c):
			return True
	return False

def find_children(node, predicate):
	result = []
	for c in node.get_children():
		if predicate(c):
			result.append(c)
	return result

def find_clunions(node, result = []):
	if node.kind == clang.cindex.CursorKind.STRUCT_DECL:
		if has_child(node, lambda x: x.kind == CursorKind.ANNOTATE_ATTR and x.displayname == 'clunion'):
			result.append(node)
	for c in node.get_children():
		find_clunions(c, result)
	return result

clunions = find_clunions(translation_unit.cursor)
print "found %d clunions" % len(clunions)

for clunion in clunions:
	methods = find_children(clunion, lambda x: x.kind == CursorKind.CXX_METHOD)
	for m in methods:
		print dir(m)
							
