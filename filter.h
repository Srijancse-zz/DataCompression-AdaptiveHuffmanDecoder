#ifndef FILTER_H
#define FILTER_H

// Abstract class intended to create UNIX-like filters
// The virtual member function process is intended to be defined in
// any derived classes
class Filter {
    protected:
        void processFile(std::string const&);
        void processStream(std::istream&);

        virtual void process(std::string) = 0;

    public:
        void filter(int, char**);
};

#endif
