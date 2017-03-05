#pragma once

#include <map>
#include <set>
#include <list>
#include <vector>
#include <memory>
#include <numeric>
#include <algorithm>
#include <functional>


namespace ent
{
	class qbase
	{
		public:
			virtual ~qbase() {}

		protected:
			std::shared_ptr<qbase> parent = nullptr;
			template <class T> friend class query;
	};


	template <class T> class query : public qbase
	{
		public:

			// Force the default copy constructor (to avoid getting caught by the templated one below)
			query(const query<T> &copy) = default;


			// Construct from initialiser list
			query(std::initializer_list<T> data) : buffer(std::make_shared<std::vector<T>>(data))
			{
				this->setup(*this->buffer);
			}


			// Construct from any container type
			template <class U, class = typename std::enable_if<std::is_same<T, typename U::value_type>::value>::type> query(U &data)
			{
				this->setup(data);
			}


			query<T> where(std::function<bool(const T&)> predicate)
			{
				return {
					*this,
					[=](qbase *c, bool forward) -> T* {
						auto p = static_cast<query<T>*>(c->parent.get());

						for (T *i = p->start(p, forward); i; i = p->next(p)) if (predicate(*i)) return i;

						return nullptr;
					},
					[=](qbase *c) -> T* {
						auto p = static_cast<query<T>*>(c->parent.get());

						for (T *i = p->next(p); i; i = p->next(p)) if (predicate(*i)) return i;

						return nullptr;
					}
				};
			}


			query<T> take(int count)
			{
				return {
					*this,
					[=](qbase *c, bool forward) {
						auto p = static_cast<query<T>*>(c->parent.get());

						static_cast<query<T>*>(c)->counter = 1;

						return count > 0 ? p->start(p, forward) : nullptr;
					},
					[=](qbase *c) {
						auto t = static_cast<query<T>*>(c);
						auto p = static_cast<query<T>*>(c->parent.get());

						return t->counter++ < count ? p->next(p) : nullptr;
					}
				};
			}


			query<T> take_while(std::function<bool(const T&)> predicate)
			{
				return {
					*this,
					[=](qbase *c, bool forward) {
						auto p 	= static_cast<query<T>*>(c->parent.get());
						T *i	= p->start(p, forward);

						return predicate(*i) ? i : nullptr;
					},
					[=](qbase *c) {
						auto p 	= static_cast<query<T>*>(c->parent.get());
						T *i	= p->next(p);

						return predicate(*i) ? i : nullptr;
					}
				};
			}


			query<T> reverse()
			{
				return {
					*this,
					[](qbase *c, bool forward) {
						return static_cast<query<T>*>(c->parent.get())->start(c->parent.get(), !forward);
					},
					[](qbase *c) {
						return static_cast<query<T>*>(c->parent.get())->next(c->parent.get());
					}
				};
			}


			template <class U> query<U> select(std::function<const U(const T&)> operation)
			{
				return query<U> {
					*this,
					[=](qbase *c, bool forward) {
						auto t	= static_cast<query<U>*>(c);
						auto p	= static_cast<query<T>*>(c->parent.get());
						T *i	= p->start(p, forward);

						return i ? &(t->item = operation(*i)) : nullptr;
					},
					[=](qbase *c) {
						auto t	= static_cast<query<U>*>(c);
						auto p	= static_cast<query<T>*>(c->parent.get());
						T *i	= p->next(p);

						return i ? &(t->item = operation(*i)) : nullptr;
					}
				};
			}


			query<T> distinct()
			{
				return {
					*this,
					[](qbase *c, bool forward) {
						auto t = static_cast<query<T>*>(c);
						auto p = static_cast<query<T>*>(c->parent.get());

						t->buffer = std::make_shared<std::vector<T>>();
						for (T *i = p->start(p, forward); i; i = p->next(p))
						{
							if (std::find(t->buffer->begin(), t->buffer->end(), *i) == t->buffer->end())
							{
								t->buffer->emplace_back(*i);
							}
						}

						t->it = std::make_shared<iterators<typename std::vector<T>::iterator>>(t->buffer->begin(), t->buffer->end());
						return t->it->next();
					},
					[=](qbase *c) {
						return static_cast<query<T>*>(c)->it->next();
					}
				};
			}


			template <class U> query<T> order_by(std::function<const U(const T&)> selector, bool descending = false)
			{
				return {
					*this,
					[=](qbase *c, bool forward) {
						auto t = static_cast<query<T>*>(c);
						auto p = static_cast<query<T>*>(c->parent.get());

						t->buffer = std::make_shared<std::vector<T>>();
						for (T *i = p->start(p, true); i; i = p->next(p)) t->buffer->emplace_back(*i);

						if (descending)
						{
							std::stable_sort(t->buffer->begin(), t->buffer->end(), [&](const T &a, const T &b) {
								return t->comparator && selector(a) == selector(b) ? t->comparator(a, b) : selector(a) > selector(b);
							});
						}
						else
						{
							std::stable_sort(t->buffer->begin(), t->buffer->end(), [&](const T &a, const T &b) {
								return t->comparator && selector(a) == selector(b) ? t->comparator(a, b) : selector(a) < selector(b);
							});
						}

						if (forward)	t->it = std::make_shared<iterators<typename std::vector<T>::iterator>>(t->buffer->begin(), t->buffer->end());
						else			t->it = std::make_shared<iterators<typename std::vector<T>::reverse_iterator>>(t->buffer->rbegin(), t->buffer->rend());

						return t->it->next();
					},
					[=](qbase *c) {
						return static_cast<query<T>*>(c)->it->next();
					},
				};
			}


			// This uses comparator passing and is therefore safe to call on any query instance, however it will
			// have no effect on the query unless it follows an order_by/then_by chain.
			template <class U> query<T> then_by(std::function<const U(const T&)> selector, bool descending = false)
			{
				return {
					*this,
					[=](qbase *c, bool forward) {
						auto t = static_cast<query<T>*>(c);
						auto p = static_cast<query<T>*>(c->parent.get());

						if (t->comparator)
						{
							if (descending)	p->comparator = [=](const T &a, const T &b) { return selector(a) == selector(b) ? t->comparator(a, b) : selector(a) > selector(b); };
							else			p->comparator = [=](const T &a, const T &b) { return selector(a) == selector(b) ? t->comparator(a, b) : selector(a) < selector(b); };
						}
						else
						{
							if (descending)	p->comparator = [=](const T &a, const T &b) { return selector(a) > selector(b); };
							else			p->comparator = [=](const T &a, const T &b) { return selector(a) < selector(b); };
						}

						return p->start(p, forward);
					},
					[](qbase *c) {
						return static_cast<query<T>*>(c->parent.get())->next(c->parent.get());
					}
				};
			}


			query<T> concat(const query<T> &src)
			{
				return {
					*this,
					[=](qbase *c, bool forward) {
						auto t		= static_cast<query<T>*>(c);
						t->counter	= forward;

						if (forward)	t->queries = { c->parent, std::make_shared<query<T>>(src) };
						else			t->queries = { std::make_shared<query<T>>(src), c->parent };

						T *i = static_cast<query<T>*>(t->queries.front().get())->start(t->queries.front().get(), forward);

						if (!i)
						{
							t->queries.pop_front();
							return static_cast<query<T>*>(t->queries.front().get())->start(t->queries.front().get(), forward);
						}

						return i;
					},
					[](qbase *c) -> T* {
						auto t = static_cast<query<T>*>(c);

						if (!t->queries.empty())
						{
							T *i = static_cast<query<T>*>(t->queries.front().get())->next(t->queries.front().get());

							if (!i && t->queries.size() > 1)
							{
								t->queries.pop_front();
								return static_cast<query<T>*>(t->queries.front().get())->start(t->queries.front().get(), t->counter);
							}

							return i;
						}

						return nullptr;
					}
				};
			}


			query<T> except(const query<T> &data)
			{
				return {
					*this,
					[=](qbase *c, bool forward) -> T* {
						auto p = static_cast<query<T>*>(c->parent.get());

						for (T *i = p->start(p, forward); i; i = p->next(p))
						{
							if (std::find(data.begin(), data.end(), *i) == data.end()) return i;
						}

						return nullptr;
					},
					[=](qbase *c) -> T* {
						auto p = static_cast<query<T>*>(c->parent.get());

						for (T *i = p->next(p); i; i = p->next(p))
						{
							if (std::find(data.begin(), data.end(), *i) == data.end()) return i;
						}

						return nullptr;
					}
				};
			}


			template<class U, class V> query<U> zip(const query<V> &src, std::function<const U(const T&, const V&)> selector)
			{
				return {
					*this,
					[=](qbase *c, bool forward) {
						auto t		= static_cast<query<U>*>(c);
						auto p		= static_cast<query<T>*>(c->parent.get());
						t->queries	= { std::make_shared<query<V>>(src) };
						T *i		= p->start(p, forward);
						V *j		= static_cast<query<V>*>(t->queries.front().get())->start(t->queries.front().get(), forward);

						return i && j ? &(t->item = selector(*i, *j)) : nullptr;
					},
					[=](qbase *c) -> U* {
						auto t	= static_cast<query<U>*>(c);
						auto p	= static_cast<query<T>*>(c->parent.get());

						if (t->queries.size() == 1)
						{
							T *i	= p->next(p);
							V *j	= static_cast<query<V>*>(t->queries.front().get())->next(t->queries.front().get());

							return i && j ? &(t->item = selector(*i, *j)) : nullptr;
						}

						return nullptr;
					}
				};
			}


			query<T> default_if_empty(const T &defaultValue = T())
			{
				return {
					*this,
					[=](qbase *c, bool forward) {
						T *i = static_cast<query<T>*>(c->parent.get())->start(c->parent.get(), forward);
						return i ? i : &(static_cast<query<T>*>(c)->item = defaultValue);
					},
					[](qbase *c) {
						return static_cast<query<T>*>(c->parent.get())->next(c->parent.get());
					}
				};
			}


			T aggregate(std::function<const T(const T&, const T&)> accumulator)
			{
				return this->begin() == this->end() ? T() : std::accumulate(++this->begin(), this->end(), *this->begin(), accumulator);
			}


			template <class U> U aggregate(const U seed, std::function<const U(const U&, const T&)> accumulator)
			{
				return std::accumulate(this->begin(), this->end(), seed, accumulator);
			}


			template <class U, class V> V aggregate(const U seed, std::function<const U(const U&, const T&)> accumulator, std::function<const V(const U&)> selector)
			{
				return selector(std::accumulate(this->begin(), this->end(), seed, accumulator));
			}


			bool all(std::function<bool(const T&)> predicate)
			{
				for (T *i = start(this, true); i; i = next(this)) if (!predicate(*i)) return false;

				return true;
			}


			bool any(std::function<bool(const T&)> predicate)
			{
				for (T *i = start(this, true); i; i = next(this)) if (predicate(*i)) return true;

				return false;
			}


			bool contains(const T &item)
			{
				for (T *i = start(this, true); i; i = next(this)) if (*i == item) return true;

				return false;
			}


			bool sequence_equal(const query<T> &src)
			{
				query<T> s = src;

				if (s.count() == this->count())
				{
					for (T *i = start(this, true), *j = s.start(&s, true); i && j; i = next(this), j = s.next(&s))
					{
						if (*i != *j) return false;
					}

					return true;
				}

				return false;
			}


			int count(std::function<bool(const T&)> predicate = nullptr)
			{
				int result = 0;

				if (predicate)
				{
					for (T *i = start(this, true); i; i = next(this)) if (predicate(*i)) result++;
				}
				else for (T *i = start(this, true); i; i = next(this), result++);

				return result;
			}


			T element_at(int index)												{ return this->element_at(index, true); }
			T element_at_or_default(int index)									{ return this->element_at(index, false); }

			T first()															{ return this->first_last(nullptr, true, true); }
			T first_or_default()												{ return this->first_last(nullptr, false, true); }
			T first(std::function<bool(const T&)> predicate)					{ return this->first_last(predicate, true, true); }
			T first_or_default(std::function<bool(const T&)> predicate)			{ return this->first_last(predicate, false, true); }

			T last()															{ return this->first_last(nullptr, true, false); }
			T last_or_default()													{ return this->first_last(nullptr, false, false); }
			T last(std::function<bool(const T&)> predicate)						{ return this->first_last(predicate, true, false); }
			T last_or_default(std::function<bool(const T&)> predicate)			{ return this->first_last(predicate, false, false); }

			T single() 															{ return this->single(nullptr, true); }
			T single_or_default()												{ return this->single(nullptr, false); }
			T single(std::function<bool(const T&)> predicate)					{ return this->single(predicate, true); }
			T single_or_default(std::function<bool(const T&)> predicate)		{ return this->single(predicate, false); }

			T min()																{ return this->min_max(true); }
			T max()																{ return this->min_max(false); }
			T sum()																{ return sum<T>(nullptr); }
			double average()													{ return average<T>(nullptr); }
			template <class U> U min(std::function<const U(const T&)> selector)	{ return this->min_max<U>(selector, true); }
			template <class U> U max(std::function<const U(const T&)> selector)	{ return this->min_max<U>(selector, false); }

			query<T> skip(int count)											{ return this->skip(count, nullptr); }
			query<T> skip_while(std::function<bool(const T&)> predicate)		{ return this->skip(0, predicate); }


			template <class U> double average(std::function<const U(const T&)> selector)
			{
				static_assert(std::is_arithmetic<U>::value, "query::average is only suitable for arithmetic types");

				int count 	= 0;
				double sum	= 0;

				if (selector) 	for (T *i = start(this, true); i; i = next(this), count++) sum += (double)selector(*i);
				else			for (T *i = start(this, true); i; i = next(this), count++) sum += (double)*i;

				if (!count) throw std::runtime_error("query::average invalid since query result is empty");

				return sum / (double)count;
			}


			template <class U> U sum(std::function<const U(const T&)> selector)
			{
				static_assert(std::is_arithmetic<U>::value, "query::sum is only suitable for arithmetic types");

				U result = 0;

				if (selector) 	for (T *i = start(this, true); i; i = next(this)) result += selector(*i);
				else			for (T *i = start(this, true); i; i = next(this)) result += *i;

				return result;
			}


			// Will return a new container of items but only where the given container type
			// supports emplace_back.
			template <class U, class = typename std::enable_if<std::is_same<T, typename U::value_type>::value>::type> U to()
			{
				U result;

				// Using insert works for vectors, lists and sets.
				for (T *i = start(this, true); i; i = next(this)) result.insert(result.end(), *i);

				return result;
			}


			template <class U, class V> std::map<U, V> map(std::function<const U(const T&)> key, std::function<const V(const T&)> value)
			{
				std::map<U, V> result;

				for (T *i = start(this, true); i; i = next(this)) result[key(*i)] = value(*i);

				return result;
			}


			std::vector<T> vector()	{ return to<std::vector<T>>(); }
			std::list<T> list()		{ return to<std::list<T>>(); }
			std::set<T> set()		{ return to<std::set<T>>(); }


			// Basic iterator that allows query to be used in a for( : ) each loop without
			// needing to first convert to a vector (avoids unnecessary memory allocation).
			struct iterator : std::iterator<std::input_iterator_tag, T>
			{
				iterator() {}

				iterator(const query<T> &q, bool forward) : q(q)
				{
					this->current = this->q.start(&this->q, forward);
				}

				iterator operator++(int)
				{
					iterator result = *this;
					this->current = this->q.next(&this->q);
					return result;
				}

				iterator &operator++()
				{
					this->current = this->q.next(&this->q);
					return *this;
				}

				bool operator!=(const iterator &i) const	{ return i.current != this->current; }
				bool operator==(const iterator &i) const	{ return i.current == this->current; }
				T &operator*()								{ return *this->current; }

				private:
					query<T> q;
					T *current = nullptr;
			};


			iterator begin() const	{ return iterator(*this, true); }
			iterator end()	const	{ return iterator(); }
			iterator rbegin() const	{ return iterator(*this, false); }
			iterator rend()	const	{ return iterator(); }

		private:

			struct ibase
			{
				virtual T *next() = 0;
			};

			template<class C> struct iterators : ibase
			{
				C current, end;

				iterators(C start, C end) : current(start), end(end) {}

				T *next() { return (current != end) ? &*current++ : nullptr; }
			};


			query() {}

			template <class U> query(query<U> &parent, std::function<T *(qbase *, bool)> start, std::function<T *(qbase *)> next)
			{
				this->parent		= std::make_shared<query<U>>(parent);
				this->start			= start;
				this->next			= next;
			}


			// Used by the query instance that first referenced the source container data.
			// The initialise function will store the begin and end iterators for the given container
			// The next function is used to iterator over the items in the container
			template <class U> void setup(U &data)
			{
				this->start = [&](qbase *c, bool forward) {
					if (forward) 	static_cast<query<T>*>(c)->it = std::make_shared<iterators<typename U::iterator>>(data.begin(), data.end());
					else			static_cast<query<T>*>(c)->it = std::make_shared<iterators<typename U::reverse_iterator>>(data.rbegin(), data.rend());
					return 			static_cast<query<T>*>(c)->it->next();
				};

				this->next = [](qbase *c) {
					return static_cast<query<T>*>(c)->it->next();
				};
			}


			T element_at(int index, bool except)
			{
				T *i = nullptr;

				if (index >= 0)
				{
					int count = 0;
					for (i = start(this, true); i && count < index; i = next(this), count++);
				}

				if (except && !i) throw std::runtime_error("query::element_at invalid since index is out of range");

				return i ? *i : T();
			}


			T first_last(std::function<bool(const T&)> predicate, bool except, bool forward)
			{
				T *i = start(this, forward);

				if (predicate) for (; i && !predicate(*i); i = next(this));

				if (except && !i) throw std::runtime_error("query::first/last invalid since query result is empty");

				return i ? *i : T();
			}


			T single(std::function<bool(const T&)> predicate, bool except)
			{
				T *i = start(this, true);

				if (predicate)		for(; i && !predicate(*i); i = next(this));
				if (except && !i)	throw std::runtime_error("query::single invalid since query result is empty");

				T *n = next(this);

				if (predicate)	for (; n && !predicate(*n); n = next(this));
				if (n)			throw std::runtime_error("query::single more than one result");

				return i ? *i : T();
			}


			T min_max(bool min)
			{
				T *i = start(this, true);
				if (!i) throw std::runtime_error("query::min/max invalid since query result is empty");

				T result = *i;

				if (min)	for (T *i = next(this); i; i = next(this)) result = std::min(result, *i);
				else		for (T *i = next(this); i; i = next(this)) result = std::max(result, *i);

				return result;
			}


			template <class U> U min_max(std::function<const U(const T&)> selector, bool min)
			{
				T *i = start(this, true);
				if (!i) throw std::runtime_error("query::min/max invalid since query result is empty");

				U result = selector(*i);

				if (min)	for (T *i = next(this); i; i = next(this)) result = std::min(result, selector(*i));
				else		for (T *i = next(this); i; i = next(this)) result = std::max(result, selector(*i));

				return result;
			}


			query<T> skip(int count, std::function<bool(const T&)> predicate)
			{
				return {
					*this,
					[=](qbase *c, bool forward) {
						auto p 	= static_cast<query<T>*>(c->parent.get());
						T *i	= p->start(p, forward);

						if (predicate)	for (; i && predicate(*i); i = p->next(p));
						else			for (int j=0; i && j<count; i = p->next(p), j++);

						return i;
					},
					[](qbase *c) {
						return static_cast<query<T>*>(c->parent.get())->next(c->parent.get());
					}
				};
			}


			std::function<T *(qbase *, bool)> start;	// Initialises this link in the query chain and returns the first item
			std::function<T *(qbase *)> next;			// Gets the next item from the chain
			std::shared_ptr<ibase> it;					// Iterator storage
			std::shared_ptr<std::vector<T>> buffer;		// Buffer used for storing initializer_list data or temporary data (see order_by)
			std::list<std::shared_ptr<qbase>> queries;	// Handle parent queries in the correct order (see concat)
			int counter = 0;							// Counter used by operations such as take and skip
			T item;										// Temporary used to return a reference to a modified value

			std::function<bool(const T&, const T&)> comparator = nullptr;

			template <class U> friend class query;
	};


