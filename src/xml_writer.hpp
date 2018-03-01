// This is a lightweight xml writer taken from https://gist.github.com/sebclaeys/1227644

#ifndef XML_WRITER_HPP
# define XML_WRITER_HPP

# define INDENT "    "
# define NEWLINE "\n"

# include <string>
# include <stack>
# include <iostream>

class Writer
{
public:

    Writer(std::ostream& os) : os(os), tag_open(false), new_line(true) {}
    ~Writer() {}

    Writer& openElt(const char* tag) {
        this->closeTag();
        if (elt_stack.size() > 0)
            os << NEWLINE;
        this->indent();
        this->os << "<" << tag;
        elt_stack.push(tag);
        tag_open = true;
        new_line = false;
        return *this;
    }

    Writer& closeElt() {
        if (tag_open && !new_line) {
            this->elt_stack.pop();
            this->os << "/>";
            tag_open = false;
        }
        else {
            this->closeTag();
            std::string elt = elt_stack.top();
            this->elt_stack.pop();
            if (new_line)
            {
                os << NEWLINE;
                this->indent();
            }
            this->os << "</" << elt << ">";
        }
        new_line = true;
        return *this;
    }

    Writer& closeAll() {
        while (elt_stack.size())
            this->closeElt();
        return *this;
    }

    Writer& attr(const char* key, const char* val) {
        this->os << " " << key << "=\"";
        this->write_escape(val);
        this->os << "\"";
        return *this;
    }

    Writer& attr(const char* key, std::string val) {
        return attr(key, val.c_str());
    }

    Writer& content(std::string val) {
        this->closeTag();
        this->write_escape(val.c_str());
        return *this;
    }

private:
    std::ostream& os;
    bool tag_open;
    bool new_line;
    std::stack<std::string> elt_stack;

    inline void closeTag() {
        if (tag_open)
            {
                this->os << ">";
                tag_open = false;
            }
    }

    inline void indent() {
        for (int i = 0; i < elt_stack.size(); i++)
            os << (INDENT);
    }

    inline void write_escape(const char* str) {
        for (; *str; str++)
            switch (*str) {
            case '&': os << "&amp;"; break;
            case '<': os << "&lt;"; break;
            case '>': os << "&gt;"; break;
            case '\'': os << "&apos;"; break;
            case '"': os << "&quot;"; break;
            default: os.put(*str); break;
            }
    }
};

#endif /* !XML_WRITER_HPP */