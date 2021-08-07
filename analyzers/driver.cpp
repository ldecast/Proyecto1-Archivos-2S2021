#include "driver.h"
#include "parser.tab.hh"

int parser_driver::parse(const std::string &archivo)
{
  file = archivo;
  iniciarScanner();
  yy::class_parser parser(*this);
  parser.set_debug_level(false);
  float resultado = parser.parse();
  terminarScanner();
  return resultado;
}
