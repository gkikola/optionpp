#ifndef OPTIONPP_HPP
#define OPTIONPP_HPP

#include <initializer_list>
#include <string>
#include <vector>

struct Option {
  char short_name;
  std::string long_name;
  std::string argument;
  std::string description;
  int group = 0;
};

class OptionParser {
public:
  typedef std::vector<Option> option_seq;
  
  OptionParser() { }
  OptionParser(std::initializer_list<Option> opts):
    m_opts(opts.begin(), opts.end()) { }

  void add(const Option& option) { m_opts.push_back(option); }
  void add(char sname, const std::string& lname,
           const std::string& aname, const std::string& desc,
           int group = 0);
  void add(std::initializer_list<Option> opts);

  option_seq& options_accepted() { return m_opts; }
  const option_seq& options_accepted() const { return m_opts; }

  option_seq& options_given() { return m_opts_read; }
  const option_seq& options_given() const { return m_opts_read; }
  
  bool parse(int argc, char* argv[]);
private:
  option_seq m_opts;
  option_seq m_opts_read;
  option_seq m_bad_opts_read;
};

bool operator < (const Option& o1, const Option& o2);

#endif
