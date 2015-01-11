#include <clang-c/Index.h>

#include <assert.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct FileLocation {
    string path;
    unsigned line;
    unsigned column;
    unsigned offset;

    friend ostream &operator<<(ostream &out, const FileLocation &floc) {
        return out << floc.path << ":" << floc.line << ":" << floc.column << "[@" << floc.offset << "]";
    }
};

string str(const CXString &cxs) {
    const char *cs = clang_getCString(cxs);
    string cpps(cs);
    clang_disposeString(cxs);
    return cpps;
}

string str(const CXCursor &cursor) {
    return str(clang_getCursorSpelling(cursor));
}

string str(const CXCursorKind &kind) {
    return str(clang_getCursorKindSpelling(kind));
}

string str(const CXFile &file) {
    return str(clang_getFileName(file));
}

string spelling(const CXType &type) {
    return str(clang_getTypeSpelling(type));
}

string spelling(const CXCursor &cursor) {
    return str(clang_getCursorSpelling(cursor));
}

string spelling(CXTranslationUnit tu,
                CXToken token) {
    return str(clang_getTokenSpelling(tu, token));
}

CXCursorKind kind(CXCursor cursor) {
    return clang_getCursorKind(cursor);
}

CXType type(CXCursor cursor) {
    return clang_getCursorType(cursor);
}

CXCursor semantic_parent(CXCursor cursor) {
    return clang_getCursorSemanticParent(cursor);
}

CXSourceRange get_extent(CXCursor cursor) {
    return clang_getCursorExtent(cursor);
}
CXSourceRange get_extent(CXTranslationUnit tu, CXToken token) {
    return clang_getTokenExtent(tu, token);
}

CXSourceLocation start(CXSourceRange range) {
    return clang_getRangeStart(range);
}
CXSourceLocation start(CXCursor cursor) {
    return start(get_extent(cursor));
}

CXSourceLocation end(CXSourceRange range) {
    return clang_getRangeEnd(range);
}
CXSourceLocation end(CXCursor cursor) {
    return end(get_extent(cursor));
}

FileLocation file_location(const CXSourceLocation &location) {
    FileLocation result;
    CXFile file;

    clang_getFileLocation(location,
                          &file,
                          &result.line,
                          &result.column,
                          &result.offset);
    result.path = str(file);
    return result;
}
unsigned int file_offset(CXSourceLocation location) {
    unsigned int result;
    clang_getFileLocation(location,
                          nullptr,
                          nullptr,
                          nullptr,
                          &result);
    return result;
}

ostream &operator<<(ostream &out, const CXCursor &c) {return out << str(c);}
ostream &operator<<(ostream &out, const CXType &c) {return out << spelling(c);}
ostream &operator<<(ostream &out, const CXCursorKind &k) {return out << str(k);}
ostream &operator<<(ostream &out, const CXSourceLocation &l) {return out << file_location(l);}
ostream &operator<<(ostream &out, const CXSourceRange &r) {return out << start(r) << " -> " << end(r);}

#define verbose(msg)
//#define verbose(msg) cout << msg << endl
#define err(msg) {cerr << msg << endl; exit(1);}

void visit(CXCursor cursor, 
           function<CXChildVisitResult (CXCursor, CXCursor)> visitor) {
    struct local {
        struct parms_t {
            function<CXChildVisitResult (CXCursor, CXCursor)> visitor;
        };
        static CXChildVisitResult visit(CXCursor cursor,
                                        CXCursor parent,
                                        CXClientData client_data) {
            parms_t *parms = (parms_t *)client_data;
            return parms->visitor(cursor, parent);
        }
    };
    local::parms_t parms = {visitor};
    clang_visitChildren(cursor, local::visit, &parms);
}
void visit(CXCursor cursor, 
           function<CXChildVisitResult (CXCursor)> visitor) {
    visit(cursor,
          [visitor](CXCursor cursor, CXCursor parent) {
              return visitor(cursor);
          });
}

vector<CXCursor> find(CXCursor cursor,
                      function<bool (CXCursor)> predicate,
                      bool recursive) {
    vector<CXCursor> results;
    visit(cursor,
          [predicate, &results](CXCursor cursor) {
              if(predicate(cursor)) {
                  results.push_back(cursor);
              }
              return CXChildVisit_Recurse;
          });
    return results;
}

vector<CXCursor> get_children(CXCursor cursor,
                              function<bool (CXCursor)> predicate = [](CXCursor){return true;}) {
    return find(cursor, predicate, false);
}

vector<CXCursor> get_children(CXCursor cursor,
                              CXCursorKind kind) {
    return get_children(cursor, [kind](CXCursor cursor) {return cursor.kind == kind;});
}

CXCursor get_child(CXCursor cursor, function<bool (CXCursor)> predicate) {
    vector<CXCursor> children = get_children(cursor, predicate);
    assert(children.size() == 1);
    return children.front();
}

