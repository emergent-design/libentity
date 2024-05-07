#pragma once

#include <entity/any.hpp>
#include <entity/codec.hpp>
#include <stack>
#include <map>


namespace ent
{
	struct vbase
	{
		virtual ~vbase() {}

		virtual void encode(const codec &c, os &dst, const std::string &name, std::stack<int> &stack) const = 0;
		virtual tree to_tree() const = 0;

		virtual int decode(const codec &c, const std::string &data, int position, int type) = 0;
		virtual void from_tree(const tree &data) = 0;

		// Modify the underlying value with the supplied function. The function is reponsible
		// for handling the type safely based on the type_info. The recurse option gives specific
		// concrete vrefs the option of recursing into containers instead of passing the container
		// to the function. This removes the need for the modification function to handle object
		// traversal. Since this uses any_ref to provide access to the underlying value great care
		// must be taken to handle the type properly.
		virtual void modify(std::function<void(any_ref)> modifier, const bool recurse = true) = 0;
	};

	// Structure for storing the entity description
	typedef std::map<std::string, std::shared_ptr<vbase>> mapping;


	// Template conditionals
	template <typename T> struct fail : std::false_type {};
	template <typename T, typename enable=void> struct vref { static_assert(fail<T>::value, "Item must contain a description and have a default constructor, have you missed a public 'emap()' definition in your entity?"); };


	template <typename T> inline constexpr bool is_not_const = !std::is_const<T>::value;

	// Helper functions to create a vref with automatic type detection
	template <typename T> vref<T> make_vref(T &value)				{ return vref<T>(value); }
	template <typename T> vref<const T> make_vref(const T &value)	{ return vref<const T>(value); }

	// template <typename T> void invalid_const() { static_assert(!std::is_const_v<T>, "Attempt to decode into or modify a const"); }
}
