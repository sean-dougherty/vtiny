#!/usr/bin/python
# vim: set fileencoding=utf-8

# taken from https://github.com/sztomi/code-generator/blob/master/src/dump_ast.py

import clang.cindex
import asciitree
import sys

def node_children(node):
	#return (c for c in node.get_children() if c.location.file and c.location.file.name == sys.argv[1])
	return node.get_children()

def print_node(node):
	text = node.spelling or node.displayname
	kind = str(node.kind)[str(node.kind).index('.')+1:]
	return '{} {}'.format(kind, text)

if len(sys.argv) != 2:
	print("Usage: dump_ast.py [header file name]")
	sys.exit()

path = sys.argv[1]

clang.cindex.Config.set_library_file('/usr/lib/llvm-3.4/lib/libclang.so.1')
index = clang.cindex.Index.create()
translation_unit = index.parse(path, ['-x', 'c++', '-std=c++11', '-Dclunion=struct __attribute__((annotate("clunion")))'])

print(asciitree.draw_tree(translation_unit.cursor, node_children, print_node))

