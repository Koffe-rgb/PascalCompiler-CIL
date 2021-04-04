#include "Translator.h"

String^ Translator::convert_string(string& str)
{
	return msclr::interop::marshal_as<String^>(str);
}

Translator::Translator()
{
	const auto assembly_name = gcnew AssemblyName(name_);
	auto domain = Thread::GetDomain();
	assembly_builder_ = domain->DefineDynamicAssembly(assembly_name, AssemblyBuilderAccess::RunAndSave);
	
	module_builder_ = assembly_builder_->DefineDynamicModule(name_, name_);
	module_builder_->CreateGlobalFunctions();
	
	type_builder_ = module_builder_->DefineType("Build_Build");
	
	method_builder_ = type_builder_->DefineMethod(
		"Main_Main", 
		Reflection::MethodAttributes::Public | Reflection::MethodAttributes::Static, 
		Void::typeid, 
		nullptr
	);
	
	il_generator = method_builder_->GetILGenerator();
	
	variables_ = gcnew Dictionary<String^, LocalBuilder^>();
}

Translator::~Translator()
{
	delete assembly_builder_;
	delete module_builder_;
	delete type_builder_;
	delete method_builder_;
	delete il_generator;
	
	delete variables_;
}

bool Translator::IsWorking::get()
{
	return is_working_;
}

void Translator::IsWorking::set(const bool status)
{
	is_working_ = status;
}

void Translator::save()
{
	if (!is_working_) return;
	il_generator->Emit(OpCodes::Ret);
	type_builder_->CreateType();
	assembly_builder_->SetEntryPoint(method_builder_);
	assembly_builder_->Save(name_);
}

void Translator::add_local_var(string& var, Type^ type)
{
	if (!is_working_) return;
	const auto var_name = convert_string(var);
	variables_[var_name] = il_generator->DeclareLocal(type);
}

void Translator::assign(string& var)
{
	if (!is_working_) return;
	const auto var_name = convert_string(var);
	il_generator->Emit(OpCodes::Stloc, variables_[var_name]);
}

void Translator::assign_const(string& var, const int val)
{
	if (!is_working_) return;
	load_const(val);
	assign(var);
}

void Translator::assign_const(string& var, const double val)
{
	if (!is_working_) return;
	load_const(val);
	assign(var);
}

void Translator::assign_const(string& var, const bool val)
{
	if (!is_working_) return;
	assign_const(var, val ? 1 : 0);
}

void Translator::assign_const(string& var, const char val)
{
	if (!is_working_) return;
	assign_const(var, static_cast<int>(val));
}

void Translator::assign_const(string& var, string& val)
{
	if (!is_working_) return;
	load_const(val);
	assign(var);
}

void Translator::write_line(int val)
{
	if (!is_working_) return;
	il_generator->EmitWriteLine(val.ToString());
}

void Translator::write_line(double val)
{
	if (!is_working_) return;
	il_generator->EmitWriteLine(val.ToString());
}

void Translator::write_line(bool val)
{
	if (!is_working_) return;
	il_generator->EmitWriteLine(val.ToString());
}

void Translator::write_line(char val)
{
	if (!is_working_) return;
	il_generator->EmitWriteLine(val.ToString());
}

void Translator::write_line(string& val)
{
	if (!is_working_) return;
	il_generator->EmitWriteLine(convert_string(val));
}

void Translator::write_line(Type^ type)
{
	if (!is_working_) return;	
	auto types = gcnew cli::array<Type^>(1);
	types[0] = type;
	const auto mi = Console::typeid->GetMethod("WriteLine", types);
	il_generator->EmitCall(OpCodes::Call, mi, nullptr);
}

void Translator::write_line()
{
	if (!is_working_) return;
	const auto mi = Console::typeid->GetMethod("WriteLine", Console::typeid->EmptyTypes);
	il_generator->EmitCall(OpCodes::Call, mi, nullptr);
}

void Translator::read_line(string& var, Type^ type_to_cast)
{
	if (!is_working_) return;
	const auto mi = Console::typeid->GetMethod("ReadLine", Console::typeid->EmptyTypes);
	il_generator->EmitCall(OpCodes::Call, mi, nullptr);

	if (type_to_cast == String::typeid)
	{
		assign(var);
		return;
	}

	auto types = gcnew cli::array<Type^>(1);
	types[0] = String::typeid;
	const auto method_info = type_to_cast->GetMethod("Parse", types);
	il_generator->EmitCall(OpCodes::Call, method_info, nullptr);
	assign(var);
}

void Translator::read_line()
{
	if (!is_working_) return;
	const auto mi = Console::typeid->GetMethod("ReadLine", Console::typeid->EmptyTypes);
	il_generator->EmitCall(OpCodes::Call, mi, nullptr);
	il_generator->Emit(OpCodes::Pop);
}

