#include <iostream>

#include "PlastDVParser.h"




std::string PlastDoopParser::parse_invok_site(std::string str, PlastInvocSite& site) {


  int ptr = 0;
  str = PlastDoopParser::parse_method_spec(str, site.method);

  site.cls = site.method.cls;
  ptr = 0;
  PlastDoopParser::get_callee(str, ptr, site);
  PlastDoopParser::disc_empty(str, ptr);
  site.n = std::stoi(str.substr(ptr, str.length()-ptr));

  
  std::string ns = std::to_string(site.n);
  ptr += ns.length();
  return str.substr(ptr, str.length()-ptr);
}

std::string PlastDoopParser::parse_method_spec(std::string str, PlastMethodSpec& spec) {

  int ptr = 0;
  PlastDoopParser::get_cls_name(str, ptr, spec);
  PlastDoopParser::get_method_p(str, ptr, spec);
  std::cout << spec.cls << std::endl;

  return str.substr(ptr, str.length()-ptr);


}

void PlastDoopParser::get_cls_name(std::string str, int& ptr, PlastMethodSpec& spec) {

  PlastDoopParser::PlastDoopParser::disc_empty(str, ptr);
  ptr++;
  PlastDoopParser::PlastDoopParser::disc_empty(str, ptr);
  int i =0;
  while (str[ptr+i] != ':' && !isspace(str[ptr+i]))
  	i++;

  spec.cls = str.substr(ptr, i);
  ptr += i;
  PlastDoopParser::disc_empty(str, ptr);
  if (str[ptr] != ':')
  	std::cerr << "Problem." << std::endl;
  ptr++;


}

void PlastDoopParser::get_method_p(std::string str, int& ptr, PlastMethodSpec& spec) {

  PlastDoopParser::disc_empty(str, ptr);
  int i = 0;

  while (!isspace(str[ptr+i]))
    i++;
  std::string rtype = str.substr(ptr, i);

  ptr += i;
  PlastDoopParser::disc_empty(str, ptr);
  i = 0;

  while (!isspace(str[ptr+i]) && str[ptr+i] != '(')
    i++;
  std::string name = str.substr(ptr, i);
  ptr += i;

  PlastDoopParser::disc_empty(str, ptr);

  if (str[ptr] != '(')
    std::cerr << "Problem." << std::endl;

  ptr++;

  std::vector<std::string> *vptr = new std::vector<std::string>();

  //now read the arguments
  PlastDoopParser::disc_empty(str, ptr);

  // FIX THIS !
  int first = 0;
  while (str[ptr] != ')') {

    if (first) {
    ptr++;
    PlastDoopParser::disc_empty(str,ptr);
    }
    else
    first = 1;

    i = 0;

    while (!isspace(str[ptr+i]) && str[ptr+i] != ',' && str[ptr+i] != ')')
    i++;
    vptr->push_back(std::string(str.substr(ptr, i)));
    ptr += i;
    PlastDoopParser::disc_empty(str, ptr);

  }
  ptr++;

  spec.args = vptr;
  spec.rtype.assign(rtype);
  spec.name.assign(name);

  if (str[ptr] != '>')
    std::cerr << "Problem." << std::endl;
  ptr++;

}

void PlastDoopParser::get_callee(std::string str, int& ptr, PlastInvocSite& site) {
  if (str[ptr] != '/')
    std::cerr << "Problem." << std::endl;

  ptr++;
  PlastDoopParser::disc_empty(str, ptr);
  int i =0;
  while (str[ptr+i] != '/' && !isspace(str[ptr+i]))
    i++;

  site.callee = str.substr(ptr, i);
  ptr += i;
  PlastDoopParser::disc_empty(str, ptr);
  if (str[ptr] != '/')
    std::cerr << "Problem." << std::endl;
  ptr++;



}


bool operator<(const PlastMethodSpec& l, const PlastMethodSpec& r) {
  if (l.name.compare(r.name) != 0)
    return l.name.compare(r.name) < 0;
  if (l.rtype.compare(r.rtype) != 0)
    return l.rtype.compare(r.rtype) < 0;
  if (l.args->size() != r.args->size())

    return l.args->size() < r.args->size() ;
  for (size_t i=0; i < l.args->size(); i++)
    if (l.args->at(i).compare(r.args->at(i)) != 0)
      return l.args->at(i).compare(r.args->at(i)) < 0;

  return false;
}


bool operator==(const PlastMethodSpec& l, const PlastMethodSpec& r) {
  if (l.name.compare(r.name) != 0)
    return false;
  if (l.rtype.compare(r.rtype))
    return false;

  if (l.args->size() != r.args->size())
    return false;
  for (size_t i=0; i < l.args->size(); i++)
    if (l.args->at(i).compare(r.args->at(i)))
      return false;

  return true;
}

PlastMethodSpec::PlastMethodSpec(const DexMethodRef *dm) {

  this->name.assign((dm->get_name()->str()));
  this->rtype.assign(std::string(dm->get_proto()->get_rtype()->get_name()->str()));
  this->args = new std::vector<std::string> ();

  for (size_t i=0; i < dm->get_proto()->get_args()->get_type_list().size(); i++) {
    this->args->push_back(dm->get_proto()->get_args()->get_type_list()[i]->get_name()->str());
  }
}

bool PlastMethodSpec::compare(const DexMethodRef* dm) const {

  if (this->name.compare(dm->get_name()->str()))
    return false;
  if (this->rtype.compare(dm->get_proto()->get_rtype()->get_name()->str()))
    return false;
  if (this->args->size() != dm->get_proto()->get_args()->get_type_list().size())
    return false;

  for (size_t i=0; i < this->args->size();i++) {

    if (this->args->at(i) !=
    dm->get_proto()->get_args()->get_type_list()[i]->get_name()->str())

      return false;
  }
  return true;

}
