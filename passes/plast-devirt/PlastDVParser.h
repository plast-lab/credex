#pragma once


#include <string>
#include <vector>


#include "DexClass.h"

class PlastMethodSpec {
  friend class PlastDoopParser;

  public:
  std::string name;
  std::string rtype;
  std::string cls;
  std::vector<std::string> *args;

  PlastMethodSpec():args(NULL){}
  PlastMethodSpec(const DexMethodRef* dm);
  PlastMethodSpec(const DexMethodRef* , bool);  //look for the static version
  PlastMethodSpec(std::string name, std::vector<std::string> * vptr, std::string rtype):
	name(name), rtype(rtype), args(vptr){ }

  bool compare(const DexMethodRef* ) const;

  void to_dalvik();

  ~PlastMethodSpec() { if (args != NULL)delete args; }
};




class PlastInvocSite {
  friend class PlastDoopParser;

  std::string cls;                                //class name
  PlastMethodSpec method;                         //method prototype
  std::pair <std::string, std::string> callee;    //callee's name
  int n;                                         //nth function call whatsoever


  public:
  const std::string& gcls(void) {return cls;}
  const PlastMethodSpec& gmethod(void) {return method;}
  const std::pair<std::string, std::string>& gcallee(void) {return callee;}
  int gn(void) {return n;}

  PlastInvocSite(){}

  void to_dalvik();
};



class PlastDoopParser {
  public:
  static std::string parse_invok_site(std::string, PlastInvocSite&);
  static std::string parse_method_spec(std::string, PlastMethodSpec&);

  static std::string type_to_dalvik(const std::string&);
  static std::string basic_to_dalvik(const std::string&);
  static std::pair<std::string, std::string> simplify_name(const std::string& str);

  private:
  static void get_cls_name(std::string, int&, PlastMethodSpec&);
  static void get_method_p(std::string, int&, PlastMethodSpec&);
  static void get_callee(std::string, int&, PlastInvocSite&);

  static void disc_empty(std::string str, int& ptr) {
    while (isspace(str[ptr]))
      ptr++;
  }



};


bool operator<(const PlastMethodSpec&, const PlastMethodSpec&);
bool operator==(const PlastMethodSpec&, const PlastMethodSpec&);
