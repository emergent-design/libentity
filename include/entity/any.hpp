#pragma once
#include <typeinfo>
#include <stdexcept>


namespace ent
{
	// Helper structure to hold a generic reference to an object/value along
	// with the type_info so that a function can selectively modify
	// that value based on the typeid. Potentially very dangerous!
	//   * Dangling references - unsafe to use beyond the lifetime of the object it is referencing
	//   * Type safety - casting to the wrong type would be bad, the cast() function is provided to
	//                   check the type and throw an exception if there is a mismatch.
	struct any_ref
	{
		void *value					= nullptr;
		const std::type_info *type	= nullptr;


		template <typename T> any_ref(T &value)
			: value(&value), type(&typeid(value)) {}


		template <typename T> bool is()
		{
			return type && *type == typeid(T);
		}


		template <typename T> T &cast()
		{
			if (this->is<T>())
			{
				return *static_cast<T*>(value);
			}

			throw std::runtime_error(
				std::string("bad conversion of any_ref from ") + type->name() + " to " + typeid(T).name()
			);
		}
	};
}
