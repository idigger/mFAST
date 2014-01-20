// Copyright (c) 2013, Huang-Ming Huang,  Object Computing, Inc.
// All rights reserved.
//
// This file is part of mFAST.
//
//     mFAST is free software: you can redistribute it and/or modify
//     it under the terms of the GNU Lesser General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     mFAST is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU Lesser General Public License
//     along with mFast.  If not, see <http://www.gnu.org/licenses/>.
//
#include "codegen_base.h"
#include <cctype>
#include <boost/foreach.hpp>

codegen_base::codegen_base(const char* filebase, const char* fileext)
  : filebase_(filebase)
  , out_((filebase_+fileext).c_str(), std::ofstream::trunc)
{
  if (!out_.is_open()) {
    filebase_ += fileext;
    throw file_open_error(filebase_);
  }
}

void codegen_base::traverse(mfast::dynamic_templates_description& desc)
{
  BOOST_FOREACH(const mfast::field_instruction* inst, desc.defined_type_instructions())
  {
    // we use the second parameter to identify wether the instruction is nested. If the
    // second parameter is not 0, it is nested inside another composite types.
    inst->accept(*this, this);
  }

  for (size_t i = 0; i < desc.size(); ++i)
  {
    desc[i]->accept(*this, this);
  }
}

void codegen_base::traverse(const mfast::group_field_instruction* inst, const char* name_suffix)
{
  std::string saved_cref_scope = cref_scope_.str();
  cref_scope_ << cpp_name(inst) << name_suffix << "_cref::";

  for (std::size_t i = 0; i < inst->subinstructions_count(); ++i)
  {
    // we use the second parameter to identify wether the instruction is nested. If the
    // second parameter is not 0, it is nested inside another composite types.
    inst->subinstruction(i)->accept(*this, 0);
  }
  reset_scope(cref_scope_, saved_cref_scope);
}

void codegen_base::reset_scope(std::stringstream& strm, const std::string& str)
{
  strm.str("");
  strm << str;
}

bool codegen_base::contains_only_templateRef(const mfast::group_field_instruction* inst)
{
  return inst->ref_instruction() != 0 ||
         (inst->subinstructions_count() == 1 && inst->subinstruction(0)->field_type() == mfast::field_type_templateref);
}

std::string
codegen_base::cpp_name(const mfast::field_instruction* inst) const
{
  return cpp_name( inst->name() );
}

std::string codegen_base::cpp_name(const char* name) const
{
  std::string result;
  if (!std::isalpha(name[0]))
    result = "_";
  while (*name != '\x0') {
    char c = *name++;
    if (isalnum(c))
      result += c;
    else
      result += '_';
  }
  return result;
}

bool codegen_base::is_const_field(const mfast::field_instruction* inst) const
{
  if (inst->field_operator() != mfast::operator_constant )
    return false;
  if (inst->field_type() == mfast::field_type_exponent) {
    const mfast::decimal_field_instruction* the_inst = static_cast<const mfast::decimal_field_instruction*>(inst);
    if (the_inst->mantissa_instruction() ==0 || the_inst->mantissa_instruction()->field_operator() != mfast::operator_constant)
      return false;
  }
  return true;
}