CXCursor get_child(CXCursor cursor, CXCursorKind kind) {
    return get_child(cursor, [kind](CXCursor cursor) {return cursor.kind == kind;});
}

bool has_child(CXCursor cursor,
               function<bool (CXCursor)> predicate) {
    bool result = false;
    visit(cursor,
          [predicate, &result](CXCursor cursor) {
              if(predicate(cursor)) {
                  result = true;
                  return CXChildVisit_Break;
              } else {
                  return CXChildVisit_Continue;
              }
          });
    return result;
}

bool has_child(CXCursor cursor, CXCursorKind kind) {
    return has_child(cursor, [kind](CXCursor child) {return child.kind == kind;});
}

bool is_struct(CXCursor cursor) {
    return cursor.kind == CXCursor_StructDecl;
}

bool is_method(CXCursor cursor) {
    return kind(cursor) == CXCursor_CXXMethod;
}

bool has_annotate(CXCursor cursor, string _spelling) {
    return has_child(cursor, [&_spelling](CXCursor child) {
            return (child.kind == CXCursor_AnnotateAttr)
                && (spelling(child) == _spelling);
        });
}

bool has_body(CXCursor cursor) {
    assert(is_method(cursor));
    return has_child(cursor, CXCursor_CompoundStmt);
}

bool is_clunion(CXCursor cursor) {
    return is_struct(cursor) && has_annotate(cursor, "clunion");
}

vector<CXCursor> find_clunions(CXCursor cursor) {
    return find(cursor,
                [](CXCursor cursor) {return is_clunion(cursor);},
                true);
}

vector<CXCursor> get_methods(CXCursor cursor) {
    return get_children(cursor, [](CXCursor cursor) {return is_method(cursor);});
}

vector<CXCursor> get_bases(CXCursor cursor) {
    return get_children(cursor, CXCursor_CXXBaseSpecifier);
}

CXType get_return_type(CXCursor cursor) {
    return clang_getCursorResultType(cursor);
}

vector<CXCursor> get_arguments(CXCursor cursor) {
    int num_arguments = clang_Cursor_getNumArguments(cursor);
    assert(num_arguments > -1);

    vector<CXCursor> result;
    for(int i = 0; i < num_arguments; i++) {
        result.push_back(clang_Cursor_getArgument(cursor, i));
    }
    return result;
}

vector<CXType> get_argument_types(CXCursor cursor) {
    CXType method_type = type(cursor);
    int num_arguments = clang_getNumArgTypes(method_type);
    assert(num_arguments > -1);


    vector<CXType> result;
    for(int i = 0; i < num_arguments; i++) {
        result.push_back(clang_getArgType(method_type, i));
    }
    return result;
}

bool is_virtual(CXCursor cursor) {
    return clang_CXXMethod_isVirtual(cursor);
}

bool is_variadic(CXCursor cursor) {
    return clang_isFunctionTypeVariadic(type(cursor));
}

struct SourceExtractor {
    static map<string, unique_ptr<char>> file_buffer_cache;

    static char *get_file_buffer(const string &path) {
        auto it = file_buffer_cache.find(path);
        if(it != file_buffer_cache.end())
            return it->second.get();

        verbose("Reading " << path);
        
        FILE *f = fopen(path.c_str(), "r");
        if(!f) {
            err("Failed opening " << path << " for reading");
        }

        int flen;
        {
            if( (0 != fseek(f, 0, SEEK_END))
                || (0 > (flen = ftell(f))) ) {
                err("Failed finding size of " << path);
            }
        }
        
        char *buf = (char *)malloc(flen + 1);
        if( (0 != fseek(f, 0, SEEK_SET))
            || (flen != fread(buf, 1, flen, f)) ) {
            err("Failed reading from " << path);
        }
        fclose(f);
        
        buf[flen] = '\0';
        file_buffer_cache[path] = unique_ptr<char>(buf);

        return buf;
    }

    static string extract(CXSourceLocation start,
                          CXSourceLocation end) {
        FileLocation floc = file_location(start);
        assert(floc.path == file_location(end).path);

        unsigned int start_offset = file_offset(start);
        unsigned int end_offset = file_offset(end);
        unsigned int len = end_offset - start_offset;

        string result;
        result.resize(len);
        char *rbuf = const_cast<char *>(result.data());

        char *fbuf = get_file_buffer(floc.path);
        memcpy(rbuf, fbuf + start_offset, len);

        return result;
    }

    static string extract(CXTranslationUnit tu,
                          CXSourceRange range) {
        return extract(start(range), end(range));
    }

    static string extract(CXTranslationUnit tu,
                          CXCursor cursor) {
        return extract(tu, get_extent(cursor));
    }

    static string extract_method_body(CXTranslationUnit tu,
                                      CXCursor method) {
        return extract(tu, get_child(method, CXCursor_CompoundStmt));
    }
};
map<string, unique_ptr<char>> SourceExtractor::file_buffer_cache;