	template <typename T> struct is_container
	{
		private:
			template <typename U> static char test(typename U::iterator*);
			template <typename U> static int test(...);

		public:
			enum { value = sizeof(test<T>(0)) == sizeof(char) };
	};


	/*template <class T> class wrapper
	{
		public:

			typedef T value_type;

			wrapper(T *data, int size) : data(data), size(size) {}

			T *begin()	{ return data; }
			T *end()	{ return data + size; }
			T *rbegin()	{ return data + size - 1; }	// Wrong - needs to decrement
			T *rend()	{ return data - 1; }

	private:

			T *data;
			int size;
	};*/


	template <class T, class = typename std::enable_if<is_container<T>::value>::type> query<typename T::value_type> from(T &data) { return query<typename T::value_type>(data); }
	template <class T> query<T> from(std::initializer_list<T> data) { return query<T>(data); }
	//template <class T> query<T> from(T *data, int size) { return query<T>(wrapper<T>(data, size)); }
}


/* 44/51
-aggregate(reducer)
-aggregate(seed, reducer)
-aggregate(seed, reducer, selector)
-all(predicate)
-any(predicate)
-average()
-concat(range)
-contains(element)
-count()
-count(predicat)
-default_if_empty()
-default_if_empty(default_value)
-distinct()
-element_at(index)
-element_at_or_default(index)
-except(range)
-first()
-first(predicate)
-first_or_default()
-first_or_default(predicate)
group_by(key_selector)
group_by(key_selector, element_selector)
group_join(range, outer_key_selector, inner_key_selector, result_selector)
intersect(range)
join(range, outer_key_selector, inner_key_selector, result_selector)
-last()
-last(predicate)
-last_or_default()
-last_or_default(predicate)
-max()
-min()
-order_by(selector)
-order_by_descending(selector)
-reverse()
-select(selector)
select_many(selector)
-sequence_equal(range)
-single()
-single_or_default()
-skip(count)
-skip_while(predicate)
-sum()
-take(count)
-take_while(predicate)
-then_by(selector)
-then_by_descending(selector)
-to_container()
union(range)
-where(predicate)
-zip(range)
-zip(range, selector)
*/