void Translator::load_const(const int val)
{
	if (!is_working_) return;
	il_generator->Emit(OpCodes::Ldc_I4, val);
}

void Translator::load_const(const double val)
{
	if (!is_working_) return;
	il_generator->Emit(OpCodes::Ldc_R8, val);
}

void Translator::load_const(const bool val)
{
	if (!is_working_) return;
	load_const(val ? 1 : 0);
}

void Translator::load_const(const char val)
{
	if (!is_working_) return;
	load_const(static_cast<int>(val));
}

void Translator::load_const(string& val)
{
	if (!is_working_) return;
	const auto str = convert_string(val);
	il_generator->Emit(OpCodes::Ldstr, str);
}

void Translator::cast_to_real()
{
	if (!is_working_) return;
	il_generator->Emit(OpCodes::Conv_R8);
}

void Translator::load_local_var(string& name)
{
	if (!is_working_) return;
	const auto var_name = convert_string(name);
	il_generator->Emit(OpCodes::Ldloc, variables_[var_name]);
}

void Translator::operation(const TokenCodes operation)
{
	if (!is_working_) return;
	switch (operation)
	{
		case TokenCodes::plus:		il_generator->Emit(OpCodes::Add); break;
		case TokenCodes::minus:		il_generator->Emit(OpCodes::Sub); break;
		case slash:					il_generator->Emit(OpCodes::Div); break;
		case star:					il_generator->Emit(OpCodes::Mul); break;
		case modsy:					il_generator->Emit(OpCodes::Rem); break;
		case divsy:					il_generator->Emit(OpCodes::Div); break;
		case TokenCodes::equal:		il_generator->Emit(OpCodes::Ceq); break;
		case later:					il_generator->Emit(OpCodes::Clt); break;
		case TokenCodes::greater:	il_generator->Emit(OpCodes::Cgt); break;
		
		case andsy:					il_generator->Emit(OpCodes::And); break;
		case orsy:					il_generator->Emit(OpCodes::Or);  break;
		
		case notsy:					
			load_const(0);
			il_generator->Emit(OpCodes::Ceq);
		break;
		
		case laterequal: 
			il_generator->Emit(OpCodes::Cgt);
			il_generator->Emit(OpCodes::Ldc_I4_0);
			il_generator->Emit(OpCodes::Ceq);
			break;
		
		case latergreater:
			il_generator->Emit(OpCodes::Ceq);
			il_generator->Emit(OpCodes::Ldc_I4_0);
			il_generator->Emit(OpCodes::Ceq);
			break;
		
		case greaterequal:
			il_generator->Emit(OpCodes::Clt);
			il_generator->Emit(OpCodes::Ldc_I4_0);
			il_generator->Emit(OpCodes::Ceq);
			break;
	}
}

void Translator::branch_true(Label label)
{
	if (!is_working_) return;
	il_generator->Emit(OpCodes::Brtrue, label);
}

void Translator::branch_false(Label label)
{
	if (!is_working_) return;
	il_generator->Emit(OpCodes::Brfalse, label);
}

void Translator::branch(Label label)
{
	if (!is_working_) return;
	il_generator->Emit(OpCodes::Br, label);
}

void Translator::create_else_ifless_labels()
{
	if (!is_working_) return;
	auto else_lab = il_generator->DefineLabel();
	auto end_lab = il_generator->DefineLabel();
	else_labs.Push(else_lab);
	ifless_labs.Push(end_lab);
}

void Translator::pop_else_and_mark()
{
	if (!is_working_) return;
	auto loc = Label(else_labs.Pop());
	il_generator->MarkLabel(loc);
	il_generator->Emit(OpCodes::Nop);
	
}

void Translator::pop_ifless_and_mark()
{
	if (!is_working_) return;
	auto loc = Label(ifless_labs.Pop());
	il_generator->MarkLabel(loc);
	il_generator->Emit(OpCodes::Nop);
}

void Translator::create_while_loopless_labels()
{
	if (!is_working_) return;
	auto while_lab = il_generator->DefineLabel();
	auto loopless_lab = il_generator->DefineLabel();
	while_labs.Push(while_lab);
	loopless_labs.Push(loopless_lab);
}

void Translator::pop_while_and_mark()
{
	if (!is_working_) return;
	auto loc = Label(while_labs.Pop());
	il_generator->MarkLabel(loc);
	il_generator->Emit(OpCodes::Nop);
}

void Translator::pop_loopless_and_mark()
{
	if (!is_working_) return;
	auto loc = Label(loopless_labs.Pop());
	il_generator->MarkLabel(loc);
	il_generator->Emit(OpCodes::Nop);
}