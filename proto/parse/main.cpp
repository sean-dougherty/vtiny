//todo: determine if method is const!
//todo: properly find end of clunion decl.
//todo: indent clunions
//todo: remove trailing ; for pure virtual

#include <clang-c/Index.h>

#include <assert.h>

#include <algorithm>
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
ostream &operator<<(ostream &out, const ostream &) {return out;}

void reset(stringstream &ss) {
    ss.str("");
    ss.clear();
}

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

bool is_pure_virtual(CXCursor cursor) {
    return clang_CXXMethod_isPureVirtual(cursor);
}

bool is_virtual(CXCursor cursor) {
    return clang_CXXMethod_isVirtual(cursor);
}

bool is_variadic(CXCursor cursor) {
    return clang_isFunctionTypeVariadic(type(cursor));
}

bool is_const(CXCursor cursor) {
    return clang_isConstQualifiedType(type(cursor));
}

struct Method {
    CXCursor cursor;
    string signature;

    Method() {
    }
    Method(CXCursor cursor_);

    bool is_virtual() {
        return ::is_virtual(cursor);
    }
    bool is_pure_virtual() {
        return ::is_pure_virtual(cursor);
    }

    friend ostream &operator<<(ostream &out, const Method &m) {
        return out << m.signature;
    }
};

struct Clunion {
    typedef shared_ptr<Clunion> ptr;
    static map<string, shared_ptr<Clunion>> all;

    CXCursor cursor;
    string name;
    shared_ptr<Clunion> base;
    vector<shared_ptr<Clunion>> derivations;
    map<string, Method> methods;
    
    Clunion(CXCursor cursor_)
    : cursor(cursor_)
    , name(spelling(type(cursor))) {

        for(auto &c: get_methods(cursor)) {
            Method m{c};
            methods[m.signature] = m;
        }
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
    
    static void resolve_bases() {
        for(auto &kv: all) {
            shared_ptr<Clunion> ptr = kv.second;
            vector<CXCursor> bases = get_bases(ptr->cursor);
            if(bases.size() > 0) {
                if(bases.size() > 1) {
                    err(start(ptr->cursor) << ": Multiple inheritance not supported.");
                }
                ptr->base = get(bases.front());
                ptr->base->derivations.push_back(ptr);
            }
        }
    }

    friend ostream &operator<<(ostream &out, Clunion &c) {
        return out << c.name;
    }
    friend ostream &operator<<(ostream &out, shared_ptr<Clunion> &c) {
        return out << *c;
    }
};
map<string, shared_ptr<Clunion>> Clunion::all;

struct ClunionHierarchy {
    static vector<ClunionHierarchy> all;

    string name;
    shared_ptr<Clunion> root;
    vector<shared_ptr<Clunion>> members;
    vector<Method> virtual_methods;

    void find_members(shared_ptr<Clunion> &u) {
        members.push_back(u);
        for(auto &c: u->derivations) {
            find_members(c);
        }
    }

    void find_virtual_methods() {
        map<string, Method> lookup;
        for(auto &u: members) {
            for(auto &kv: u->methods) {
                auto &m = kv.second;
                if(m.is_virtual()) {
                    if(lookup.find(m.signature) == lookup.end()) {
                        lookup[m.signature] = m;
                        virtual_methods.push_back(m);
                    }
                }
            }
        }
        
    }

    ClunionHierarchy(shared_ptr<Clunion> root_) : root(root_) {
        find_members(root);
        find_virtual_methods();

        name = root->name;
    }

    static void resolve() {
        for(auto &kv: Clunion::all) {
            if(!kv.second->base) {
                all.push_back(kv.second);
            }
        }
    }

    friend ostream &operator<<(ostream &out, ClunionHierarchy &h) {
        return out << "Hierarchy(" << h.root << ")";
    }
};
vector<ClunionHierarchy> ClunionHierarchy::all;

struct SourceBuffer {
    shared_ptr<char> data;
    unsigned length;
};

struct SourceExtractor {
    static map<string, SourceBuffer> file_buffer_cache;

