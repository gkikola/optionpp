#ifndef OPTIONPP_HPP
#define OPTIONPP_HPP

#include <initializer_list>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

struct OptionDesc {
  char short_name;
  std::string long_name;
  std::string argument_name; //use : before name to indicate optional argument
  std::string description;
  int group = 0;
};

struct Option {
  char short_name;
  std::string long_name;
  std::string argument;
};

class OptionParser {
public:
  typedef std::set<OptionDesc> desc_set;
  typedef std::vector<Option> option_seq;
  typedef std::vector<std::string> arg_seq;

  typedef option_seq::iterator iterator;
  typedef option_seq::const_iterator const_iterator;
  typedef option_seq::reverse_iterator reverse_iterator;
  typedef option_seq::const_reverse_iterator const_reverse_iterator;
  
  OptionParser() = default;
  OptionParser(std::initializer_list<OptionDesc> opts):
    m_opts(opts.begin(), opts.end()) { }

  void allow_bad_opts(bool allow = true) { m_allow_bad_opts = allow; }
  void allow_bad_args(bool allow = true) { m_allow_bad_args = allow; }
  
  void add(const OptionDesc& desc) { m_opts.insert(desc); }
  void add(char sname, const std::string& lname,
           const std::string& aname, const std::string& desc,
           int group = 0);
  void add(std::initializer_list<OptionDesc> opts);

  OptionDesc* lookup(char short_name);
  const OptionDesc* lookup(char short_name) const;
  OptionDesc* lookup(const std::string& long_name);
  const OptionDesc* lookup(const std::string& long_name) const;
  
  bool parse(int argc, char* argv[]);

  iterator begin() { return m_opts_read.begin(); }
  const_iterator begin() const { return m_opts_read.begin(); }
  iterator end() { return m_opts_read.end(); }
  const_iterator end() const { return m_opts_read.end(); }

  reverse_iterator rbegin() { return m_opts_read.rbegin(); }
  const_reverse_iterator rbegin() const { return m_opts_read.rbegin(); }
  reverse_iterator rend() { return m_opts_read.rend(); }
  const_reverse_iterator rend() const { return m_opts_read.rend(); }
  
  const_iterator cbegin() const { return m_opts_read.cbegin(); }
  const_iterator cend() const { return m_opts_read.cend(); }

  const_reverse_iterator crbegin() const { return m_opts_read.crbegin(); }
  const_reverse_iterator crend() const { return m_opts_read.crend(); }
  
  arg_seq& program_args() { return m_prog_args; }
  const arg_seq& program_args() const { return m_prog_args; }
private:
  //return true if argument is to be read next
  bool read_short_args(const std::string& argstr);
  bool read_long_arg(const std::string& argstr);

  bool m_allow_bad_opts = false;
  bool m_allow_bad_args = false;
  
  desc_set m_opts;
  option_seq m_opts_read;
  arg_seq m_prog_args;
  std::string m_last_option_read; //used for error reporting
};

class BadOption : public std::logic_error {
public:
  explicit BadOption(const std::string& what_arg): logic_error(what_arg) { }
  explicit BadOption(const char* what_arg): logic_error(what_arg) { }
};

bool operator < (const OptionDesc& o1, const OptionDesc& o2);

#endif