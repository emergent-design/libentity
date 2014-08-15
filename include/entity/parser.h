#pragma once

#include <entity/entity2.h>


namespace ent
{
	class parser
	{
		public:

			//std::string to(const tree &item, bool pretty = false, int depth = 0);

			template <class T> std::string to(T &item, bool pretty = false)
			{
				static_assert(std::is_base_of<entity2, T>::value, "Item must be derived from entity");

				auto &codec = this->get_codec();

				os result;

				//codec.initialise(result, pretty);

				//this->object(codec, result, item, pretty, depth);

				vref<T>::encode(item, codec, result, pretty, 0);

				//codec.finalise(result, pretty);

				return result.str();
			}

			//void object(codec &codec, os &result, entity2 &item, bool pretty, int depth);

			//tree from(const std::string &text);

			template <class T> typename std::enable_if<std::is_base_of<entity2, T>::value>::type from(const std::string &text)
			{

			}


			// ostream??
			/*virtual void encode(std::ostringstream &dst); // null?
			virtual void encode(std::ostringstream &dst, bool value);
			virtual void encode(std::ostringstream &dst, long value);
			virtual void encode(std::ostringstream &dst, double value);
			virtual void encode(std::ostringstream &dst, const char *value);
			virtual void encode(std::ostringstream &dst, const std::string &value);
			virtual void encode(std::ostringstream &dst, const std::vector<byte> &value);*/


			//virtual void initialise(std::string &text) = 0;

			virtual codec &get_codec() = 0; //{ return this->c; }
	};


	struct json2 : public parser
	{
		struct jcodec : public codec
		{
			void initialise(os &dst, bool pretty) {}
			void finalise(os &dst, bool pretty) {}

			void object_start(os &dst, bool pretty, int depth)
			{
				dst << '{';
				if (pretty) dst << '\n';
			}


			void object_end(os &dst, bool pretty, int depth)
			{
				if (pretty) dst << std::string(2 * depth, ' ');
				dst << '}';
			}


			void array_start(os &dst, bool pretty, int depth)
			{
				dst << '[';
				if (pretty) dst << '\n';
			}

			void array_end(os &dst, bool pretty, int depth)
			{
				if (pretty) dst << std::string(2 * depth, ' ');
				dst << ']';
			}

			void item(os &dst, bool pretty, int depth)
			{
				if (pretty) dst << std::string(2 * depth, ' ');
			}

			void key(os &dst, const std::string &name, bool pretty, int depth)
			{
				if (pretty) dst << std::string(2 * depth, ' ');
				dst << '"' << name << "\":";
				if (pretty) dst << ' ';
			}

			void separator(os &dst, bool last, bool pretty, int depth)
			{
				if (!last)	dst << ",";
				if (pretty)	dst << '\n';
			}

			void encode(os &dst) 									{ dst << "null"; }
			void encode(os &dst, bool value) 						{ dst << (value ? "true" : "false"); }
			void encode(os &dst, long value)						{ dst << value; }
			void encode(os &dst, double value)						{ dst << value; }
			void encode(os &dst, const std::string &value)			{ dst << '"' << escape(value) << '"'; }
			void encode(os &dst, const std::vector<byte> &value)	{ dst << '"' << encode64(value) << '"'; }

			// TODO
			std::string escape(const std::string &value) { return value; }
		};


		virtual codec &get_codec() { return this->jc; }

		jcodec jc;
	};


}
	/*case value::Type::String:	result.append(quote); escape(item.get(string()), result); result.append(quote);			break;
			case value::Type::Binary:	result.append(quote).append(encode64(item.get(vector<byte>()))).append(quote);			break;
			//case value::Type::Number:	convert_number(result, item);															break;
			case value::Type::Number:	result.append(item.is_floating() ? to_string(item.get(0.0)) : to_string(item.get(0)));	break;
			case value::Type::Boolean:	result.append(item.get(false) ? "true" : "false");										break;
			case value::Type::Null:		result.append("null");																	break;
			case value::Type::Object:	result.append(to(item.object(), pretty, depth+1));										break;*/