    static SourceBuffer get_file_buffer(const string &path) {
        auto it = file_buffer_cache.find(path);
        if(it != file_buffer_cache.end())
            return it->second;

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
        return file_buffer_cache[path] = {shared_ptr<char>(buf), unsigned(flen)};
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

        shared_ptr<char> fbuf = get_file_buffer(floc.path).data;
        memcpy(rbuf, fbuf.get() + start_offset, len);

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
map<string, SourceBuffer> SourceExtractor::file_buffer_cache;

struct SourceEditor {
    struct Edit {
        enum Type {
            Delete,
            Insert
        } type;
        unsigned int offset;
        unsigned int len;
        string text;

        Edit(Type t, unsigned o, unsigned l, const string &x = "")
            : type(t), offset(o), len(l), text(x) {
        }

        friend bool operator<(const Edit &a, const Edit &b) {
            return a.offset < b.offset;
        }
    };
    vector<Edit> edits;

    void insert(CXSourceLocation loc,
                const string &text,
                int hack = 0) {
        edits.emplace_back(Edit::Insert, file_offset(loc) + hack, text.length(), text);
    }

    void replace(CXSourceLocation start,
                 CXSourceLocation end,
                 const string &text) {
        cout << "replace(" << start << ", " << end << ", '" << text << "')" << endl;
        unsigned s = file_offset(start);
        unsigned e = file_offset(end);

        edits.emplace_back(Edit::Delete, s, e - s, "");
        edits.emplace_back(Edit::Insert, s, text.length(), text);
    }

    void commit(SourceBuffer src,
                ostream &dst) {
        stable_sort(edits.begin(), edits.end());

        unsigned src_offset = 0;
        for(Edit &e: edits) {
            if(e.offset < src_offset) {
                // we're in a deleted region. don't write anything.
            } else {
                unsigned len = e.offset - src_offset;
                if(len) {
                    dst.write(src.data.get() + src_offset, len);
                    src_offset += len;
                }
            }

            switch(e.type) {
            case Edit::Delete:
                src_offset += e.len;
                break;
            case Edit::Insert:
                dst << e.text;
                break;
            default:
                abort();
            }
        }

        unsigned len = src.length - src_offset;
        if(len) {
            dst.write(src.data.get() + src_offset, len);
        }
    }
};

struct SourceGenerator {
    static ostream &parameters(ostream &out,
                               CXCursor method,
                               bool parens = true) {
        if(parens) {
            out << "(";
        }

        vector<CXType> arg_types = get_argument_types(method);
        vector<CXCursor> arg_names = get_arguments(method);
        for(size_t i = 0; i < arg_types.size(); i++) {
            if((i != 0) || !parens)
                out << ", ";
            out << spelling(arg_types[i]) << " ";
            out << spelling(arg_names[i]);
        }

        if(parens) {
            out << ")";
        }
        return out;
    }

    static ostream &signature(ostream &out, CXCursor method) {
        out << spelling(get_return_type(method)) << " "
            << spelling(method);
        parameters(out, method);
        if(::is_const(method)) {
            out << " const";
        }
        return out;
    }
    static string signature(CXCursor method) {
        stringstream ss;
        signature(ss, method);
        return ss.str();
    }

    static ostream &signature_virtual_method_impl(ostream &out, CXCursor method) {
        out << spelling(get_return_type(method)) << " "
            << "__clunion_virtual_" << spelling(method);
        return parameters(out, method);
    }

    static ostream &signature_virtual_method_dispatch(ostream &out,
                                                      const ClunionHierarchy &h,
                                                      const Method &m) {
        return out << spelling(get_return_type(m.cursor)) << " "
                   << spelling(m.cursor)
                   << "(target::" << h.name << " *thiz"
                   << parameters(out, m.cursor, false)
                   << out << ")";
    }

    static ostream &call_virtual_method_dispatch(ostream &out, CXCursor method) {
        if(spelling(get_return_type(method)) != "void") {
            out << "return ";
        }
        out << "dispatch::" << spelling(method) << "(this";
        for(CXCursor arg: get_arguments(method)) {
            out << ", " << spelling(arg);
        }
        return out << ")";
    }

    static ostream &virtual_method_accessor_decl(ostream &out, CXCursor method) {
        assert(is_method(method));
        assert(is_virtual(method));
        if(is_variadic(method)) {
            err(start(method) << ": Variadic virtual functions not supported.");
        }

        return signature(out, method);
    }

    template<typename T, typename U, typename V>
    static ostream &join(ostream &out, const T &glue, const U &cont, const V &write) {
        bool first = true;
        for(auto &x: cont) {
            if(!first) {
                out << glue;
            } else {
                first = false;
            }
            write(out, x);
        }
        return out;
    }

#define p(txt) out << txt << endl

    static ostream &hierarchy_preamble(ostream &out, ClunionHierarchy &h) {
        p("namespace __clunion { namespace clunion_" << h.name << " {");
        p("    namespace target {");
        p("      struct " << h.name << ";");
        p("    }");
        p("    namespace dispatch {");
        p("      " << join(out,
                           "\n      ",
                           h.virtual_methods,
                           [&h](ostream &out, const Method &m) {signature_virtual_method_dispatch(out, h, m) << ";";}));
        p("    }");
        p("    typedef unsigned char vindex_t;");
        p("    enum class Ids {");
        p("      " << join(out,
                           ", ",
                           h.members,
                           [](ostream &out, const Clunion::ptr &c){out << c->name;}));
        p("    };");
        p("}}");

        return out;
    }

    static ostream &clunion_target(ostream &out,
                                   ClunionHierarchy &h,
                                   Clunion::ptr &c) {
        p("namespace __clunion { namespace clunion_" << h.name << " { namespace target {");
        p("} } }");

        return out;
    }

    static void generate(SourceEditor &editor,
                         ClunionHierarchy &h) {
        stringstream out;

        hierarchy_preamble(out, h);
        editor.insert(start(h.root->cursor), out.str());
        reset(out);

        for(Clunion::ptr &c: h.members) {
            p("namespace __clunion { namespace clunion_" << h.name << " { namespace target {");
            p("////////////////////////////////////////////////////");
            p("/// clunion " << c->name);
            p("////////////////////////////////////////////////////");
            editor.insert(start(c->cursor), out.str());
            reset(out);

            for(auto &kv: c->methods) {
                Method &m = kv.second;
                if(m.is_pure_virtual()) {
                    out << m.signature << "{ " << call_virtual_method_dispatch(out, m.cursor) << ";}";
                    editor.replace(start(m.cursor),
                                   end(m.cursor),
                                   out.str());
                    reset(out);
                } else if(m.is_virtual()) {
                    CXSourceLocation body_loc = start(get_child(m.cursor, CXCursor_CompoundStmt));
                    editor.replace(start(m.cursor),
                                   body_loc,
                                   m.signature);

                    p(" {" << call_virtual_method_dispatch(out, m.cursor) << ";}");
                    out << "    " << signature_virtual_method_impl(out, m.cursor) << " ";
                    editor.insert(body_loc, out.str());
                    reset(out);
                }
            }

            p("    vindex_t __clunion_vindex_actual;");
            editor.insert(end(c->cursor),
                          out.str(),
                          -1);
            reset(out);

            p("}}}");
            editor.insert(end(c->cursor), out.str(), 2); //hack
            reset(out);
        }

        p("");
        p("namespace __clunion { namespace clunion_" << h.name << " { namespace target {");
        p("  template<typename T> constexpr");
        p("      const T &max(const T &a, const T &b) {");
        p("          return a > b ? a : b;");
        p("      }");
        p("      template<typename T, typename... U> constexpr");
        p("      const T &max(const T &a, const U &... b) {");
        p("          return max(a, max(b...));");
        p("      }");
        p("      constexpr size_t sizeof_members = max("
          << join(out,
                  ", ",
                  h.members,
                  [](ostream &out, const Clunion::ptr &c){out << "sizeof(" << c->name << ")";})
          << ");");
        p("}}}");
        editor.insert(end(h.members.back()->cursor),
                      out.str(),
                      2);
        reset(out);
    }

    static void generate(SourceEditor &editor) {
        for(auto &h: ClunionHierarchy::all) {
            generate(editor, h);
        }
    }

#undef p
};

Method::Method(CXCursor cursor_)
    : cursor(cursor_)
    , signature(SourceGenerator::signature(cursor)) {
}

int main() {
    CXIndex index = clang_createIndex(1, 1);

    char *args[] = {};
    int nargs = sizeof(args) / sizeof(char*);
    string path_in = "test/hello.cpp";
    CXTranslationUnit tu = clang_createTranslationUnitFromSourceFile(index,
                                                                     path_in.c_str(),
                                                                     nargs, args,
                                                                     0, nullptr);
    CXCursor cursor = clang_getTranslationUnitCursor(tu);

    for(auto &c: find_clunions(cursor)) {
        Clunion::declare(c);
    }
    Clunion::resolve_bases();
    ClunionHierarchy::resolve();

    SourceEditor editor;
    SourceGenerator::generate(editor);
    editor.commit(SourceExtractor::get_file_buffer(path_in),
                  cout);
    
    clang_disposeTranslationUnit(tu);
    
    cout << "COMPLETE" << endl;
}