struct SourceGenerator {
    static ostream &parameters(ostream &out, CXCursor method) {
        out << "(";
        vector<CXType> arg_types = get_argument_types(method);
        vector<CXCursor> arg_names = get_arguments(method);
        for(size_t i = 0; i < arg_types.size(); i++) {
            if(i != 0)
                out << ", ";
            out << spelling(arg_types[i]) << " ";
            out << spelling(arg_names[i]);
        }
        out << ")";
        return out;
    }

    static ostream &signature(ostream &out, CXCursor method) {
        out << spelling(get_return_type(method)) << " "
            << spelling(method);
        return parameters(out, method);
    }

    static ostream &signature_virtual_method_impl(ostream &out, CXCursor method) {
        out << spelling(get_return_type(method)) << " "
            << "__clunion_virtual_" << spelling(method);
        return parameters(out, method);
    }

    static ostream &call_virtual_method_dispatch(ostream &out, CXCursor method) {
        if(spelling(get_return_type(method)) != "void") {
            out << "return ";
        }
        out << "dispatch::" << spelling(method) << "(this";
        for(CXCursor arg: get_arguments(method)) {
            out << ", " << spelling(arg);
        }
        out << ")";
        return out;
    }

    static ostream &virtual_method_accessor_decl(ostream &out, CXCursor method) {
        assert(is_method(method));
        assert(is_virtual(method));
        if(is_variadic(method)) {
            err(start(method) << ": Variadic virtual functions not supported.");
        }

        return signature(out, method);
    }
};

struct Clunion {
    static map<string, shared_ptr<Clunion>> all;

    CXCursor cursor;
    string name;
    shared_ptr<Clunion> base;
    
    Clunion(CXCursor cursor_)
    : cursor(cursor_)
    , name(spelling(type(cursor))) {
    }

    static void declare(CXCursor c) {
        auto ptr = make_shared<Clunion>(c);
        assert(all.find(ptr->name) == all.end());
        all[ptr->name] = ptr;
    }

    static shared_ptr<Clunion> get(string s) {
        return all[s];
    }
    static shared_ptr<Clunion> get(CXType t) {
        return get(spelling(t));
    }
    static shared_ptr<Clunion> get(CXCursor c) {
        return get(type(c));
    }
    
    static void resolve_hierarchy() {
        for(auto &kv: all) {
            shared_ptr<Clunion> ptr = kv.second;
            vector<CXCursor> bases = get_bases(ptr->cursor);
            if(bases.size() > 0) {
                if(bases.size() > 1) {
                    err(start(ptr->cursor) << ": Multiple inheritance not supported.");
                }
                ptr->base = get(bases.front());
                cout << ptr->name << " extends " << ptr->base->name << endl;
            }
        }
    }
};
map<string, shared_ptr<Clunion>> Clunion::all;

int main() {
    CXIndex index = clang_createIndex(1, 1);

    char *args[] = {};
    int nargs = sizeof(args) / sizeof(char*);
    CXTranslationUnit tu = clang_createTranslationUnitFromSourceFile(index,
                                                                     "test/hello.cpp",
                                                                     nargs, args,
                                                                     0, nullptr);
    CXCursor cursor = clang_getTranslationUnitCursor(tu);

    for(auto &c: find_clunions(cursor)) {
        Clunion::declare(c);
/*
        cout << "  START = " << file_location(start(c)) << endl;
        cout << "  END = " << file_location(end(c)) << endl;
*/
        for(auto &m: get_methods(c)) {
/*
            SourceGenerator::signature(cout, m) << endl;
            SourceGenerator::call_virtual_method_dispatch(cout, m) << endl;
            SourceGenerator::signature_virtual_method_impl(cout, m) << endl;
*/
            //cout << "'" << SourceExtractor::extract(tu, m) << "'" << endl;
            //cout << "'" << SourceExtractor::extract_method_body(tu, m) << "'" << endl;
/*
            if(is_virtual(m)) {
                cout << spelling(m) << ": accessor_decl=" << SourceGenerator::virtual_method_accessor_decl(m) << endl;
            }
            cout << "  method " << spelling(m) << " is_virtual=" << is_virtual(m) << " has_body=" << has_body(m) << endl;
            cout << "    type spelling = " << spelling(type(m)) << endl;
            cout << "    arguments:" << endl;
            for(auto &a: get_arguments(m)) {
                cout << "      spelling=" << spelling(a) << ", location=" << start(a) << " -> " << end(a) << endl;
            }
            cout << "    START = " << file_location(start(m)) << endl;
            cout << "    END = " << file_location(end(m)) << endl;
*/
        }
    }
    Clunion::resolve_hierarchy();
    
    clang_disposeTranslationUnit(tu);
    
    cout << "COMPLETE" << endl;
}
