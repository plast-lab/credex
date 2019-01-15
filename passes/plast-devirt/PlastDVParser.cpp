#include <iostream>
#include <algorithm>

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

  site.callee = simplify_name(str.substr(ptr, i));
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

PlastMethodSpec::PlastMethodSpec(const DexMethodRef *dm, bool stat_version) {


  this->name.assign(std::string("static_")+(dm->get_name()->str()));
  this->rtype.assign(std::string(dm->get_proto()->get_rtype()->get_name()->str()));
  this->args = new std::vector<std::string> ();
  this->args->push_back(
    dm->get_class()->get_name()->c_str());
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

/* transforms a java-style declared method to a dalvik one
 * example
 * java : lang.java.String int indexOf(lang.java.String, int)
 * dalvik Lcom/java/lang/String; I endsWith(Lcom/java/lang/String;I)
 * mainly it will translate types and won't bother with method prototype
 * annotation differences
 */
void PlastMethodSpec::to_dalvik(void) {
  rtype = PlastDoopParser::type_to_dalvik(rtype);
  cls = PlastDoopParser::type_to_dalvik(cls);
  for (int i=0; i < args->size(); i++)
    args->at(i) = PlastDoopParser::type_to_dalvik(args->at(i));

}

void PlastInvocSite::to_dalvik(void) {
  cls = PlastDoopParser::type_to_dalvik(cls);
  method.to_dalvik();
}


std::string PlastDoopParser::basic_to_dalvik(const std::string& str) {
  if (!str.compare("void"))
    return std::string("V");
  if (!str.compare("boolean"))
    return std::string("Z");
  if (!str.compare("byte"))
    return std::string("B");
  if (!str.compare("short"))
    return std::string("S");
  if (!str.compare("char"))
    return std::string("C");
  if (!str.compare("int"))
    return std::string("I");
  if (!str.compare("long"))
    return std::string("J");
  if (!str.compare("float"))
    return std::string("F");
  if (!str.compare("double"))
    return std::string("D");

  std::string rv = std::string(std::string("L")+str);
  std::replace(rv.begin(), rv.end(), '.', '/');
  rv.push_back(';');

  return rv;
}
//TODO take care of whitespace that may be here because it will cause a problem
std::string PlastDoopParser::type_to_dalvik(const std::string& str) {
  //first consume all the array identifiers

  std::string tmp(str);


  int i = 0,j=tmp.length();
  int total = 0;
  std::string array_anno;
  for (;;) {
    i = tmp.rfind("[]");
    if (i == -1)
      break;
    //burn the findings
    tmp[i] = '*';
    tmp[i+1]= '*';
    array_anno += "[";
    j = i;
  }
  while (j >= 0 && isspace(tmp[j-1]))
    j--;

  std::string tmp1(str.substr(0, j));

  return std::string(array_anno+PlastDoopParser::basic_to_dalvik(tmp1));
}
//simplifies a name from android.os.Parcel.createTypeArray
// to createTypeArray
//TODO maybe change that to be more specific
//have to change deveritualize targets function
std::pair<std::string, std::string> PlastDoopParser::simplify_name(const std::string& str) {

  int idx = str.rfind('.');
  std::string method =  std::string(str.substr(idx+1, str.length()-idx-1));
  std::string cls =  std::string(str.substr(0, idx));
  return std::pair<std::string, std::string>(type_to_dalvik(cls), method);
}
