// ReSharper disable CppRedundantQualifier
#pragma once
using System::Reflection::AssemblyName;
using System::Threading::Thread;

#include <string>
// ReSharper disable once CppUnusedIncludeDirective
#include <msclr/marshal_cppstd.h>

#include "Codes.h"

using namespace System::Collections;
using namespace Generic;
using namespace System;
using namespace Reflection::Emit;

using namespace std;


ref class Translator
{
private:
	String^ name_ = "build.exe";

	AssemblyBuilder^	assembly_builder_;
	ModuleBuilder^		module_builder_;
	TypeBuilder^		type_builder_;
	MethodBuilder^		method_builder_;
	
	Dictionary<String^, LocalBuilder^>^ variables_;
	
	bool is_working_ = true;


	String^ convert_string(string& str);

public:
	ILGenerator^ il_generator;

	// метки условной конструкции
	Collections::Stack else_labs;
	Collections::Stack ifless_labs;

	// метки цикла с предусловием
	Collections::Stack while_labs;
	Collections::Stack loopless_labs;
	
	Translator();
	~Translator();

	property bool IsWorking
	{
		bool get();
		void set(bool status);
	}
	
	void save();
	
	void add_local_var(string& var, Type^ type);
	
	void assign(string& var);
	void assign_const(string& var, int val);
	void assign_const(string& var, double val);
	void assign_const(string& var, bool val);
	void assign_const(string& var, char val);
	void assign_const(string& var, string& val);

	void write_line(int val);
	void write_line(double val);
	void write_line(bool val);
	void write_line(char val);
	void write_line(string& val);

	void write_line(Type^ type);
	void write_line();
	void read_line(string& var, Type^ type_to_cast);
	void read_line();

	void load_const(int val);
	void load_const(double val);
	void load_const(bool val);
	void load_const(char val);
	void load_const(string& val);

	void cast_to_real();

	void load_local_var(string& name);

	void operation(TokenCodes operation);

	void branch_true(Label label);
	void branch_false(Label label);
	void branch(Label label);

	void create_else_ifless_labels();
	void pop_else_and_mark();
	void pop_ifless_and_mark();

	void create_while_loopless_labels();
	void pop_while_and_mark();
	void pop_loopless_and_mark();
};

