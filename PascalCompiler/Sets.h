#pragma once

#include <set>
#include "Codes.h"

using namespace std;

const auto start_block              = set<TokenCodes> { constsy, typesy, varsy, beginsy };
const auto start_constPart          = set<TokenCodes> { constsy, typesy, varsy, beginsy };
const auto start_constDeclaration   = set<TokenCodes> { ident };
const auto start_constant           = set<TokenCodes> { TokenCodes::plus, TokenCodes::minus, charconst, stringconst, ident, intconst, floatconst };
const auto start_typePart           = set<TokenCodes> { typesy, varsy, beginsy };
const auto start_typeDeclaration    = set<TokenCodes> { ident };
const auto start_type               = set<TokenCodes> { arraysy, TokenCodes::plus, TokenCodes::minus, ident, leftpar, intconst, floatconst, charconst, stringconst, arrow };
const auto start_simpleType         = set<TokenCodes> { TokenCodes::plus, TokenCodes::minus, ident, leftpar, intconst, floatconst, charconst, stringconst };
const auto start_varPart            = set<TokenCodes> { varsy };
const auto start_varDeclaration     = set<TokenCodes> { ident };
const auto start_compoundOperator   = set<TokenCodes> { beginsy };
const auto start_operator           = set<TokenCodes> { beginsy, ifsy, whilesy, ident, semicolon, casesy };
const auto start_assignmentOperator = set<TokenCodes> { ident };
const auto start_variable           = set<TokenCodes> { ident };
const auto start_expression         = set<TokenCodes> { TokenCodes::plus, TokenCodes::minus, ident, leftpar, notsy, intconst, floatconst, charconst, stringconst, nilsy };
const auto start_simpleExpression   = set<TokenCodes> { TokenCodes::plus, TokenCodes::minus, ident, leftpar, notsy, intconst, floatconst, charconst, stringconst, nilsy };
const auto start_additive           = set<TokenCodes> { TokenCodes::plus, TokenCodes::minus, orsy };
const auto start_term               = set<TokenCodes> { ident, intconst, floatconst, charconst, stringconst, nilsy, leftpar, notsy };
const auto start_multiplicative     = set<TokenCodes> { star, slash, divsy, modsy, andsy };
const auto start_factor             = set<TokenCodes> { ident, intconst, floatconst, charconst, stringconst, nilsy, leftpar, notsy };
const auto start_ifOperator         = set<TokenCodes> { ifsy };
const auto start_whileOperator      = set<TokenCodes> { whilesy };
const auto start_caseOperator       = set<TokenCodes> { casesy };
const auto start_elementOfVariants  = set<TokenCodes> { TokenCodes::plus, TokenCodes::minus, ident, intconst, floatconst, charconst, stringconst, nilsy };
const auto start_listOfMarks        = set<TokenCodes> { TokenCodes::plus, TokenCodes::minus, ident, intconst, floatconst, charconst, stringconst, nilsy };
const auto start_linkType           = set<TokenCodes> { arrow };

const auto follow_block             = set<TokenCodes> { point, endoffile };
const auto follow_constPart         = set<TokenCodes> { typesy, varsy, beginsy };
const auto follow_typePart          = set<TokenCodes> { varsy, beginsy };
const auto follow_varPart           = set<TokenCodes> { beginsy };
const auto follow_constDeclaration  = set<TokenCodes> { semicolon };
const auto follow_typeDeclaration   = set<TokenCodes> { semicolon };
const auto follow_varDeclaration    = set<TokenCodes> { semicolon };
const auto follow_operator          = set<TokenCodes> { semicolon, endsy };
const auto follow_assignmentOperator= set<TokenCodes> { assign };
const auto follow_simpleExpression  = set<TokenCodes> { TokenCodes::equal, latergreater, later, laterequal, TokenCodes::greater, greaterequal };
const auto follow_factorExpression  = set<TokenCodes> { rightpar };
const auto follow_ifOperatorThen    = set<TokenCodes> { thensy };
const auto follow_ifOperatorElse    = set<TokenCodes> { elsesy };
const auto follow_whileDo           = set<TokenCodes> { dosy };
const auto follow_caseOf            = set<TokenCodes> { ofsy };
const auto follow_caseEnd           = set<TokenCodes> { endsy };
const auto follow_elementOfVariants = set<TokenCodes> { colon };
const auto follow_write_line		= set<TokenCodes> { rightpar };
const auto follow_read_line			= set<TokenCodes> { rightpar };